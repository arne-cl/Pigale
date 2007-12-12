echo "Running qmake..."

if test -f qt4dir ; then
  QT4DIR=`cat qt4dir`
  export QT4DIR
fi

QMAKE=$QT4DIR/bin/qmake
${QMAKE} -o tgraph/Makefile  tgraph/tgraph.pro
${QMAKE} -o freeglut/Makefile  freeglut/freeglut.pro
${QMAKE} -o qt/Makefile  qt/pigale.pro
${QMAKE} -o ClientServer/Makefile  ClientServer/client.pro
${QMAKE} -o cgi/Makefile  cgi/pigale.pro