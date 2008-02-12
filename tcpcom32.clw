; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSettingsDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "tcpcom32.h"
LastPage=0

ClassCount=5
Class1=CSettingsDlg
Class2=CTcpcom32App
Class3=CTcpcom32Dlg

ResourceCount=5
Resource1=IDD_DIALOG_TCPCOM32
Resource2=IDD_DIALOG_ABOUT
Resource3=IDD_DIALOG_SETTINGS
Class4=CAboutDlg
Resource4=IDD_DIALOG_TCPIP
Class5=CTcpIpDlg
Resource5=IDR_MENU_TCPCOM32

[CLS:CSettingsDlg]
Type=0
BaseClass=CDialog
HeaderFile=SettingsDlg.h
ImplementationFile=SettingsDlg.cpp
LastObject=IDC_RADIO_BASIC
Filter=D
VirtualFilter=dWC

[CLS:CTcpcom32App]
Type=0
BaseClass=CWinApp
HeaderFile=tcpcom32.h
ImplementationFile=tcpcom32.cpp
LastObject=CTcpcom32App

[CLS:CTcpcom32Dlg]
Type=0
BaseClass=CDialog
HeaderFile=tcpcom32Dlg.h
ImplementationFile=tcpcom32Dlg.cpp
LastObject=CTcpcom32Dlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_DIALOG_SETTINGS]
Type=1
Class=CSettingsDlg
ControlCount=39
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_LOCAL_PORT,edit,1350770816
Control4=IDC_STATIC_LOCAL_PORT,static,1342308352
Control5=IDC_EDIT_LOCAL_IP,edit,1350762624
Control6=IDC_STATIC_LOCAL_IP,static,1342308352
Control7=IDC_STATIC_LOCAL_GROUP,button,1342308359
Control8=IDC_STATIC,button,1342308359
Control9=IDC_EDIT_LOG_DIR,edit,1350631552
Control10=IDC_STATIC_LOGDIR,static,1342308352
Control11=IDC_CHECK_LOG,button,1342242819
Control12=IDC_STATIC_PROXY_GROUP,button,1342308359
Control13=IDC_EDIT_PROXY_IP,edit,1350762624
Control14=IDC_EDIT_PROXY_PORT,edit,1350762624
Control15=IDC_STATIC_PROXY_PORT,static,1342308352
Control16=IDC_STATIC_PROXY_IP,static,1342308352
Control17=IDC_CHECK_PROXY,button,1342373891
Control18=IDC_EDIT_USERNAME,edit,1350762624
Control19=IDC_EDIT_PASSWORD,edit,1350762656
Control20=IDC_STATIC_USERNAME,static,1342308352
Control21=IDC_STATIC_PASSWORD,static,1342308352
Control22=IDC_STATIC_CLIENT_GROUP,button,1342177287
Control23=IDC_CHECK_CLIENT_MODE,button,1342242819
Control24=IDC_EDIT_REMOTE_PORT,edit,1350770816
Control25=IDC_STATIC_REMOTE_PORT,static,1342308352
Control26=IDC_EDIT_REMOTE_IP,edit,1350762624
Control27=IDC_STATIC_REMOTE_IP,static,1342308352
Control28=IDC_STATIC_REMOTE_GROUP,button,1342308359
Control29=IDC_STATIC_SERVER_GROUP,button,1342177287
Control30=IDC_CHECK_SERVER_MODE,button,1342242819
Control31=IDC_EDIT_WORKGROUP,edit,1350631552
Control32=IDC_EDIT_WORKSTATION,edit,1350631552
Control33=IDC_STATIC_WORKGROUP,static,1342308352
Control34=IDC_STATIC_WORKSTATION,static,1342308352
Control35=IDC_STATIC_AUTH_GROUP,button,1342308359
Control36=IDC_RADIO_BASIC,button,1342308361
Control37=IDC_RADIO_NTLM,button,1342308361
Control38=IDC_RADIO_NTLMV2,button,1342308361
Control39=IDC_CHECK_AUTH,button,1342242819

[DLG:IDD_TCPCOM32_DIALOG]
Type=1
Class=CTcpcom32Dlg

[DLG:IDD_DIALOG_TCPCOM32]
Type=1
Class=CTcpcom32Dlg
ControlCount=2
Control1=IDC_TREEVIEW,SysTreeView32,1350631443
Control2=IDC_EDIT_STATUSLINE,edit,1350633600

[MNU:IDR_MENU_TCPCOM32]
Type=1
Class=CTcpcom32Dlg
Command1=ID_SYSTEM_RESTART
Command2=ID_SYSTEM_SETTINGS
Command3=ID_SYSTEM_EXIT
Command4=ID_PORT_CLOSE
Command5=ID_PORT_ENABLE
Command6=ID_PORT_DISABLE
Command7=ID_PORT_SETTINGS
Command8=ID_PORT_CONNECTION
Command9=ID_HELP_ABOUT
CommandCount=9

[DLG:IDD_DIALOG_ABOUT]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342181390
Control3=IDC_EDIT_LGPL,edit,1344280580
Control4=IDC_STATIC,static,1342181390

[CLS:CAboutDlg]
Type=0
HeaderFile=AboutDlg.h
ImplementationFile=AboutDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CAboutDlg
VirtualFilter=dWC

[DLG:IDD_DIALOG_TCPIP]
Type=1
Class=CTcpIpDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_EDIT_REQUEST,edit,1352669316
Control3=IDC_EDIT_RESPONSE,edit,1352669316
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352

[CLS:CTcpIpDlg]
Type=0
HeaderFile=TcpIpDlg.h
ImplementationFile=TcpIpDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_EDIT_REQUEST

