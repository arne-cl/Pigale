#!/bin/sh

case "$OSTYPE" in
  solaris*) SYSTEM"solaris" ;;
  darwin*)  SYSTEM="osx" ;; 
  linux*)   SYSTEM="linux" ;;
  bsd*)     SYSTEM="bsd" ;;
  msys*)    SYSTEM="windows" ;;
  *)        SYSTEM"unknown: $OSTYPE" ;;
esac

echo $SYSTEM
echo $QTDIR>qtdir
echo $SYSTEM>systeme
VERSION='1.3.24'
export VERSIONH="#define VERSION \"$VERSION\""
echo $VERSIONH > ./qt/version.h
echo $VERSION > ./version

#UBUNTU    QMAKE=/usr/bin/qmake-qt4
QMAKE=$QTDIR/bin/qmake 

if [ $SYSTEM  = "osx" ]; then
    echo "Creating Xcode projects qtdir:$QTDIR"
    ${QMAKE}  -spec macx-xcode  pigale.pro    
    cd tgraph
    ${QMAKE}  -spec macx-xcode  tgraph.pro
    cd ../qt
    ${QMAKE}  -spec macx-xcode  qt.pro
    cd ../ClientServer
    ${QMAKE}  -spec macx-xcode  ClientServer.pro
    cd ../UsingTgraph
    ${QMAKE}  -spec macx-xcode  UsingTgraph.pro
    cd ../cgi
    ${QMAKE}  -spec macx-xcode  cgi.pro
    cd ..
fi

echo "Creating Makefiles"
${QMAKE}  -o Makefile  pigale.pro
echo $VERSIONH>version.h
cd tgraph
${QMAKE}  -o Makefile  tgraph.pro
if [ $SYSTEM  != "osx" ]; then
	cd ../freeglut
	${QMAKE}  -o Makefile  freeglut.pro
	fi
cd ../qt
echo $VERSIONH>version.h
${QMAKE}  -o Makefile  qt.pro
cd ../ClientServer
${QMAKE}  -o Makefile  ClientServer.pro
cd ../cgi
${QMAKE}  -o Makefile  cgi.pro
cd ../UsingTgraph
${QMAKE}  -o Makefile  UsingTgraph.pro
cd ..
