all: libtgraph  libqt

libtgraph : tgraph/*.cpp
	cd tgraph ;make -e

libqt : qt/*.cpp
	cd qt ;make -e

clean:
	cd tgraph ; make -e clean
	cd qt ; make -e clean
	cd incl/TAXI ;rm -f *~ *.hh

rebuild:
	cd tgraph ; touch *.cpp
	cd qt ; touch *.cpp

depend:
	cd tgraph ;make -e depend
	cd qt ;make -e depend

nodepend:
	cd tgraph ;make -e nodepend
	cd qt ;make -e nodepend
 

list:
	cd tgraph ;make list

# DO NOT DELETE
