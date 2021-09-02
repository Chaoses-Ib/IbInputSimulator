; Mode 0
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include "..\IbAhkSend.ahk"

IbSendInit("AnyDriver", 0)

IbSend("#r")
WinWaitActive("ahk_class #32770")
IbSend("notepad`n")

WinWaitActive("ahk_exe notepad.exe")
IbSend("Hello world{!}")
Sleep(100)
IbMouseClickDrag("Left", 5, 5, 150, 50)