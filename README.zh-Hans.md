# IbAhkSendLib
语言：[English](README.md)，[简体中文](README.zh-Hans.md)  
[AutoHotkey](https://www.autohotkey.com/) 驱动按键库。  

## 支持驱动
* [罗技 G HUB](https://www.logitechg.com.cn/zh-cn/innovation/g-hub.html)  
  不需要罗技硬件。  
  用例：`IbSendInit("Logitech")`
* [罗技游戏软件](https://support.logi.com/hc/zh-cn/articles/360025298053)  
  不需要罗技硬件。  
  用例：`IbSendInit("Logitech")`
* [雷蛇雷云3](http://cn.razerzone.com/synapse-3)  
  不需要雷蛇硬件，不过如果有的话会更安全。  
  用例：`IbSendInit("Razer")`
* [DD 虚拟鼠标 & 虚拟键盘](https://github.com/ddxoft/master)  
  可能会导致蓝屏；难以卸载干净；需要联网。  
  使用时需要把 DLL 文件（`DD94687.64.dll`/`DD64.dll`/`DDHID64.dll`）与你的脚本文件放到一起，然后：
  ```ahk
  IbSendInit("DD")
  ```
  或者在调用 IbSendInit 时指定 DLL 路径：
  ```ahk
  IbSendInit("DD", 1, "C:\SomeDir\DD64.dll")
  ```

## 例子
<table>
<thead><tr>
    <th>AHK v2</th>
    <th>AHK v1</th>
</tr></thead>
<tbody>
    <tr>
        <td><pre lang="ahk">; 运行记事本，输入“Hello world!”并通过鼠标全选文本。
<br/>
#Include "IbAhkSend.ahk"
<br/>
IbSendInit() ; IbSendInit("AnyDriver", 1)
<br/>
Send("#r")
WinWaitActive("ahk_class #32770")
Send("notepad`n")
<br/>
WinWaitActive("ahk_exe notepad.exe")
Send("Hello world+1")
Sleep(100)
MouseClickDrag("Left", 5, 5, 150, 50)</pre></td>
        <td><pre lang="ahk">; 运行记事本，输入“Hello world!”并通过鼠标全选文本。
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
Send Hello world+1
Sleep 100
CoordMode, Mouse, Client
MouseClickDrag, Left, 5, 5, 150, 50</pre></td>
    </tr>
</tbody>
</table>

## 下载
[Releases](../../releases)

## 相关推荐
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)

## 开发者
### 构建
1. 将 [IbWinCppLib](https://github.com/Chaoses-Ib/IbWinCppLib/tree/master/WinCppLib/IbWinCppLib) 放入 `C:\L\C++\packages`（其它位置需要修改 .vcxproj 文件）。
1. [vcpkg](https://github.com/microsoft/vcpkg)
    ```
    set VCPKG_DEFAULT_TRIPLET=x64-windows-static-md
    vcpkg install detours rapidjson
    ```
    Test 项目还需要：
    ```
    vcpkg install boost-test fmt
    ```
    如果需要 x86 版本就把 VCPKG_DEFAULT_TRIPLET 改为  x86-windows-static-md。