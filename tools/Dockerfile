FROM opensuse/leap
MAINTAINER openSUSE KDE Maintainers <opensuse-kde@opensuse.org>

ENV ci_variant=kf5-webengine
COPY tools/ci-install.sh .
RUN ./ci-install.sh

# For D-Bus to be willing to start it needs a Machine ID
RUN dbus-uuidgen > /etc/machine-id

# Certain X11 based software is very particular about permissions and ownership around /tmp/.X11-unix/ so ensure this is right
RUN mkdir /tmp/.X11-unix/ && chown root:root /tmp/.X11-unix/ && chmod 1777 /tmp/.X11-unix/

# We need a user account to do things as, and we need specific group memberships to be able to access video/render DRM nodes
#RUN groupadd -g 44 host-video && groupadd -g 109 host-render && useradd -d /home/user/ -u 1000 --user-group --create-home -G video,host-video,host-render --shell /usr/bin/bash user

# Switch to our unprivileged user account
#USER user

