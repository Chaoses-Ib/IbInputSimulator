; IbAhkSendLib (v1)
; Description: Enable AHK to send keystrokes by drivers.
; Author: Chaoses Ib
; Version: 210815
; Git: https://github.com/Chaoses-Ib/IbAhkSendLib

IbSendInit(mode := 0){
    static hModule := DllCall("LoadLibrary", "Str", A_ScriptDir "\IbAhkSend.dll", "Ptr")
    if (hModule == 0){
        if (A_PtrSize == 4)
            throw "LibLoadingFailed: Please use AutoHotkey x64"
        else if (!FileExist(A_ScriptDir "\IbAhkSend.dll"))
            throw "LibLoadingFailed: Please put IbAhkSend.dll with your script file (or use AHK v2 instead, which can locate those DLLs that are put with the library files)"
        else
            throw "LibLoadingFailed"
    }

    result := DllCall("IbAhkSend\IbAhkSendInit", "Int")
    if (result != 0){
        error_text := ["DeviceNotFound", "DeviceOpeningFailed", "LogiSettingsNotFound"]
        throw error_text[result - 1]
    }

    if (mode != 0){
        IbSendMode(mode)
    }
}

IbSendMode(mode){
    static ahk_mode := ""
    if (mode == 1){
        DllCall("IbAhkSend\IbAhkSendInputHookBegin")
        ahk_mode := A_SendMode
        SendMode Input
    } else if (mode == 0){
        SendMode %ahk_mode%
        DllCall("IbAhkSend\IbAhkSendInputHookEnd")
    } else {
        throw "Invalid argument"
    }
}

IbSendDestroy(){
    DllCall("IbAhkSend\IbAhkSendDestroy")
    ;DllCall("FreeLibrary", "Ptr", hModule)
}


IbSend(keys){
    DllCall("IbAhkSend\IbAhkSendInputHookBegin")  ;or IbSendMode(1)
    SendInput %keys%
    DllCall("IbAhkSend\IbAhkSendInputHookEnd")  ;or IbSendMode(0)
}