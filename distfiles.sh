#!/bin/sh

version=$(cat version)
DESTDIR="Pigale-"$version
mkdir ../$DESTDIR
dir="tgraph"
rsync -R -r $dir/*.cpp $dir/*.pro $dir/*.awk  ../$DESTDIR
#rsync -R -r lib/*.* ../$DESTDIR
dir="qt"
rsync -R -r $dir/*.cpp $dir/*.h $dir/*.pro $dir/*.awk $dir/pigale.ico $dir/pigale.icns $dir/pigale.rc $dir/icones/* $dir/version ../$DESTDIR
rsync -R -r $dir/*.ts $dir/*.qm  ../$DESTDIR
dir="ClientServer"
rsync -R -r $dir/*.cpp $dir/*.pro $dir/*.h $dir/*.txt  ../$DESTDIR
dir="UsingTgraph"
rsync -R -r $dir/*.cpp $dir/*.pro $dir/*.txt ../$DESTDIR
dir="freeglut"
#rsync -R -r $dir/*.c $dir/*.h $dir/*.pro  ../$DESTDIR
rsync -R -r $dir/*.pro  ../$DESTDIR
rsync -R -r $dir/src/*.c $dir/src/*.h ../$DESTDIR
rsync -R -r $dir/src/mswin/*.c $dir/src/mswin/*.h ../$DESTDIR
rsync -R -r $dir/src/util/*.c $dir/src/util/*.h ../$DESTDIR
rsync -R -r $dir/include/GL/*.h ../$DESTDIR
dir="cgi"
rsync -R -r $dir/*.cpp $dir/*.h $dir/*.pro  $dir/*.txt  ../$DESTDIR
dir="images"
rsync -R -r $dir/* ../$DESTDIR
dir="incl"
rsync -R -r $dir/* ../$DESTDIR
rsync -R -r tgf/* macro/* ../$DESTDIR
rsync -R -r Doc/* translations/* ../$DESTDIR
rsync -R -r incl/* ../$DESTDIR
rsync AUTHORS *.pro distfiles.sh runqmake.sh w*.bat *.txt version ../$DESTDIR
cd ..
tar -czvf pigaleSave/$DESTDIR.tar.gz $DESTDIR 
#rm -Rf $DESTDIR
