# tcpcom32

## What is it?
Serial Port to TCP/IP Connection Mapper. Maps sockets to serial ports.

Originally created in 2008 by Roberto Cerfogli (http://sourceforge.net/projects/tcpcom32/), now fixed to work under Windows 7.

## Version history

### [Version 1.2]
- fix for COM port file naming (ports COM10+ should be \\\\.\COM10 instead of COM10)
- settings save fix - do not try to store settings in HKLM registry branch

### [Version 1.1]
- insert orangee logo into about box.
- add statistics dialog with:
	tcp/ip in bytes 
	tcp/ip out bytes
	tcp/ip up time
	rs232 in bytes
	rs232 out bytes

### [Version 1.0.1]
- fixed control of disabled port on connect
- added new icon "disconnected port"
- tray icon update with Shell_NotifyIcon(NIM_MODIFY, &m_nid) changing both tooltip text (e.g. "TcpCOM32\r\nAvailable: ...\r\nConnected: ...") and icon


### [Version 1.0]
- featured stuff...
