#!/bin/sh
#
# @author Ralf Habacker <ralf.habacker@freenet.de>
#

# the docker image to use
: "${ci_image:=opensuse/leap:15.6}"

# the distribution to use
: "${ci_distro:=}"

# enter source dir
# cd ~/src/alkimia

case "$ci_distro" in
    (opensuse-leap|opensuse-leap-15.6)
        ci_image=opensuse/leap:15.6
        ;;
    (opensuse-tumbleweed)
        ci_image=opensuse/tumbleweed
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
shopts="export ci_parallel=$ci_parallel; export ci_distro=$ci_distro; export ci_variant=$ci_variant; export ci_host=$ci_host;"
if [ "$1" == "--use-host-display" ]; then
    options="-v $HOME/.Xauthority:/root/.Xauthority:rw --env=DISPLAY --net=host"
    shopts="export DISPLAY=$DISPLAY;"
fi

sudo docker pull $ci_image
sudo docker run \
    -v $PWD:/mnt \
    $options \
    -it $ci_image \
    /bin/bash -c "cd /mnt; $shopts tools/ci-install.sh; tools/ci-build.sh; bash"

