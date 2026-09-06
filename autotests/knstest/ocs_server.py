#!/usr/bin/env python3
#
# SPDX-FileCopyrightText: 2026 Thomas Baumgart <tbaumgart@kde.org> (with AI support)
# SPDX-License-Identifier: LGPL-2.1-or-later
#
# Minimal, fully local OCS (Open Collaboration Services) server used to exercise
# the KHotNewStuff update flow of AlkNewStuffEngine without any external network
# access.
#
# It implements just enough of the OCS/Attica protocol that KNSCore needs to:
#   1. discover a provider          -> GET /ocs/providers.xml
#   2. query available content      -> GET /ocs/v1/content/data
#   3. download a payload           -> GET /ocs/v1/content/download/<id>/<n>
#
# The single content entry is reported at version "2.0". A test seeds the local
# KNS registry with the same entry at version "1.0", so the engine must report
# exactly one updateable entry.
#
# Usage:
#   ocs_server.py [--port N] [--host 127.0.0.1] [--version 2.0]
#
# On startup it prints a single line to stdout:
#   LISTENING <host> <port>
# so a parent process can read the actually-bound port (use --port 0 for an
# ephemeral port) before continuing.

import argparse
import sys
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse

# The content entry advertised by the server. Bump CONTENT_VERSION (or pass
# --version) to make it newer than the seeded registry entry.
CONTENT_ID = "1000"
CONTENT_NAME = "Test Quote Source [Currency]"
CONTENT_CATEGORY_ID = "560"
CONTENT_CATEGORY_NAME = "Alkimia Online Quotes"
CONTENT_VERSION = "2.0"

PROVIDERS_XML = """<?xml version="1.0"?>
<providers>
  <provider>
    <id>{base}/ocs/v1/</id>
    <location>{base}/ocs/v1/</location>
    <name>Alkimia Local Test Store</name>
    <icon></icon>
    <termsofuse>{base}/</termsofuse>
    <register>{base}/register</register>
    <services>
      <content></content>
    </services>
  </provider>
</providers>
"""

# Attica needs to validate the provider before using it: it issues a
# "person/check" style request or simply trusts the location. For the KNS
# content flow the important endpoint is content/data. We answer any
# unknown /ocs/v1/* GET with an OCS "ok" envelope so provider validation
# does not fail.
OCS_OK_EMPTY = """<?xml version="1.0"?>
<ocs>
  <meta>
    <status>ok</status>
    <statuscode>100</statuscode>
    <message></message>
  </meta>
  <data/>
</ocs>
"""

# KNSCore resolves the human-readable category name from the knsrc
# (Categories=Alkimia Online Quotes) into a numeric id via this endpoint
# before it can issue a content/data search.
CATEGORIES_XML = """<?xml version="1.0"?>
<ocs>
  <meta>
    <status>ok</status>
    <statuscode>100</statuscode>
    <message></message>
    <totalitems>1</totalitems>
    <itemsperpage>1</itemsperpage>
  </meta>
  <data>
    <category>
      <id>{catid}</id>
      <name>{catname}</name>
      <displayName>{catname}</displayName>
    </category>
  </data>
</ocs>
""".format(catid=CONTENT_CATEGORY_ID, catname=CONTENT_CATEGORY_NAME)


def content_data_xml(base, version):
    payload = "{base}/ocs/v1/content/download/{cid}/1".format(base=base, cid=CONTENT_ID)
    return """<?xml version="1.0"?>
<ocs>
  <meta>
    <status>ok</status>
    <statuscode>100</statuscode>
    <message></message>
    <totalitems>1</totalitems>
    <itemsperpage>1</itemsperpage>
  </meta>
  <data>
    <content details="full">
      <id>{cid}</id>
      <name>{name}</name>
      <version>{version}</version>
      <typeid>{catid}</typeid>
      <typename>{catname}</typename>
      <language></language>
      <personid>testauthor</personid>
      <created>2026-01-01T00:00:00+00:00</created>
      <changed>2026-06-01T00:00:00+00:00</changed>
      <downloads>42</downloads>
      <score>50</score>
      <description>Local test quote source</description>
      <summary>Local test quote source</summary>
      <changelog></changelog>
      <homepage>{base}/p/{cid}</homepage>
      <licensetype>MIT</licensetype>
      <licensename>MIT</licensename>
      <preview1></preview1>
      <smallpreview1></smallpreview1>
      <downloadlink1>{payload}</downloadlink1>
      <downloadname1>Test-{version}.zip</downloadname1>
      <downloadsize1>16</downloadsize1>
      <downloadgpgsignature1></downloadgpgsignature1>
      <downloadpackagename1></downloadpackagename1>
      <tags>mit-license,addon,alkimia</tags>
    </content>
  </data>
</ocs>
""".format(
        cid=CONTENT_ID,
        name=CONTENT_NAME,
        version=version,
        catid=CONTENT_CATEGORY_ID,
        catname=CONTENT_CATEGORY_NAME,
        base=base,
        payload=payload,
    )


# A tiny but valid ZIP archive containing a single file "Test.txt".
# KNS uncompresses "archive" payloads; the contents are irrelevant to the
# update-detection test, so any well-formed zip works.
def make_zip():
    import io
    import zipfile

    buf = io.BytesIO()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("Test.txt", "local test quote source\n")
    return buf.getvalue()


class Handler(BaseHTTPRequestHandler):
    # Silence default per-request logging; enable with --verbose.
    def log_message(self, fmt, *args):
        if self.server.verbose:
            sys.stderr.write("[ocs_server] " + (fmt % args) + "\n")

    def _base_url(self):
        host = self.headers.get("Host")
        if not host:
            host = "{}:{}".format(*self.server.server_address)
        return "http://{}".format(host)

    def _send(self, body, content_type="application/xml"):
        if isinstance(body, str):
            body = body.encode("utf-8")
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        path = urlparse(self.path).path
        base = self._base_url()

        if path == "/ocs/providers.xml":
            self._send(PROVIDERS_XML.format(base=base))
            return

        if path.rstrip("/").endswith("/content/categories"):
            self._send(CATEGORIES_XML)
            return

        # Single content fetch: /ocs/v1/content/data/<id>
        # (used by the Updates filter to compare installed vs. server version)
        stripped = path.rstrip("/")
        if "/content/data/" in stripped:
            self._send(content_data_xml(base, self.server.content_version))
            return

        if stripped.endswith("/content/data"):
            self._send(content_data_xml(base, self.server.content_version))
            return

        if "/content/download" in path:
            self._send(make_zip(), content_type="application/zip")
            return

        # Any other OCS call: answer with an empty "ok" envelope so provider
        # validation and capability probing succeed.
        if path.startswith("/ocs/"):
            self._send(OCS_OK_EMPTY)
            return

        self.send_response(404)
        self.end_headers()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=0, help="0 = ephemeral")
    ap.add_argument("--version", default=CONTENT_VERSION,
                    help="advertised content version")
    ap.add_argument("--verbose", action="store_true")
    args = ap.parse_args()

    server = ThreadingHTTPServer((args.host, args.port), Handler)
    server.verbose = args.verbose
    server.content_version = args.version
    host, port = server.server_address

    # Announce the bound address so the parent can read the real port.
    sys.stdout.write("LISTENING {} {}\n".format(host, port))
    sys.stdout.flush()

    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
