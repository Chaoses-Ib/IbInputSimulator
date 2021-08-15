# IbAhkSendLib
语言：[English](README.md)，[简体中文](README.zh-Hans.md)  
[AutoHotkey](https://www.autohotkey.com/) 驱动按键库。  

## 支持驱动
* [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html)
* [Logitech Gaming Software](https://support.logi.com/hc/en-gb/articles/360025298053-Logitech-Gaming-Software)

## 例子
### AHK v2
```ahk
; 运行记事本，输入“Hello world!”并通过鼠标全选文本。

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
; 运行记事本，输入“Hello world!”并通过鼠标全选文本。

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

## 相关推荐
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)