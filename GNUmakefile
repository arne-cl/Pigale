all: libtgraph  libqt

libtgraph : tgraph/*.cpp
	cd tgraph ;make -e

libqt : qt/*.cpp
	cd qt ;make -e

server : ClientServer/*.cpp
	cd ClientServer ;make -e

clean:
	cd tgraph ; make -e clean
	cd qt ; make -e clean
	cd ClientServer ; make -e clean
	cd incl/TAXI ;rm -f *~ *.hh
unix:
	cd tgraph ; dos2unix -k *.cpp
	cd qt ; dos2unix -k *.h ; dos2unix -k *.cpp
	cd ClientServer ; dos2unix -k *.h ; dos2unix -k *.cpp
	cd incl ; dos2unix -k *.h 
	cd incl/TAXI ; dos2unix -k *.h 
	cd incl/QT ; dos2unix -k *.h 

rebuild:
	cd tgraph ; touch *.cpp
	cd qt ; touch *.cpp
	cd ClientServer ; touch *.cpp

depend:
	cd tgraph ;make -e depend
	cd qt ;make -e depend
	cd ClientServer ;make -e depend

nodepend:
	cd tgraph ;make -e nodepend
	cd qt ;make -e nodepend
	cd ClientServer ;make -e nodepend	

list:
	cd tgraph ;make list

# DO NOT DELETE
