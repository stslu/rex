REM--------------------------------------------------------------------------
REM--------- 1 Launch the QT Project-----------------------------------------
REM----------2 Run de compilation in Debug  Mode-----------------------------
REM----------3 Run the compilation in release Mode---------------------------
REM----------4 Execute the batch according to the plateForm (64 or 32 bits)--
REM----------5 copy paste the directory in msvc120_x__-----------------------
REM--------------------------------------------------------------------------



remove .\msvc120_x86
mkdir  .\msvc120_x86
mkdir ".\msvc120_x86\bin\msvc120_x86\release\"
mkdir ".\msvc120_x86\bin\msvc120_x86\debug\"

REM --- -- DLL
mkdir ".\msvc120_x86\bin\msvc120_x86\release\"
mkdir "..\msvc120_x86\bin\msvc120_x86\debug\"

xcopy    /Y  ".\bin\qtndbgrid5.dll"  ".\msvc120_x86\bin\msvc120_x86\release\"
xcopy    /Y ".\bin\qtndbgridd5.dll"  ".\msvc120_x86\bin\msvc120_x86\debug\"

REM --- -- LIBS
mkdir ".\msvc120_x86\lib\msvc120_x86\release\"
mkdir ".\msvc120_x86\lib\msvc120_x86\debug\"

xcopy   /Y  ".\bin\qtndbgrid5.lib"  ".\msvc120_x86\lib\msvc120_x86\release\"
xcopy   /Y  ".\bin\qtndbgrid5.exp"  ".\msvc120_x86\lib\msvc120_x86\release\"
xcopy    /Y ".\bin\qtndbgridd5.lib"  ".\msvc120_x86\lib\msvc120_x86\debug\"
xcopy    /Y ".\bin\qtndbgridd5.exp"  ".\msvc120_x86\lib\msvc120_x86\debug\"


REM --- -- INCLUDES
mkdir  ".\msvc120_x86\include\"

 xcopy   /Y ".\include\QtitanDBGrid.h"  				".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\include\QtitanGrid.h"  					".\msvc120_x86\include\qtitangrid\"
 
 xcopy   /Y ".\src/grid\QtnDBGrid.h"  					".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnDBGridBandedTableView.h"  	".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnDBGridTableView.h"  			".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGrid.h"  					".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridBandedTableView.h"  		".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridCardView.h"  			".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridDef.h"  					".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridEditors.h"  				".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridFilter.h"  				".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridFilterWindow.h"  		".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridGraphics.h"  			".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridModelController.h"  		".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridTableView.h"  			".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridTableViewBase.h"  		".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\grid\QtnGridViewGraphics.h"  		".\msvc120_x86\include\qtitangrid\"
 xcopy   /Y ".\src\base\QtitanDef.h"  					".\msvc120_x86\include\qtitangrid\"
 
REM --- -- Prf
REM mkdir  ".\msvc120_x86\features\"
xcopy    /R ".\qtitangrid.prf"  ".\msvc120_x86\features\"  /Y