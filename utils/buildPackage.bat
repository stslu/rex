REM--------------------------------------------------------------------------
REM--------- 1 Launch the QT Project-----------------------------------------
REM----------2 Run the compilation in release Mode---------------------------
REM----------3 Execute the batch  
REM-----------4 Get the directory -------------------------------------------

remove .\RexExec\*.*
remove .\RexExec\bin\*.*
remove .\RexExec\

mkdir  .\RexExec
mkdir  .\RexExec\bin
mkdir  .\RexExec\bin\platforms
mkdir  .\RexExec\bin\sqldrivers
mkdir  .\RexExec\doc

set QTDIR = C:\Qt\Qt5.5.0_x86\5.5\msvc2013

xcopy    /Y  ".\build-RSDesktop-Clone_de_Desktop_Qt_5_5_0_MSVC2013_32bit-Release\bin\RexSensors.exe"  ".\RexExec\bin\"
xcopy    /Y  ".\run.bat"  ".\RexExec\"
xcopy    /Y  ".\RexUserGuide.doc"  ".\RexExec\doc\"

xcopy    /Y "%QTDIR%\bin\Qt5Core.dll"  ".\RexExec\bin\"
xcopy    /Y "%QTDIR%\bin\Qt5Widgets.dll"  ".\RexExec\bin\"
xcopy    /Y "%QTDIR%\bin\Qt5PrintSupport.dll"  ".\RexExec\bin\"

xcopy    /Y "./msvc120_x86\bin\msvc120_x86\release\qtndbgrid5.dll"  ".\RexExec\bin\"

xcopy    /Y ".\drivers\platforms\*.*"  	".\RexExec\bin\platforms\"
xcopy    /Y ".\drivers\sqldrivers\*.*"  	".\RexExec\bin\sqldrivers\"
xcopy    /Y ".\drivers\*.*"  	".\RexExec\bin\"