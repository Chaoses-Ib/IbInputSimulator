# IbAhkSendLib
Languages: [English](README.md), [简体中文](README.zh-Hans.md)  
An [AutoHotkey](https://www.autohotkey.com/) library that enables AHK to send keystrokes by drivers.

## Supported Drivers
* [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html)  
  No Logitech hardware required.  
  e.g. `IbSendInit("Logitech")`
* [Logitech Gaming Software](https://support.logi.com/hc/en-gb/articles/360025298053-Logitech-Gaming-Software)  
  No Logitech hardware required.  
  e.g. `IbSendInit("Logitech")`
* [Razer Synapse 3](https://www.razer.com/synapse-3)  
  No Razer hardware required, but it will be safer if you have one.  
  e.g. `IbSendInit("Razer")`
* [DD Virtual Mouse & Virtual Keyboard](https://github.com/ddxoft/master)  
  May cause a blue screen; difficult to uninstall cleanly; need network.  
  To use it, put the DLL (`DD94687.64.dll`/`DD64.dll`/`DDHID64.dll`) with your script file, and then:
  ```ahk
  IbSendInit("DD")
  ```
  or specify the DLL path when call IbSendInit:
  ```ahk
  IbSendInit("DD", 1, "C:\SomeDir\DD64.dll")
  ```

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
Send("Hello world{!}")
Sleep(100)
MouseClickDrag("Left", 5, 5, 150, 50)</pre></td>
        <td><pre lang="ahk">; Run Notepad, type "Hello world!"
; and then select all text by mouse.
<br/>
#Include %A_ScriptDir%
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
Send Hello world{!}
Sleep 100
CoordMode, Mouse, Client
MouseClickDrag, Left, 5, 5, 150, 50</pre></td>
    </tr>
</tbody>
</table>

## Downloading
[Releases](../../releases)

## Building
1. Put [IbWinCppLib](https://github.com/Chaoses-Ib/IbWinCppLib/tree/master/WinCppLib/IbWinCppLib) in `C:\L\C++\packages` (in other locations you need to modify the .vcxproj files).
1. [vcpkg](https://github.com/microsoft/vcpkg)
    ```
    set VCPKG_DEFAULT_TRIPLET=x64-windows-static-md
    vcpkg install detours rapidjson
    ```
    For Test project you also need:
    ```
    vcpkg install boost-test fmt
    ```
    Change VCPKG_DEFAULT_TRIPLET to x86-windows-static-md if you need x86 version. 

## See Also
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)

## Credits
* Logitech
  * @The-BigEagle
  * [ekknod/logitech-cve](https://github.com/ekknod/logitech-cve) for learning that Logitech devices can be opened directly
* Razer
  * [Sadmeme/rzctl](https://github.com/Sadmeme/rzctl)
  * @任性 for providing test environment
* @任性 for providing feedback and sponsorship