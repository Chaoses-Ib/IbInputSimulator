# IbAhkSendLib
Languages: [English](README.md), [简体中文](README.zh-Hans.md)  
An [AutoHotkey](https://www.autohotkey.com/) library that enables AHK to send keystrokes by drivers.

## Supported Drivers
* [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html)
* [Logitech Gaming Software](https://support.logi.com/hc/en-gb/articles/360025298053-Logitech-Gaming-Software)

## Example
### AHK v2
```ahk
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include "IbAhkSend.ahk"

IbSendInit(1)

Send("#r")
WinWaitActive("ahk_class #32770")
Send("notepad`n")

WinWaitActive("ahk_exe notepad.exe")
Send("Hello world+1")
Sleep 100
MouseClickDrag("Left", 5, 5, 150, 50)
```

### AHK v1
```ahk
; Run Notepad, type "Hello world!" and then select all text by mouse.

#Include IbAhkSend.ahk

IbSendInit(1)

Send #r
WinWaitActive, ahk_class #32770
Send notepad`n

WinWaitActive, ahk_exe notepad.exe
Send Hello world+1
Sleep 100
CoordMode, Mouse, Client
MouseClickDrag, Left, 5, 5, 150, 50
```

## See Also
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)