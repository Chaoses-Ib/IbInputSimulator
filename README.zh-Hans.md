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
* [DD 虚拟鼠标 & 虚拟键盘](https://github.com/ddxoft/master)  
  可能会导致蓝屏；难以卸载干净。  
  使用时需要把 DLL 文件（`DD94687.64.dll`/`DD64.dll`/`DDHID64.dll`）与你的脚本文件放到一起，或者在调用 IbSendInit 时指定路径参数：
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