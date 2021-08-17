; IbAhkSendLib (v1)
; Description: Enable AHK to send keystrokes by drivers.
; Author: Chaoses Ib
; Version: 210815
; Git: https://github.com/Chaoses-Ib/IbAhkSendLib

IbSendInit(send_type := "AnyDriver", mode := 1){
    static hModule := DllCall("LoadLibrary", "Str", A_ScriptDir "\IbAhkSend.dll", "Ptr")
    if (hModule == 0){
        if (A_PtrSize == 4)
            throw "LibLoadingFailed: Please use AutoHotkey x64"
        else if (!FileExist(A_ScriptDir "\IbAhkSend.dll"))
            throw "LibLoadingFailed: Please put IbAhkSend.dll with your script file (or use AHK v2 instead, which can locate those DLLs that are put with the library files)"
        else
            throw "LibLoadingFailed"
    }

    send_type_table := ["AnyDriver", "SendInput", "Logitech"]
    send_type_v := 0
    for i, e in send_type_table
    {
        if (e == send_type){
            send_type_v := i
            break
        }
    }
    if (send_type_v == 0)
        throw "Invalid send type"
    send_type_v := send_type_v - 1

    result := DllCall("IbAhkSend\IbAhkSendInit", "Int", send_type_v, "Int", 0, "Ptr", 0, "Int")
    if (result != 0){
        error_text := ["DeviceNotFound", "DeviceOpeningFailed"]
        throw error_text[result]
    }

    if (mode != 0){
        IbSendMode(mode)
    }
}

IbSendMode(mode){
    static ahk_mode := ""
    if (mode == 1){
        DllCall("IbAhkSend\IbAhkSendInputHook", "Int", 1)
        ahk_mode := A_SendMode
        SendMode Input
    } else if (mode == 0){
        SendMode %ahk_mode%
        DllCall("IbAhkSend\IbAhkSendInputHook", "Int", 0)
    } else {
        throw "Invalid argument"
    }
}

IbSendDestroy(){
    DllCall("IbAhkSend\IbAhkSendDestroy")
    ;DllCall("FreeLibrary", "Ptr", hModule)
}

IbSyncKeyStates(){
    DllCall("IbAhkSend\IbAhkSendSyncKeyStates")
}

IbSend(keys){
    DllCall("IbAhkSend\IbAhkSendInputHook", "Int", 1)  ;or IbSendMode(1)
    SendInput %keys%
    DllCall("IbAhkSend\IbAhkSendInputHook", "Int", 0)  ;or IbSendMode(0)
}