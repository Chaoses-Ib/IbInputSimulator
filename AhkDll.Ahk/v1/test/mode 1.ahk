; Mode 1 (v1)
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include ..\IbAhkSend.ahk

IbSendInit()

Send #r
WinWaitActive, ahk_class #32770
Send notepad`n

WinWaitActive, ahk_exe notepad.exe
Send Hello world+1
Sleep 100
CoordMode, Mouse, Client
MouseClickDrag, Left, 5, 5, 150, 50