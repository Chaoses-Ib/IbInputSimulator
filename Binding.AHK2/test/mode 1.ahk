; Mode 1
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include "..\IbInputSimulator.ahk"

IbSendInit()

Send("#r")
WinWaitActive("ahk_class #32770")
Send("notepad`n")

WinWaitActive("ahk_exe notepad.exe")
Send("Hello world{!}")
Sleep(100)
MouseClickDrag("Left", 5, 5, 150, 50)