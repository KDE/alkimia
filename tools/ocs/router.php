<?php
/*
 * Copyright 2026 Ralf Habacker <ralf.habacker@freenet.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Web service for a simple store to test access with knewstuff
 */

$uri    = $_SERVER['REQUEST_URI'];
$path = parse_url($uri, PHP_URL_PATH);
$host = $_SERVER['HTTP_HOST'];
$method = $_SERVER['REQUEST_METHOD'];

// 1. Compatibility with the embedded php server (php -S)
if (php_sapi_name() === 'cli-server') {
    // If the file exists, instruct the PHP server to serve it directly.
    $file = __DIR__ . $path;
    if (file_exists($file) && !is_dir($file)) {
        return false;
    }
    // otherwise dump request
    file_put_contents('php://stderr', sprintf("[%s] %s uri:%s path:%s\n", date('H:i:s'), $method, $uri, $path));
}

$baseUrl = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] === 'on' ? "https" : "http") . "://" . $_SERVER['HTTP_HOST'];

// Determine the OCS version exactly once.
$version = str_starts_with($path, '/ocs/v2/') ? '2.0' : '1.0';
$versionNumber = $version === '2.0' ? '2' : '1';
$zipName = 'quote-v' . $versionNumber . '.zip';
$downloadVersion = $version === '2.0' ? '2026-09-12' : '2026-09-11';

// 1. Providers Info
if (strpos($path, '/providers.xml') !== false) {
    header('Content-Type: application/xml; charset=utf-8');
    echo '<?xml version="1.0"?>
    <providers>
      <provider>
        <id>alkimia-test-store</id>
        <location>' . $baseUrl . '/ocs/v' . $versionNumber . '/</location>
        <name>Alkimia Local Test Store</name>
        <termsofuse>' . $baseUrl . '/</termsofuse>
        <register>' . $baseUrl . '/register</register>
        <services>
          <content></content>
        </services>
      </provider>
    </providers>';
    exit;
}

// 2. categories (/ocs/v1/content/categories)
if (strpos($path, '/content/categories') !== false) {
    header('Content-Type: application/xml; charset=utf-8');
    echo '<?xml version="1.0"?>
    <ocs>
      <meta>
        <status>ok</status>
        <statuscode>100</statuscode>
        <message/>
        <totalitems>1</totalitems>
        <itemsperpage>1</itemsperpage>
      </meta>
      <data>
        <category>
          <id>560</id>
          <name>Alkimia Online Quotes</name>
          <displayName>Alkimia Online Quotes</displayName>
        </category>
      </data>
    </ocs>';
    exit;
}

// 3. Content Data (Both /content/data and /content/data/<id>)
if (strpos($path, '/content/data') !== false) {
    header('Content-Type: application/xml; charset=utf-8');
    echo '<?xml version="1.0"?>
    <ocs>
      <meta>
        <status>ok</status>
        <statuscode>100</statuscode>
        <message/>
        <totalitems>1</totalitems>
        <itemsperpage>1</itemsperpage>
      </meta>
      <data>
        <content details="full">
          <id>1000</id>
          <name>Test Quote Source [Currency]</name>
          <version>' . $version . '</version>
          <typeid>560</typeid>
          <typename>Alkimia Online Quotes</typename>
          <personid>testauthor</personid>
          <summary>Local test quote source</summary>
          <downloadlink1>' . $baseUrl . '/ocs/v' . $versionNumber . '/content/download/1000/1</downloadlink1>
          <downloadname1>' . $zipName . '</downloadname1>
        </content>
      </data>
    </ocs>';
    exit;
}

// 4. Download link for metadata endpoint (Called when you click “Install”)
if (strpos($path, 'content/download/') !== false) {
    $zipPath = __DIR__ . '/' . $zipName;
    $downloadMd5 = file_exists($zipPath) ? md5_file($zipPath) : '';
    echo '<?xml version="1.0" encoding="UTF-8"?>
    <ocs>
      <meta>
        <status>ok</status>
        <statuscode>100</statuscode>
      </meta>
      <data>
        <content details="download">
          <downloadway>1</downloadway>
          <downloadlink>' . $baseUrl . '/ocs/' . $zipName . '</downloadlink>
          <mimetype>application/zip</mimetype>
          <gpgfingerprint/>
          <gpgsignature/>
          <packagename/>
          <repository/>
          <download_package_type/>
          <download_package_arch/>
          <download_version>' . $downloadVersion . '</download_version>
          <downloadtags>data##mimetype=application/zip</downloadtags>
          <downloadmd5sum>' . $downloadMd5 . '</downloadmd5sum>
        </content>
      </data>
    </ocs>';
    exit;
}

// 5. Actual file download
if ($path === '/ocs/quote-v1.zip' || $path === '/ocs/quote-v2.zip') {
    $fileName = basename($path);
    $filePath = __DIR__ . '/' . $fileName;
    if (file_exists($filePath)) {
        header('Content-Type: application/zip');
        header('Content-Disposition: attachment; filename="' . $fileName . '"' );
        header('Content-Length: ' . filesize($filePath));
        readfile($filePath);
        exit;
    }
}

if (str_ends_with($path, '/ocs') || str_ends_with($path, '/ocs/')) {
    header('Content-Type: text/html; charset=utf-8');
    echo '
<html>
  <body>
    <p>
      This service is used to test the connection to online quote sources.
    </p>
    <p>
      It is intended for use with KMyMoney, the online qoutes editor, or any other application that utilizes the online qoutes support provided by alkimia.
    </p>
    <p>
      To use this service, create a new <b>KNewStuff resource and a configuration file</b>:
    </p>
      <pre>    cat > test-store.knsrc
    [KNewStuff3]
    ProvidersUrl='. $baseUrl .'/v1/providers.xml
    Categories=Alkimia Online Quotes
    TargetDir=test-store/quotes
    Uncompress=archive
      </pre>
    <p>
      For a quick check of the service then run:
      <pre>    knewstuff-dialog test-store.knsrc
      </pre>
    </p>
    <p>
      For debugging you can run:
      <pre>    QT_LOGGING_RULES="kf.*.debug=true" knewstuff-dialog test-store.knsrc
      </pre>
    </p>
  </body>
</html>';
    exit;
}

echo "url $path not found";

http_response_code(404);
