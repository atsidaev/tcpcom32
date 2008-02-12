# Microsoft Developer Studio Project File - Name="tcpcom32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=tcpcom32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tcpcom32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tcpcom32.mak" CFG="tcpcom32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tcpcom32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "tcpcom32 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "tcpcom32 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "tcpcom32 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x410 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x410 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "tcpcom32 - Win32 Release"
# Name "tcpcom32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\apr_md4.c
# End Source File
# Begin Source File

SOURCE=.\apr_md5.c
# End Source File
# Begin Source File

SOURCE=.\apr_sha1.c
# End Source File
# Begin Source File

SOURCE=.\AsyncPort.cpp
# End Source File
# Begin Source File

SOURCE=.\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\des_crypt.c
# End Source File
# Begin Source File

SOURCE=.\des_impl.c
# End Source File
# Begin Source File

SOURCE=.\des_soft.c
# End Source File
# Begin Source File

SOURCE=.\HTTP.cpp
# End Source File
# Begin Source File

SOURCE=.\NTLM.cpp
# End Source File
# Begin Source File

SOURCE=.\PortContext.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\tcpcom32.cpp
# End Source File
# Begin Source File

SOURCE=.\tcpcom32.rc
# End Source File
# Begin Source File

SOURCE=.\tcpcom32Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TcpIpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\apr_md4.h
# End Source File
# Begin Source File

SOURCE=.\apr_md5.h
# End Source File
# Begin Source File

SOURCE=.\apr_sha1.h
# End Source File
# Begin Source File

SOURCE=.\AsyncPort.h
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\BaseSocket.h
# End Source File
# Begin Source File

SOURCE=.\ClientSocket.h
# End Source File
# Begin Source File

SOURCE=.\des_crypt.h
# End Source File
# Begin Source File

SOURCE=.\HTTP.h
# End Source File
# Begin Source File

SOURCE=.\NTLM.H
# End Source File
# Begin Source File

SOURCE=.\PortContext.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\rpc_des.h
# End Source File
# Begin Source File

SOURCE=.\ServerSocket.h
# End Source File
# Begin Source File

SOURCE=.\SettingsDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tcpcom32.h
# End Source File
# Begin Source File

SOURCE=.\tcpcom32Dlg.h
# End Source File
# Begin Source File

SOURCE=.\TcpIpDlg.h
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\__icon_conn_error.ico
# End Source File
# Begin Source File

SOURCE=.\res\__icon_conn_open.ico
# End Source File
# Begin Source File

SOURCE=.\res\__icon_delete.ico
# End Source File
# Begin Source File

SOURCE=.\res\__icon_port.ico
# End Source File
# Begin Source File

SOURCE=.\res\_bar_chart.bmp
# End Source File
# Begin Source File

SOURCE=.\res\_computer.bmp
# End Source File
# Begin Source File

SOURCE=.\res\_forbidden.bmp
# End Source File
# Begin Source File

SOURCE=.\res\_home.bmp
# End Source File
# Begin Source File

SOURCE=.\res\_mobile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\_monitor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\arrows.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_error.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_forbidden.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_open.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_open_both.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_open_local.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_open_remote.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_opening1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_opening2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_opening3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\conn_warning.bmp
# End Source File
# Begin Source File

SOURCE=.\res\connection.bmp
# End Source File
# Begin Source File

SOURCE=.\res\delete.bmp
# End Source File
# Begin Source File

SOURCE=.\res\gnu.bmp
# End Source File
# Begin Source File

SOURCE=.\res\handheld.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_arrows.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_handheld.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_mobile.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_por.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon_port.ico
# End Source File
# Begin Source File

SOURCE=.\res\mep.bmp
# End Source File
# Begin Source File

SOURCE=.\res\monitor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\net.bmp
# End Source File
# Begin Source File

SOURCE=.\res\netconn.bmp
# End Source File
# Begin Source File

SOURCE=.\res\network.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pencil.bmp
# End Source File
# Begin Source File

SOURCE=.\res\phone.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port.ico
# End Source File
# Begin Source File

SOURCE=.\res\port_closed.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port_disabled.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port_err.ico
# End Source File
# Begin Source File

SOURCE=.\res\port_error.bmp
# End Source File
# Begin Source File

SOURCE=.\res\port_open.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stats.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tcpcom32.ico
# End Source File
# Begin Source File

SOURCE=.\res\tcpcom32.rc2
# End Source File
# Begin Source File

SOURCE=.\res\world_monitor.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\ReleaseNotes.txt
# End Source File
# End Target
# End Project
