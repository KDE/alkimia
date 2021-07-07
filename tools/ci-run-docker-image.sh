#!/bin/sh
#
# @author Ralf Habacker <ralf.habacker@freenet.de>
#

# enter alkimia source dir
# cd ~/src/alkimia

#inside docker run
cat << EOF
#inside docker run the following commands

cd /mnt

ci_variant=kf5 tools/ci-install.sh
ci_variant=kf5 tools/ci-build.sh

# or

ci_variant=kde4 tools/ci-install.sh
ci_variant=kde4 tools/ci-build.sh
EOF

sudo docker pull opensuse/leap:15.2
sudo docker run -v $PWD:/mnt -it opensuse/leap:15.2 /bin/bash

