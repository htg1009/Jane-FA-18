# Microsoft Developer Studio Generated NMAKE File, Based on f18.dsp
!IF "$(CFG)" == ""
CFG=F18 - Win32 Release
!MESSAGE No configuration specified. Defaulting to F18 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "F18 - Win32 Release" && "$(CFG)" != "F18 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "f18.mak" CFG="F18 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "F18 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "F18 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "F18 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : ".\bin\F18.exe" ".\F18.log"

!ELSE 

ALL : "xvars - Win32 Release" "OSI - Win32 Release" "OSI3Dfx - Win32 Release" ".\bin\F18.exe" ".\F18.log"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"OSI3Dfx - Win32 ReleaseCLEAN" "OSI - Win32 ReleaseCLEAN" "xvars - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\3dxl.obj"
	-@erase "$(INTDIR)\aaa.obj"
	-@erase "$(INTDIR)\agradar.obj"
	-@erase "$(INTDIR)\aiflight.obj"
	-@erase "$(INTDIR)\airdam.obj"
	-@erase "$(INTDIR)\avmodes.obj"
	-@erase "$(INTDIR)\avsensor.obj"
	-@erase "$(INTDIR)\behave.obj"
	-@erase "$(INTDIR)\Cockpit.obj"
	-@erase "$(INTDIR)\cockpitv.obj"
	-@erase "$(INTDIR)\cpitinterface.obj"
	-@erase "$(INTDIR)\cpitsystems.obj"
	-@erase "$(INTDIR)\DemoRegistry.obj"
	-@erase "$(INTDIR)\dogfite.obj"
	-@erase "$(INTDIR)\evlog.obj"
	-@erase "$(INTDIR)\F18res.res"
	-@erase "$(INTDIR)\Flames.obj"
	-@erase "$(INTDIR)\flight.obj"
	-@erase "$(INTDIR)\flir.obj"
	-@erase "$(INTDIR)\globvars.obj"
	-@erase "$(INTDIR)\gnddam.obj"
	-@erase "$(INTDIR)\grnddef.obj"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\initdb.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\Joystuff.obj"
	-@erase "$(INTDIR)\keyfunc.obj"
	-@erase "$(INTDIR)\keysmsg.obj"
	-@erase "$(INTDIR)\keystuff.obj"
	-@erase "$(INTDIR)\langtext.obj"
	-@erase "$(INTDIR)\loadmiss.obj"
	-@erase "$(INTDIR)\los.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\missionhelp.obj"
	-@erase "$(INTDIR)\mpd.obj"
	-@erase "$(INTDIR)\multiai.obj"
	-@erase "$(INTDIR)\netchat.obj"
	-@erase "$(INTDIR)\NetWork.obj"
	-@erase "$(INTDIR)\padlock.obj"
	-@erase "$(INTDIR)\particle.obj"
	-@erase "$(INTDIR)\radar.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\rendini.obj"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\sflight.obj"
	-@erase "$(INTDIR)\simmain.obj"
	-@erase "$(INTDIR)\ssound.obj"
	-@erase "$(INTDIR)\ufc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vehicles.obj"
	-@erase "$(INTDIR)\Views.obj"
	-@erase "$(INTDIR)\weapmanage.obj"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\WrapGrab.obj"
	-@erase "$(OUTDIR)\F18.map"
	-@erase ".\bin\F18.exe"
	-@erase "F18.log"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /Ob2 /I ".\OSI" /I ".\OSI3dfx" /I "c:\f18" /I "c:\f18\osi" /I "c:\f18\osi3dfx" /I "c:\f18\wrappers" /I "c:\f18\skunkcmp" /I "c:\f18\skunkctl" /I "c:\f18\skunknet" /I "c:\f18\skunksnd" /I "c:\f18\skunkutl" /I "c:\f18\skunkvid" /I "c:\f18\UV" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GAME_VERSION" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\F18res.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\f18.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib ddraw.lib dxguid.lib dsound.lib OSI.lib OSI3dfx.lib SkunkCmp.lib SkunkNet.lib SkunkSnd.lib SkunkVid.lib UV.lib MsApi.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\F18.pdb" /map:"$(INTDIR)\F18.map" /machine:I386 /nodefaultlib:"LIBCD" /def:".\F18.def" /out:".\bin\F18.exe" /libpath:"c:\f18" /libpath:"c:\f18\osi" /libpath:"c:\f18\osi3dfx" /libpath:"c:\f18\wrappers" /libpath:"c:\f18\skunkcmp" /libpath:"c:\f18\skunkctl" /libpath:"c:\f18\skunknet" /libpath:"c:\f18\skunksnd" /libpath:"c:\f18\skunkvid" /libpath:"c:\f18\skunkutl" /libpath:"c:\f18\UV" 
DEF_FILE= \
	".\F18.def"
LINK32_OBJS= \
	"$(INTDIR)\3dxl.obj" \
	"$(INTDIR)\aaa.obj" \
	"$(INTDIR)\agradar.obj" \
	"$(INTDIR)\aiflight.obj" \
	"$(INTDIR)\airdam.obj" \
	"$(INTDIR)\avmodes.obj" \
	"$(INTDIR)\avsensor.obj" \
	"$(INTDIR)\behave.obj" \
	"$(INTDIR)\Cockpit.obj" \
	"$(INTDIR)\cockpitv.obj" \
	"$(INTDIR)\cpitinterface.obj" \
	"$(INTDIR)\cpitsystems.obj" \
	"$(INTDIR)\DemoRegistry.obj" \
	"$(INTDIR)\dogfite.obj" \
	"$(INTDIR)\evlog.obj" \
	"$(INTDIR)\Flames.obj" \
	"$(INTDIR)\flight.obj" \
	"$(INTDIR)\flir.obj" \
	"$(INTDIR)\globvars.obj" \
	"$(INTDIR)\gnddam.obj" \
	"$(INTDIR)\grnddef.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\initdb.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\Joystuff.obj" \
	"$(INTDIR)\keyfunc.obj" \
	"$(INTDIR)\keysmsg.obj" \
	"$(INTDIR)\keystuff.obj" \
	"$(INTDIR)\langtext.obj" \
	"$(INTDIR)\loadmiss.obj" \
	"$(INTDIR)\los.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\missionhelp.obj" \
	"$(INTDIR)\mpd.obj" \
	"$(INTDIR)\multiai.obj" \
	"$(INTDIR)\netchat.obj" \
	"$(INTDIR)\NetWork.obj" \
	"$(INTDIR)\padlock.obj" \
	"$(INTDIR)\particle.obj" \
	"$(INTDIR)\radar.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\rendini.obj" \
	"$(INTDIR)\Resources.obj" \
	"$(INTDIR)\sflight.obj" \
	"$(INTDIR)\simmain.obj" \
	"$(INTDIR)\ssound.obj" \
	"$(INTDIR)\ufc.obj" \
	"$(INTDIR)\vehicles.obj" \
	"$(INTDIR)\Views.obj" \
	"$(INTDIR)\weapmanage.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\WrapGrab.obj" \
	"$(INTDIR)\F18res.res" \
	".\OSI.lib" \
	".\OSI3dfx.lib"

".\bin\F18.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

InputDir=.\bin
OutDir=.\Release
WkspDir=.
InputPath=.\bin\F18.exe
SOURCE="$(InputPath)"

".\F18.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	dir $(InputPath) >F18.log 
	move $(InputDir)\F18.lib $(WkspDir) 
	move $(InputDir)\F18.exp $(WkspDir) 
	echo xcopying  g:\F18Img\*.dll  to  $(WkspDir)\bin 
	xcopy /d g:\F18Img\*.dll $(WkspDir)\bin 
	xcopy /d g:\F18Img\*.bsc $(OutDir) 
<< 
	

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\bin\F18.exe" ".\F18.log"

!ELSE 

ALL : "xvars - Win32 Debug" "OSI - Win32 Debug" "OSI3Dfx - Win32 Debug" ".\bin\F18.exe" ".\F18.log"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"OSI3Dfx - Win32 DebugCLEAN" "OSI - Win32 DebugCLEAN" "xvars - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\3dxl.obj"
	-@erase "$(INTDIR)\aaa.obj"
	-@erase "$(INTDIR)\agradar.obj"
	-@erase "$(INTDIR)\aiflight.obj"
	-@erase "$(INTDIR)\airdam.obj"
	-@erase "$(INTDIR)\avmodes.obj"
	-@erase "$(INTDIR)\avsensor.obj"
	-@erase "$(INTDIR)\behave.obj"
	-@erase "$(INTDIR)\Cockpit.obj"
	-@erase "$(INTDIR)\cockpitv.obj"
	-@erase "$(INTDIR)\cpitinterface.obj"
	-@erase "$(INTDIR)\cpitsystems.obj"
	-@erase "$(INTDIR)\DemoRegistry.obj"
	-@erase "$(INTDIR)\dogfite.obj"
	-@erase "$(INTDIR)\evlog.obj"
	-@erase "$(INTDIR)\F18res.res"
	-@erase "$(INTDIR)\Flames.obj"
	-@erase "$(INTDIR)\flight.obj"
	-@erase "$(INTDIR)\flir.obj"
	-@erase "$(INTDIR)\globvars.obj"
	-@erase "$(INTDIR)\gnddam.obj"
	-@erase "$(INTDIR)\grnddef.obj"
	-@erase "$(INTDIR)\hud.obj"
	-@erase "$(INTDIR)\initdb.obj"
	-@erase "$(INTDIR)\input.obj"
	-@erase "$(INTDIR)\Joystuff.obj"
	-@erase "$(INTDIR)\keyfunc.obj"
	-@erase "$(INTDIR)\keysmsg.obj"
	-@erase "$(INTDIR)\keystuff.obj"
	-@erase "$(INTDIR)\langtext.obj"
	-@erase "$(INTDIR)\loadmiss.obj"
	-@erase "$(INTDIR)\los.obj"
	-@erase "$(INTDIR)\Main.obj"
	-@erase "$(INTDIR)\missionhelp.obj"
	-@erase "$(INTDIR)\mpd.obj"
	-@erase "$(INTDIR)\multiai.obj"
	-@erase "$(INTDIR)\netchat.obj"
	-@erase "$(INTDIR)\NetWork.obj"
	-@erase "$(INTDIR)\padlock.obj"
	-@erase "$(INTDIR)\particle.obj"
	-@erase "$(INTDIR)\radar.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\rendini.obj"
	-@erase "$(INTDIR)\Resources.obj"
	-@erase "$(INTDIR)\sflight.obj"
	-@erase "$(INTDIR)\simmain.obj"
	-@erase "$(INTDIR)\ssound.obj"
	-@erase "$(INTDIR)\ufc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vehicles.obj"
	-@erase "$(INTDIR)\Views.obj"
	-@erase "$(INTDIR)\weapmanage.obj"
	-@erase "$(INTDIR)\weapons.obj"
	-@erase "$(INTDIR)\WrapGrab.obj"
	-@erase "$(OUTDIR)\F18.pdb"
	-@erase ".\bin\F18.exe"
	-@erase "F18.log"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\OSI" /I ".\OSI3dfx" /I "c:\f18" /I "c:\f18\osi" /I "c:\f18\osi3dfx" /I "c:\f18\wrappers" /I "c:\f18\skunkcmp" /I "c:\f18\skunkctl" /I "c:\f18\skunknet" /I "c:\f18\skunksnd" /I "c:\f18\skunkutl" /I "c:\f18\skunkvid" /I "c:\f18\UV" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GAME_VERSION" /Fp"$(INTDIR)\f18.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\F18res.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\f18.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib ddraw.lib dxguid.lib dsound.lib OSI.lib OSI3dfx.lib SkunkCmp.lib SkunkNet.lib SkunkSnd.lib SkunkVid.lib UV.lib MsApi.lib SkunkCtl.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\F18.pdb" /debug /machine:I386 /def:".\F18.def" /out:".\bin\F18.exe" /libpath:"c:\f18" /libpath:"c:\f18\osi" /libpath:"c:\f18\osi3dfx" /libpath:"c:\f18\wrappers" /libpath:"c:\f18\skunkcmp" /libpath:"c:\f18\skunkctl" /libpath:"c:\f18\skunknet" /libpath:"c:\f18\skunksnd" /libpath:"c:\f18\skunkvid" /libpath:"c:\f18\skunkutl" /libpath:"c:\f18\UV" 
DEF_FILE= \
	".\F18.def"
LINK32_OBJS= \
	"$(INTDIR)\3dxl.obj" \
	"$(INTDIR)\aaa.obj" \
	"$(INTDIR)\agradar.obj" \
	"$(INTDIR)\aiflight.obj" \
	"$(INTDIR)\airdam.obj" \
	"$(INTDIR)\avmodes.obj" \
	"$(INTDIR)\avsensor.obj" \
	"$(INTDIR)\behave.obj" \
	"$(INTDIR)\Cockpit.obj" \
	"$(INTDIR)\cockpitv.obj" \
	"$(INTDIR)\cpitinterface.obj" \
	"$(INTDIR)\cpitsystems.obj" \
	"$(INTDIR)\DemoRegistry.obj" \
	"$(INTDIR)\dogfite.obj" \
	"$(INTDIR)\evlog.obj" \
	"$(INTDIR)\Flames.obj" \
	"$(INTDIR)\flight.obj" \
	"$(INTDIR)\flir.obj" \
	"$(INTDIR)\globvars.obj" \
	"$(INTDIR)\gnddam.obj" \
	"$(INTDIR)\grnddef.obj" \
	"$(INTDIR)\hud.obj" \
	"$(INTDIR)\initdb.obj" \
	"$(INTDIR)\input.obj" \
	"$(INTDIR)\Joystuff.obj" \
	"$(INTDIR)\keyfunc.obj" \
	"$(INTDIR)\keysmsg.obj" \
	"$(INTDIR)\keystuff.obj" \
	"$(INTDIR)\langtext.obj" \
	"$(INTDIR)\loadmiss.obj" \
	"$(INTDIR)\los.obj" \
	"$(INTDIR)\Main.obj" \
	"$(INTDIR)\missionhelp.obj" \
	"$(INTDIR)\mpd.obj" \
	"$(INTDIR)\multiai.obj" \
	"$(INTDIR)\netchat.obj" \
	"$(INTDIR)\NetWork.obj" \
	"$(INTDIR)\padlock.obj" \
	"$(INTDIR)\particle.obj" \
	"$(INTDIR)\radar.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\rendini.obj" \
	"$(INTDIR)\Resources.obj" \
	"$(INTDIR)\sflight.obj" \
	"$(INTDIR)\simmain.obj" \
	"$(INTDIR)\ssound.obj" \
	"$(INTDIR)\ufc.obj" \
	"$(INTDIR)\vehicles.obj" \
	"$(INTDIR)\Views.obj" \
	"$(INTDIR)\weapmanage.obj" \
	"$(INTDIR)\weapons.obj" \
	"$(INTDIR)\WrapGrab.obj" \
	"$(INTDIR)\F18res.res" \
	".\OSI.lib" \
	".\OSI3dfx.lib"

".\bin\F18.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

InputDir=.\bin
OutDir=.\Debug
WkspDir=.
InputPath=.\bin\F18.exe
SOURCE="$(InputPath)"

".\F18.log" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	dir $(InputPath) >F18.log 
	move $(InputDir)\F18.lib $(WkspDir) 
	move $(InputDir)\F18.exp $(WkspDir) 
	echo xcopying  g:\F18Img\*.dll  to  $(WkspDir)\bin 
	xcopy /d g:\F18Img\*.dll $(WkspDir)\bin 
	xcopy /d g:\F18Img\*.bsc $(OutDir) 
<< 
	

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("f18.dep")
!INCLUDE "f18.dep"
!ELSE 
!MESSAGE Warning: cannot find "f18.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "F18 - Win32 Release" || "$(CFG)" == "F18 - Win32 Debug"
SOURCE=.\3dxl.cpp

"$(INTDIR)\3dxl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aaa.cpp

"$(INTDIR)\aaa.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\agradar.cpp

"$(INTDIR)\agradar.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\aiflight.cpp

"$(INTDIR)\aiflight.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\airdam.cpp

"$(INTDIR)\airdam.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avmodes.cpp

"$(INTDIR)\avmodes.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avsensor.cpp

"$(INTDIR)\avsensor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\behave.cpp

"$(INTDIR)\behave.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Cockpit.cpp

"$(INTDIR)\Cockpit.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cockpitv.cpp

"$(INTDIR)\cockpitv.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cpitinterface.cpp

"$(INTDIR)\cpitinterface.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\cpitsystems.cpp

"$(INTDIR)\cpitsystems.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DemoRegistry.cpp

"$(INTDIR)\DemoRegistry.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dogfite.cpp

"$(INTDIR)\dogfite.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\evlog.cpp

"$(INTDIR)\evlog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\F18res.rc

"$(INTDIR)\F18res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Flames.cpp

"$(INTDIR)\Flames.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\flight.cpp

"$(INTDIR)\flight.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\flir.cpp

"$(INTDIR)\flir.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\globvars.CPP

"$(INTDIR)\globvars.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\gnddam.cpp

"$(INTDIR)\gnddam.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\grnddef.cpp

"$(INTDIR)\grnddef.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hud.cpp

"$(INTDIR)\hud.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\initdb.cpp

"$(INTDIR)\initdb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\input.cpp

"$(INTDIR)\input.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Joystuff.cpp

"$(INTDIR)\Joystuff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\keyfunc.cpp

"$(INTDIR)\keyfunc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\keysmsg.cpp

"$(INTDIR)\keysmsg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\keystuff.cpp

"$(INTDIR)\keystuff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\langtext.cpp

"$(INTDIR)\langtext.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\loadmiss.cpp

"$(INTDIR)\loadmiss.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\los.cpp

"$(INTDIR)\los.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Main.cpp

"$(INTDIR)\Main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\missionhelp.cpp

"$(INTDIR)\missionhelp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mpd.cpp

"$(INTDIR)\mpd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\multiai.cpp

"$(INTDIR)\multiai.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\netchat.cpp

"$(INTDIR)\netchat.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\NetWork.cpp

"$(INTDIR)\NetWork.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\padlock.cpp

"$(INTDIR)\padlock.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\particle.cpp

"$(INTDIR)\particle.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\radar.cpp

"$(INTDIR)\radar.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\render.cpp

"$(INTDIR)\render.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rendini.cpp

"$(INTDIR)\rendini.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Resources.cpp

"$(INTDIR)\Resources.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sflight.cpp

"$(INTDIR)\sflight.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\simmain.cpp

"$(INTDIR)\simmain.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ssound.cpp

"$(INTDIR)\ssound.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ufc.cpp

"$(INTDIR)\ufc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vehicles.cpp

"$(INTDIR)\vehicles.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Views.cpp

"$(INTDIR)\Views.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\weapmanage.cpp

"$(INTDIR)\weapmanage.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\weapons.cpp

"$(INTDIR)\weapons.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\WrapGrab.cpp

"$(INTDIR)\WrapGrab.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "F18 - Win32 Release"

"OSI3Dfx - Win32 Release" : 
   cd ".\OSI3Dfx"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI3Dfx.mak CFG="OSI3Dfx - Win32 Release" 
   cd ".."

"OSI3Dfx - Win32 ReleaseCLEAN" : 
   cd ".\OSI3Dfx"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI3Dfx.mak CFG="OSI3Dfx - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

"OSI3Dfx - Win32 Debug" : 
   cd ".\OSI3Dfx"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI3Dfx.mak CFG="OSI3Dfx - Win32 Debug" 
   cd ".."

"OSI3Dfx - Win32 DebugCLEAN" : 
   cd ".\OSI3Dfx"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI3Dfx.mak CFG="OSI3Dfx - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "F18 - Win32 Release"

"OSI - Win32 Release" : 
   cd ".\OSI"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI.MAK CFG="OSI - Win32 Release" 
   cd ".."

"OSI - Win32 ReleaseCLEAN" : 
   cd ".\OSI"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI.MAK CFG="OSI - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

"OSI - Win32 Debug" : 
   cd ".\OSI"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI.MAK CFG="OSI - Win32 Debug" 
   cd ".."

"OSI - Win32 DebugCLEAN" : 
   cd ".\OSI"
   $(MAKE) /$(MAKEFLAGS) /F .\OSI.MAK CFG="OSI - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "F18 - Win32 Release"

"xvars - Win32 Release" : 
   cd ".\xvars"
   nmake /f "xvars.mak"
   cd ".."

"xvars - Win32 ReleaseCLEAN" : 
   cd ".\xvars"
   cd ".."

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

"xvars - Win32 Debug" : 
   cd ".\xvars"
   nmake /s /NOLOGO -f "c:\f18\xvars.mak"
   cd ".."

"xvars - Win32 DebugCLEAN" : 
   cd ".\xvars"
   cd ".."

!ENDIF 

SOURCE=.\F18msgs.rc

!ENDIF 

