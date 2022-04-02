# IbInputSimulator
语言：[English](README.md)，[简体中文](README.zh-Hans.md)  
一个用于模拟键盘、鼠标和其它输入的库。

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

## 支持语言
### AHK
<table>
<thead><tr>
    <th>AHK v2</th>
    <th>AHK v1</th>
</tr></thead>
<tbody>
    <tr>
        <td><pre lang="ahk">; 运行记事本，输入“Hello world!”并通过鼠标全选文本。
<br/>
#Include "IbInputSimulator.ahk"
<br/>
IbSendInit() ; IbSendInit("AnyDriver", 1)
<br/>
Send("#r")
WinWaitActive("ahk_class #32770")
Send("notepad`n")
<br/>
WinWaitActive("ahk_exe notepad.exe")
Send("Hello world{!}")
Sleep(100)
MouseClickDrag("Left", 5, 5, 150, 50)</pre></td>
        <td><pre lang="ahk">; 运行记事本，输入“Hello world!”并通过鼠标全选文本。
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

## 下载
[Releases](../../releases)

## 构建
[Building](README.md#Building)

## 相关推荐
* [IbLogiSoftExt](https://github.com/Chaoses-Ib/IbLogiSoftExt)

## 鸣谢
* 罗技
  * @The-BigEagle
  * [ekknod/logitech-cve](https://github.com/ekknod/logitech-cve)：了解到罗技设备可以被直接打开
* 雷蛇
  * [Sadmeme/rzctl](https://github.com/Sadmeme/rzctl)
  * @任性：提供测试环境
* @任性：提供反馈和赞助