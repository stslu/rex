This file contains information that will help you start using QtitanDataGrid component
in your project. Please refer to this guide prior to using it.

This folder contains the installed version of the QtitanDataGrid component. 

- Version

This folder contains QtitanDataGrid (version 5.1.1).
The version was built with Qt5.6.0 - 32bit(for Windows), Qt5.6.0 64bit(for MacOSX, Linux)


- Required Qt modules

The following Qt modules must be available for the correct operation 
of the QtitanDataGrid component:

QtCore, QtGui, QtSql (for the version of the grid with direct access to 
SQL models), QtXml.

Please note that you can create a grid shared library (qtngrid3.dll or qtndbgrid3.dll) that will be independent 
from QtXml. For more information on this, refer to the "Save and load column's layout to XML" section of the manual.

- Using in Qt projects

- For Windows

1. Open the Windows command line (cmd.exe) and switch to the folder where Qtitan files were installed.
2. Run the "qtitanvars.bat" bat file.
   The bat file will add the QTITANDIR environment variable with the <Install Path> value and will add
   <Install Path>/lib to the PATH environment variable.
3. Add the following code to the *.pro(Qt Project) file of your application:
   include($$QTITANDIR/src/shared/qtitangrid.pri)
4. Reconfigure your application using qmake -r or qmake -tp vc -r (for Visual Studio projects) commands 
   in the folder where your project file is located.

For Linux

1. Open the unix shell and switch to the folder where Qtitan files were installed.
2. Launch the "qtitanvars.sh" script from the command line using the following command: ". ./qtitanvars.sh"
   The script will add the QTITANDIR environment variable with the <Install Path> value and will set the value
   of the LD_LIBRARY_PATH environment variable to <Install Path>/lib for the current unix shell.
3. Add the following code to the *.pro(Qt Project) file of your application:
   include($$QTITANDIR/src/shared/qtitangrid.pri)
4. Reconfigure your application using the qmake –r command in the same folder 
   your project file is located in.

For MacOSX

1. Open the MacOSX terminal and switch to the folder where Qtitan files were installed.
2. Launch the "qtitanvarsmac.sh" script from the command line using the following command: ". ./qtitanvarsmac.sh"
   The script will add the QTITANDIR environment variable with the <Install Path> value and will set the value
   of the LD_LIBRARY_PATH environment variable to <Install Path>/lib for the current MacOSX terminal.
3. Add the following code to the *.pro(Qt Project) file of your application:
   include($$QTITANDIR/src/shared/qtitangrid.pri)
4. Reconfigure your application using the qmake –r command in the same folder 
   your project file is located in.

Note: For trial version of the component you can use release build of the library only.

- Installation of the designer plug-in

To add grid support to QtDesigner, copy the qtngriddsgn2.dll (libqtngriddsgn.so) file to 
$(QTDIR)/plugins/designer/ and <QtCreator Path>/designer/

- Conclusion

We at Developer Machines thank you for using our Qt components.
We do our best to make the Qtitan line of components more comfortable 
to use, reliable and fast. Please do not hesitate to send us your reviews, 
suggestions and feature requests to this address - support@devmachines.com.
Thank you very much and good luck!

Copyright (C) 2009-2016 Developer Machines (http://www.devmachines.com)

