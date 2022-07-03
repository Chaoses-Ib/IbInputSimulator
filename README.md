# IbInputSimulator
A library for simulating keyboard, mouse and other inputs.

## Download
[Releases](https://github.com/Chaoses-Ib/IbInputSimulator/releases)

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
* [MouClassInputInjection](https://github.com/Chaoses-Ib/MouClassInputInjection)  
  e.g. `IbSendInit("MouClassInputInjection", 1, process_id)`
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
* [EDI](https://t.me/Chaoses_Ib) (paid)

## Software Compatibility
Software | SendInput | Logitech | Razer | MCII | DD | EDI | DM | Other
--- | --- | --- | --- | --- | --- | --- | --- | ---
Blade & Soul (Korean) <!--220703--> | | ✔️ | | | ❗ | ✔️ | ✔️ | ❌ SendInput hook

## Supported Languages
### AutoHotkey
<table>
<thead><tr>
    <th>AutoHotkey v2</th>
    <th>AutoHotkey v1</th>
</tr></thead>
<tbody>
    <tr>
        <td><pre lang="ahk">; Run Notepad, type "Hello world!"
; and then select all text by mouse.
<br/>
#Include "IbInputSimulator.ahk"
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
#Include IbInputSimulator.ahk
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

## Build
[vcpkg](https://github.com/microsoft/vcpkg):
```
vcpkg install detours rapidjson --triplet=x64-windows-static
```
CMake:
```
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:\...\vcpkg\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
```

For the test you also need:
```
vcpkg install boost-test fmt
```
And add `-DBUILD_TESTING=ON` when calling `cmake ..` .

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