# IbInputSimulator
A library for simulating keyboard and mouse input with drivers.

## Supported drivers
- [Logitech Gaming Software](https://support.logi.com/hc/en-gb/articles/360025298053-Logitech-Gaming-Software)

  No Logitech hardware required. However, in the new versions of LGS, the mouse driver has been removed ([#8](https://github.com/Chaoses-Ib/IbInputSimulator/issues/8)). You can install [v9.02.65](https://github.com/Chaoses-Ib/IbLogiSoftExt/releases/download/v0.1/LGS.v9.02.65_x64.exe) to make the mouse driver available.

  e.g. `IbSendInit("Logitech")`

- [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html)

  No Logitech hardware required. However, in the new versions of G HUB (≥ 2022.3.2300), the mouse driver has been removed ([#8](https://github.com/Chaoses-Ib/IbInputSimulator/issues/8)). Unfortunately, there is currently no known way to install an old version.

  e.g. `IbSendInit("Logitech")`
  
- [Logitech G HUB](https://www.logitechg.com/innovation/g-hub.html) new

  No Logitech hardware required. Works with new versions of the G HUB. Mouse input stops working after the first reboot following the installation of the G HUB. To restore mouse functionality, you may need to reinstall G HUB with "Transfer my current settings" turned off. ([#8](https://github.com/Chaoses-Ib/IbInputSimulator/issues/8))

  e.g. `IbSendInit("LogitechGHubNew")`

- [Razer Synapse 3](https://www.razer.com/synapse-3)

  In the new versions of Razer Synapse, Razer hardware is required to make the driver available. The old versions do not require Razer hardware, but the online installer of Razer Synapse can only install the newest version. To install an old version manually, see [#7](https://github.com/Chaoses-Ib/IbInputSimulator/issues/7) for details.

  e.g. `IbSendInit("Razer")`

- [MouClassInputInjection](https://github.com/Chaoses-Ib/MouClassInputInjection)

  e.g. `IbSendInit("MouClassInputInjection", 1, process_id)`

- [DD Virtual Mouse & Virtual Keyboard](https://github.com/ddxoft/master)

  May cause a blue screen; difficult to uninstall cleanly; need network.

  To use it, put the DLL (`DD94687.64.dll`/`DD64.dll`/`DDHID64.dll`) with your script file, and then:
  ```ahk
  IbSendInit("DD")
  ```
  or specify the DLL path when call IbSendInit:
  ```ahk
  IbSendInit("DD", 1, "C:\SomeDir\DD64.dll")
  ```

- [EDI](https://t.me/Chaoses_Ib) (private, not for sale)

## Software compatibility
Software | SendInput | Logitech | Razer | MCII | DD | EDI | DM | Other
--- | --- | --- | --- | --- | --- | --- | --- | ---
Blade & Soul (Korean) <!--220703--> | | ✔️ | | | ❗ | ✔️ | ✔️ | ❌ SendInput hook
Genshin | [High](https://meta.appinn.net/t/topic/44865/10?u=chaoses_ib)

For SendInput, software with `High` indicates that the target process usually has a high integrity level, which may block SendInput due to UIPI. To put it simply, this means that you need to run the input simulator with administrator privileges.

## Supported languages
### C++
[Build](docs/README.md#build)

See [InputSimulator.hpp](Simulator/include/IbInputSimulator/InputSimulator.hpp) for the API.

### AutoHotkey
Download: [Releases](https://github.com/Chaoses-Ib/IbInputSimulator/releases)

Usage:

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

## Related projects
- [GameMate: Macro Activator & Crosshair Timer](https://github.com/Pennywise007/GameMate)

  GameMate is a powerful Windows application designed to enhance your productivity and gaming experience. It allows you to customize key bindings for different applications, add a crosshair overlay for games, block accidental key presses, track your time efficiently, and automate complex sequences of actions.

- [IbLogiSoftExt: An extension for Logitech Gaming Software. Support sending G-keys to AutoHotkey.](https://github.com/Chaoses-Ib/IbLogiSoftExt)

## Credits
- Logitech
  - @Eagle1020
  - [ekknod/logitech-cve](https://github.com/ekknod/logitech-cve) for learning that Logitech devices can be opened directly
- Razer
  - [Sadmeme/rzctl](https://github.com/Sadmeme/rzctl)
  - @任性 for providing test environment

Sponsors:

Date | Sponsor | Comment
--- | --- | ---
2022-04-03 | 任性 | MouClassInputInjection
2023-04-22 | 任性 | Logitech
2023-02-28 | - |
2021-08-28 | 任性 | Razer