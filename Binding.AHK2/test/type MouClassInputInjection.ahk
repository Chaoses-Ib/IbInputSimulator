#Include "..\IbInputSimulator.ahk"
pid := InputBox("Please specify the process ID of the target process")
IbSendInit("MouClassInputInjection", 1, pid)
#Include "mode ahk.ahk"