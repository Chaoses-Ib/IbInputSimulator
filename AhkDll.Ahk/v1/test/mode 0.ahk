; Mode 0 (v1)
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include ..\IbAhkSend.ahk

IbSendInit("AnyDriver", 0)

IbSend("#r")
WinWaitActive, ahk_class #32770
IbSend("notepad`n")

WinWaitActive, ahk_exe notepad.exe
IbSend("Hello world+1")
Sleep 100
CoordMode, Mouse, Client
IbSend("{Click 5, 5, down}{click 150, 50, up}")