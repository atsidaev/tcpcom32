SET WIXPATH="C:\Documents and Settings\Administrator\Desktop\app\wix\wix-2.0.5325.0-binaries"
candle.exe tcpcom32.wxs
light.exe -out tcpcom32.msi tcpcom32.wixobj %WIXPATH%\wixui.wixlib -loc %WIXPATH%\WixUI_en-us.wxl
