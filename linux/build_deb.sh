#!/bin/bash

dir=`pwd`

if [ $# -ne 1 ]
then
    echo "1 arg needed to build deb, ./build.sh <version>"
    exit
fi

ver=$1

rm -rf oneline-*
svn export ../ oneline-$ver
tar cfz oneline-$ver.tar.gz oneline-$ver/
cd oneline-$ver
dh_make -e jsolbrig@github.net -f ../oneline-$ver.tar.gz -c gpl
cp ../debian/* debian/
dpkg-buildpackage -rfakeroot

cd ..

#mkdir repository
#mkdir repository/binary
#mkdir repository/source
#cp *.deb repository/binary/
#cp *.orig.* *.diff.gz *.dsc  repository/source/
#cd repository
#dpkg-scanpackages binary /dev/null | gzip -9c > binary/Packages.gz
#dpkg-scansources source /dev/null | gzip -9c > source/Sources.gz
