#!/bin/bash -e

TMPDIR=`mktemp -d`

BN=0 # Build Number
V=0.0 # Version (major.minor only)
FV=0.0.0 # Full version (major.minor.revision)

mkdir -p $TMPDIR/splatgl-$BN/DEBIAN
mkdir -p $TMPDIR/splatgl-$BN/usr/share/doc/splatgl-$V-$BN
mkdir -p $TMPDIR/splatgl-$BN/usr/lib/x86_64-linux-gnu/

cp release/.libs/libsplatgl-$V.so.$FV $TMPDIR/splatgl-$BN/usr/lib/x86_64-linux-gnu/
cp README.md COPYING $TMPDIR/splatgl-$BN/usr/share/doc/splatgl-$V-$BN/
cp debian/splatgl.control $TMPDIR/splatgl-$BN/DEBIAN/control

mkdir -p $TMPDIR/splatgl-dev-$BN/DEBIAN
mkdir -p $TMPDIR/splatgl-dev-$BN/usr/include/splatgl/
mkdir -p $TMPDIR/splatgl-dev-$BN/usr/lib/x86_64-linux-gnu/

cp include/splat.h $TMPDIR/splatgl-dev-$BN/usr/include/splatgl
cp release/.libs/libsplatgl.a $TMPDIR/splatgl-dev-$BN/usr/lib/x86_64-linux-gnu/splatgl.a
cp debian/splatgl-dev.control $TMPDIR/splatgl-dev-$BN/DEBIAN/control

pushd $TMPDIR
dpkg-deb --build splatgl-$BN
dpkg-deb --build splatgl-dev-$BN
popd

mv $TMPDIR/splatgl-$BN.deb .
mv $TMPDIR/splatgl-dev-$BN.deb .

rm -rf $TMPDIR
