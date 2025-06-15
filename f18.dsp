# Microsoft Developer Studio Project File - Name="F18" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=F18 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "f18.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/f18", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "F18 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I ".\OSI" /I ".\OSI3dfx" /I "Z:\JF18\F18_SRC" /I "Z:\JF18\F18_SRC\osi" /I "Z:\JF18\F18_SRC\osi3dfx" /I "Z:\JF18\F18_SRC\wrappers" /I "Z:\JF18\F18_SRC\skunkcmp" /I "Z:\JF18\F18_SRC\skunkctl" /I "Z:\JF18\F18_SRC\skunknet" /I "Z:\JF18\F18_SRC\skunksnd" /I "Z:\JF18\F18_SRC\skunkutl" /I "Z:\JF18\F18_SRC\skunkvid" /I "Z:\JF18\F18_SRC\UV" /I "Z:\JF18\F18_SRC\skunkff" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "GAME_VERSION" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib ddraw.lib dxguid.lib dsound.lib OSI.lib OSI3dfx.lib SkunkCmp.lib SkunkNet.lib SkunkSnd.lib SkunkVid.lib UV.lib MsApi.lib SkunkFF.lib SkunkCtl.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"msvcrtd.lib" /out:".\bin\Release\F18.exe" /libpath:"Z:\JF18\F18_SRC" /libpath:"Z:\JF18\F18_SRC\osi" /libpath:"Z:\JF18\F18_SRC\osi3dfx" /libpath:"Z:\JF18\F18_SRC\wrappers" /libpath:"Z:\JF18\F18_SRC\skunkcmp" /libpath:"Z:\JF18\F18_SRC\skunkctl" /libpath:"Z:\JF18\F18_SRC\skunknet" /libpath:"Z:\JF18\F18_SRC\skunksnd" /libpath:"Z:\JF18\F18_SRC\skunkvid" /libpath:"Z:\JF18\F18_SRC\skunkutl" /libpath:"Z:\JF18\F18_SRC\UV"
# SUBTRACT LINK32 /pdb:none /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ".\bin\Release\F18.exe" "..\Janes F-18 install\F18_release.exe"
# End Special Build Tool

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I ".\OSI" /I ".\OSI3dfx" /I "Z:\JF18\F18_SRC" /I "Z:\JF18\F18_SRC\osi" /I "Z:\JF18\F18_SRC\osi3dfx" /I "Z:\JF18\F18_SRC\wrappers" /I "Z:\JF18\F18_SRC\skunkcmp" /I "Z:\JF18\F18_SRC\skunkctl" /I "Z:\JF18\F18_SRC\skunknet" /I "Z:\JF18\F18_SRC\skunksnd" /I "Z:\JF18\F18_SRC\skunkutl" /I "Z:\JF18\F18_SRC\skunkvid" /I "Z:\JF18\F18_SRC\UV" /I "Z:\JF18\F18_SRC\skunkff" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "GAME_VERSION" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib ddraw.lib dxguid.lib dsound.lib OSI.lib OSI3dfx.lib SkunkCmp.lib SkunkNet.lib SkunkSnd.lib SkunkVid.lib UV.lib MsApi.lib SkunkCtl.lib SkunkFF.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrt.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"msvcrtd.lib" /out:".\bin\Debug\F18.exe" /libpath:"Z:\JF18\F18_SRC" /libpath:"Z:\JF18\F18_SRC\osi" /libpath:"Z:\JF18\F18_SRC\osi3dfx" /libpath:"Z:\JF18\F18_SRC\wrappers" /libpath:"Z:\JF18\F18_SRC\skunkcmp" /libpath:"Z:\JF18\F18_SRC\skunkctl" /libpath:"Z:\JF18\F18_SRC\skunknet" /libpath:"Z:\JF18\F18_SRC\skunksnd" /libpath:"Z:\JF18\F18_SRC\skunkvid" /libpath:"Z:\JF18\F18_SRC\skunkutl" /libpath:"Z:\JF18\F18_SRC\UV"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "F18 - Win32 Release"
# Name "F18 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\3dxl.cpp
# End Source File
# Begin Source File

SOURCE=.\aaa.cpp
# End Source File
# Begin Source File

SOURCE=.\aero.cpp
# End Source File
# Begin Source File

SOURCE=.\agradar.cpp
# End Source File
# Begin Source File

SOURCE=.\aiflight.cpp
# End Source File
# Begin Source File

SOURCE=.\airdam.cpp
# End Source File
# Begin Source File

SOURCE=.\autoptl.cpp
# End Source File
# Begin Source File

SOURCE=.\avmodes.cpp
# End Source File
# Begin Source File

SOURCE=.\avsensor.cpp
# End Source File
# Begin Source File

SOURCE=.\behave.cpp

!IF  "$(CFG)" == "F18 - Win32 Release"

# ADD CPP /Od

!ELSEIF  "$(CFG)" == "F18 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\carrtank.cpp
# End Source File
# Begin Source File

SOURCE=.\clouds.cpp
# End Source File
# Begin Source File

SOURCE=.\Cockpit.cpp
# End Source File
# Begin Source File

SOURCE=.\cockpitv.cpp
# End Source File
# Begin Source File

SOURCE=.\cpitinterface.cpp
# End Source File
# Begin Source File

SOURCE=.\cpitsystems.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\DemoRegistry.cpp
# End Source File
# Begin Source File

SOURCE=.\dogfite.cpp
# End Source File
# Begin Source File

SOURCE=.\engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Evaluate.cpp
# End Source File
# Begin Source File

SOURCE=.\evlog.cpp
# End Source File
# Begin Source File

SOURCE=.\F18.def
# End Source File
# Begin Source File

SOURCE=.\F18res.rc
# End Source File
# Begin Source File

SOURCE=.\Flames.cpp
# End Source File
# Begin Source File

SOURCE=.\flight.cpp
# End Source File
# Begin Source File

SOURCE=.\flightfx.cpp
# End Source File
# Begin Source File

SOURCE=.\flir.cpp
# End Source File
# Begin Source File

SOURCE=.\globvars.CPP
# End Source File
# Begin Source File

SOURCE=.\gnddam.cpp
# End Source File
# Begin Source File

SOURCE=.\grnddef.cpp
# End Source File
# Begin Source File

SOURCE=.\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\hudprims.cpp
# End Source File
# Begin Source File

SOURCE=.\initdb.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\Joystuff.cpp
# End Source File
# Begin Source File

SOURCE=.\keyfunc.cpp
# End Source File
# Begin Source File

SOURCE=.\keysmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\keystuff.cpp
# End Source File
# Begin Source File

SOURCE=.\langtext.cpp
# End Source File
# Begin Source File

SOURCE=.\langtxt2.cpp
# End Source File
# Begin Source File

SOURCE=.\loadmiss.cpp
# End Source File
# Begin Source File

SOURCE=.\los.cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MemMap.cpp
# End Source File
# Begin Source File

SOURCE=.\missionhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\mpd.cpp
# End Source File
# Begin Source File

SOURCE=.\multiai.cpp
# End Source File
# Begin Source File

SOURCE=.\netchat.cpp
# End Source File
# Begin Source File

SOURCE=.\NetWork.cpp
# End Source File
# Begin Source File

SOURCE=.\padlock.cpp
# End Source File
# Begin Source File

SOURCE=.\particle.cpp
# End Source File
# Begin Source File

SOURCE=.\radar.cpp
# End Source File
# Begin Source File

SOURCE=.\render.cpp
# End Source File
# Begin Source File

SOURCE=.\rendini.cpp
# End Source File
# Begin Source File

SOURCE=.\Resources.cpp
# End Source File
# Begin Source File

SOURCE=.\sflight.cpp
# End Source File
# Begin Source File

SOURCE=.\simmain.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\ssound.cpp
# End Source File
# Begin Source File

SOURCE=.\ufc.cpp
# End Source File
# Begin Source File

SOURCE=.\vehicles.cpp
# End Source File
# Begin Source File

SOURCE=.\Views.cpp
# End Source File
# Begin Source File

SOURCE=.\weapmanage.cpp
# End Source File
# Begin Source File

SOURCE=.\weapons.cpp
# End Source File
# Begin Source File

SOURCE=.\WrapGrab.cpp
# End Source File
# Begin Source File

SOURCE=.\OSI.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\OSI3dfx\3dfxF18.h
# End Source File
# Begin Source File

SOURCE=.\agradar.h
# End Source File
# Begin Source File

SOURCE=.\avlocal.h
# End Source File
# Begin Source File

SOURCE=.\bbmenu.h
# End Source File
# Begin Source File

SOURCE=.\behavep.h
# End Source File
# Begin Source File

SOURCE=.\behavior.h
# End Source File
# Begin Source File

SOURCE=.\bit.h
# End Source File
# Begin Source File

SOURCE=.\buildmsg.h
# End Source File
# Begin Source File

SOURCE=.\clouds.h
# End Source File
# Begin Source File

SOURCE=.\dataform.h
# End Source File
# Begin Source File

SOURCE=.\DebugHelper.h
# End Source File
# Begin Source File

SOURCE=.\dplay.h
# End Source File
# Begin Source File

SOURCE=.\extvars.H
# End Source File
# Begin Source File

SOURCE=.\F18.h
# End Source File
# Begin Source File

SOURCE=.\F18Main.h
# End Source File
# Begin Source File

SOURCE=.\f18weap.h
# End Source File
# Begin Source File

SOURCE=.\OSI\family.h
# End Source File
# Begin Source File

SOURCE=.\flight.h
# End Source File
# Begin Source File

SOURCE=.\OSI\fmath.h
# End Source File
# Begin Source File

SOURCE=.\gamesettings.h
# End Source File
# Begin Source File

SOURCE=.\OSI\grvars.h
# End Source File
# Begin Source File

SOURCE=.\input.h
# End Source File
# Begin Source File

SOURCE=.\joystuff.h
# End Source File
# Begin Source File

SOURCE=.\keysmsg.h
# End Source File
# Begin Source File

SOURCE=.\keystuff.h
# End Source File
# Begin Source File

SOURCE=.\LoopProcess.h
# End Source File
# Begin Source File

SOURCE=.\mission.h
# End Source File
# Begin Source File

SOURCE=.\movplyr.h
# End Source File
# Begin Source File

SOURCE=.\MSAPI.h
# End Source File
# Begin Source File

SOURCE=.\MultiDefs.h
# End Source File
# Begin Source File

SOURCE=.\particle.h
# End Source File
# Begin Source File

SOURCE=.\OSI\polydraw.h
# End Source File
# Begin Source File

SOURCE=.\Rad.h
# End Source File
# Begin Source File

SOURCE=.\radar.h
# End Source File
# Begin Source File

SOURCE=.\OSI\regit.h
# End Source File
# Begin Source File

SOURCE=.\render.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resources.h
# End Source File
# Begin Source File

SOURCE=.\SkunkNet.h
# End Source File
# Begin Source File

SOURCE=.\Smack.h
# End Source File
# Begin Source File

SOURCE=.\SmkVideo.h
# End Source File
# Begin Source File

SOURCE=.\spchcat.h
# End Source File
# Begin Source File

SOURCE=.\OSI\terrain.h
# End Source File
# Begin Source File

SOURCE=.\uvstruct.h
# End Source File
# Begin Source File

SOURCE=.\views.h
# End Source File
# Begin Source File

SOURCE=.\WrapInterface.h
# End Source File
# Begin Source File

SOURCE=.\WrapScreens.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\JanesF18.ico
# End Source File
# Begin Source File

SOURCE=.\simmsge.bin
# End Source File
# Begin Source File

SOURCE=.\simmsgf.bin
# End Source File
# Begin Source File

SOURCE=.\simmsgg.bin
# End Source File
# Begin Source File

SOURCE=.\simmsgs.bin
# End Source File
# End Group
# Begin Source File

SOURCE=.\F18msgs.rc
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Target
# End Project
