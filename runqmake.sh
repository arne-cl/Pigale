echo "Running qmake..."

if test -f qt4dir ; then
  QT4DIR=`cat qt4dir`
  export QT4DIR
fi
if test -f system ; then
  SYSTEM=`cat system`
fi
if [ $SYSTEM  = "ubuntu" ]; then
    QMAKE=/usr/bin/qmake-qt4
else
    QMAKE=$QT4DIR/bin/qmake
fi

${QMAKE} -o tgraph/Makefile  tgraph/tgraph.pro
${QMAKE} -o freeglut/Makefile  freeglut/freeglut.pro
${QMAKE} -o qt/Makefile  qt/pigale.pro
${QMAKE} -o ClientServer/Makefile  ClientServer/client.pro
${QMAKE} -o cgi/Makefile  cgi/pigale.pro
