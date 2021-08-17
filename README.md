# IbAhkSendLib
Languages: [English](README.md), [简体中文](README.zh-Hans.md)  
An [AutoHotkey](https://www.autohotkey.com/) library that enables AHK to send keystrokes by drivers.

## Supported Drivers
* [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html) (No Logitech hardware required)
* [Logitech Gaming Software](https://support.logi.com/hc/en-gb/articles/360025298053-Logitech-Gaming-Software) (No Logitech hardware required)

## Example
<table>
<thead><tr>
    <th>AHK v2</th>
    <th>AHK v1</th>
</tr></thead>
<tbody>
    <tr>
        <td><pre lang="ahk">; Run Notepad, type "Hello world!"
; and then select all text by mouse.
<br/>
#Include "IbAhkSend.ahk"
<br/>
IbSendInit()  ; IbSendInit("AnyDriver", 1)
<br/>
Send("#r")
WinWaitActive("ahk_class #32770")
Send("notepad`n")
<br/>
WinWaitActive("ahk_exe notepad.exe")
Send("Hello world+1")
Sleep(100)
MouseClickDrag("Left", 5, 5, 150, 50)</pre></td>
        <td><pre lang="ahk">; Run Notepad, type "Hello world!"
; and then select all text by mouse.
<br/>
#Include IbAhkSend.ahk
<br/>
IbSendInit() ; IbSendInit("AnyDriver", 1)
<br/>
Send #r
WinWaitActive, ahk_class #32770
Send notepad`n
<br/>
WinWaitActive, ahk_exe notepad.exe
Send Hello world+1
Sleep 100
CoordMode, Mouse, Client
MouseClickDrag, Left, 5, 5, 150, 50</pre></td>
    </tr>
</tbody>
</table>

## Download
[Releases](../../releases)

## See Also
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)