TEMPLATE = subdirs


win32:SUBDIRS= tgraph \
         freeglut \
         qt \
         UsingTgraph \
         ClientServer

macx:SUBDIRS= tgraph \       
         UsingTgraph \
         ClientServer \
         cgi  \
		 qt 

CONFIG += ordered

