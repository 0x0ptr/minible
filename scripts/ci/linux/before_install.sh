#!/bin/bash
set -ev

SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
source $SCRIPTDIR/../funcs.sh

# Docker
docker pull mooltipass/minible-win-builder
docker pull mooltipass/minible-launchpad
docker pull mooltipass/minible-upload

#setup the source dir for ubuntu deb
rm -fr $HOME/build-debs && mkdir -p $HOME/build-debs
echo -e $LAUNCHPAD_GPG_PUB > $HOME/build-debs/gpgkey_pub.asc
echo -e $LAUNCHPAD_GPG_PRIV > $HOME/build-debs/gpgkey_sec.asc
echo "$LAUNCHPAD_GPG_PASS" > $HOME/build-debs/passphrase.txt
echo -e $LAUNCHPAD_SSH_KEY > $HOME/build-debs/ssh_launchpad.key
