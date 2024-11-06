#!/bin/sh
#
# @author Ralf Habacker <ralf.habacker@freenet.de>
#

#
: "${image:=opensuse/leap:15.6}"

# enter source dir
# cd ~/src/alkimia

case "$ci_distro" in
    (*leap)
        image=opensuse/leap:15.6
        ;;
    (*tumbleweed)
        image=opensuse/tumbleweed
        ;;
esac

#inside docker run
cat << EOF
#inside docker run the following commands

cd /mnt

ci_variant=kf6 tools/ci-install.sh
ci_variant=kf6 tools/ci-build.sh

# or

ci_variant=kf5 tools/ci-install.sh
ci_variant=kf5 tools/ci-build.sh

# or

ci_variant=kf4 tools/ci-install.sh
ci_variant=kf4 tools/ci-build.sh
EOF

options=
if [ "$1" == "--use-host-display" ]; then
    options="-v $HOME/.Xauthority:/root/.Xauthority:rw --env=DISPLAY --net=host"
    shopts="export DISPLAY=$DISPLAY"
fi

sudo docker pull $image
sudo docker run \
    -v $PWD:/mnt \
    $options \
    -it $image \
    /bin/bash -c "cd /mnt; $shopts export ci_distro=$ci_distro; export ci_variant=$ci_variant; tools/ci-install.sh; tools/ci-build.sh; bash"

