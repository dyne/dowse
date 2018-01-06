#!/bin/sh
# This is executed as root when jenkins chroots

# Jenkins needs a prepared chroot for this to work. Our instance is amd64, so
# we utilize qemu and binfmt-misc to enable chrooting into armhf userspace.
# After the script finishes, it is then possible to archive the .tar.gz
# artifact. Jenkins also needs to be able to use sudo without a password.
#
# The Jenkins shell script that uses this is the following:
#
#	CHROOTDIR="/var/lib/jenkins/chroots/dowse-ascii"
#	DOWSESRC="/home/dowse/dowse-src"
#
#	sudo mount --rbind /dev "$CHROOTDIR"/dev
#	sudo mount --rbind /sys "$CHROOTDIR"/sys
#	sudo mount -t proc /proc "$CHROOTDIR"/proc
#
#	sudo chroot "$CHROOTDIR" "$DOWSESRC"/ops/jenkins-build.sh
#
#	sudo umount -R "$CHROOTDIR"/sys "$CHROOTDIR"/proc "$CHROOTDIR"/dev
#	sudo mv -v "$CHROOTDIR/$DOWSESRC"/destbuild/*.tar.gz "$WORKSPACE" || exit 1
#

set -xe

DOWSESOURCE="/home/dowse/dowse-src"

cd "$DOWSESOURCE"
git clean -xdf
cd src/redis && git clean -xdf && cd -

cat <<EOF > build-dowse.sh
#!/bin/sh
set -xe
git pull origin master
git submodule update --init --checkout
make
EOF

sh build-dowse.sh || exit 1

mkdir -p destbuild
make DESTDIR=/home/dowse/dowse-src/destbuild install || exit 1

cd destbuild
tar czf "$DOWSESOUCE"/dowse-armhf-"$(date +%Y%m%d)".tar.gz . || exit 1
