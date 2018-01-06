#!/bin/sh
# This is executed as root when jenkins chroots

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
