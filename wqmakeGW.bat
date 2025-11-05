qmake -spec win32-g++ -o freeglut\Makefile  freeglut\freeglut.pro
qmake -spec win32-g++ -o tgraph\Makefile  tgraph\tgraph.pro
qmake -spec win32-g++ -o qt\Makefile  qt\qt.pro
qmake -spec win32-g++ -o ClientServer\Makefile  ClientServer\ClientServer.pro
qmake -spec win32-g++ -o UsingTgraph\Makefile  UsingTgraph\UsingTgraph.pro
qmake -spec win32-g++ -o cgi\Makefile  cgi\cgi.pro

rem qmake  -tp vc -r
