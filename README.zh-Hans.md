# IbAhkSendLib
语言：[English](README.md)，[简体中文](README.zh-Hans.md)  
[AutoHotkey](https://www.autohotkey.com/) 驱动按键库。  

## 支持驱动
* [罗技 G HUB](https://www.logitechg.com.cn/zh-cn/innovation/g-hub.html)（不需要罗技硬件）
* [罗技游戏软件](https://support.logi.com/hc/zh-cn/articles/360025298053)（不需要罗技硬件）

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
IbSendInit(1)
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
IbSendInit(1)
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