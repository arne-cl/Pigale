The Windows executable uses QT-5.14.1 and the OSX executable uses QT-5.14.0.

They have been tested under Mojave and Windows 10. 
They include all the needed libraries.

Changes in Pigale-1.3.24:
- Improved the parameter layout page.
- Improved client

Changes in Pigale-1.3.23:
- We updated the freeglut sources used for Window to version 3.2.1
  (previously we used a 20 years old version)
- Quite a few small changes and corrections of bugs introduced in Pigale-1.3.22.

Changes in Pigale-1.3.22:
- Few corrections to avoid warnings due to some functions deprecated in Qt-5.14.0.
- The images directory was not saved.
- Under Windows 10, the Pigale layout was very bad.

Changes for Mac in Pigale-1.3.21:
- Corrected a bug when using Xcode 11 (conflict with stack class name)

Changes in Pigale-1.3.20:
- One can load an image (of a graph) in the background of the editor. 
  Its size is adjusted to fit the editor window. 
  Its opacity can be changed with the left slider.
  (I used the included images to construct the Archimedean graphs provided in a tgf file.)
  
- ALT+move (CONTROL on Windows) always allow to move vertices.
- I reduced the speed of the Zoom which was difficult to adjust.
- I improved the graphic presentation independence  of the screen resolution and system used.

 
In case you have problems compiling and/or using Pigale, also if you find bugs:
 please contact me by email (link at the bottom of "Pigale Web Site").