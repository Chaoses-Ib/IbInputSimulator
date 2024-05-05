#Include %A_ScriptDir%
#Include ..\IbInputSimulator.ahk
InputBox, pid, , Please specify the process ID of the target process
IbSendInit("MouClassInputInjection", 1, pid)

;MouseClickDrag, Left, 5, 5, 150, 50, 0, R
SendInput {Click 100 100}