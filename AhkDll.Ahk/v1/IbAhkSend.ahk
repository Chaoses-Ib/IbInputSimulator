; IbAhkSendLib (v1)
; Description: Enable AHK to send keystrokes by drivers.
; Author: Chaoses Ib
; Version: 0.3
; Git: https://github.com/Chaoses-Ib/IbAhkSendLib

IbSendInit(send_type := "AnyDriver", mode := 1, args*){
    workding_dir := A_WorkingDir
    SetWorkingDir, %A_ScriptDir%

    static hModule := DllCall("LoadLibrary", "Str", A_ScriptDir "\IbAhkSend.dll", "Ptr")
    if (hModule == 0){
        if (A_PtrSize == 4)
            throw "SendLibLoadFailed: Please use AutoHotkey x64"
        else if (!FileExist("IbAhkSend.dll"))
            throw "SendLibLoadFailed: Please put IbAhkSend.dll with your script file (or use AHK v2 instead, which can locate those DLLs that are put with the library files)"
        else
            throw "SendLibLoadFailed"
    }

    if (send_type == "AnyDriver")
        result := DllCall("IbAhkSend\IbSendInit", "Int", 0, "Int", 0, "Ptr", 0, "Int")
    else if (send_type == "SendInput")
        result := DllCall("IbAhkSend\IbSendInit", "Int", 1, "Int", 0, "Ptr", 0, "Int")
    else if (send_type == "Logitech")
        result := DllCall("IbAhkSend\IbSendInit", "Int", 2, "Int", 0, "Ptr", 0, "Int")
    else if (send_type == "Razer")
        result := DllCall("IbAhkSend\IbSendInit", "Int", 3, "Int", 0, "Ptr", 0, "Int")
    else if (send_type == "DD"){
        if (args.MaxIndex() == 1)
            result := DllCall("IbAhkSend\IbSendInit", "Int", 4, "Int", 0, "WStr", args[1], "Int")
        else
            result := DllCall("IbAhkSend\IbSendInit", "Int", 4, "Int", 0, "Ptr", 0, "Int")
    } else
        throw "Invalid send type"
    
    SetWorkingDir, %workding_dir%
    
    if (result != 0){
        error_text := ["InvalidArgument", "LibraryNotFound", "LibraryLoadFailed", "LibraryError", "DeviceCreateFailed", "DeviceNotFound", "DeviceOpenFailed"]
        throw error_text[result]
    }

    if (mode != 0){
        IbSendMode(mode)
    }
}

IbSendMode(mode){
    static ahk_mode := ""
    if (mode == 1){
        DllCall("IbAhkSend\IbSendInputHook", "Int", 1)
        ahk_mode := A_SendMode
        SendMode Input
    } else if (mode == 0){
        SendMode %ahk_mode%
        DllCall("IbAhkSend\IbSendInputHook", "Int", 0)
    } else {
        throw "Invalid send mode"
    }
}

IbSendDestroy(){
    DllCall("IbAhkSend\IbSendDestroy")
    ;DllCall("FreeLibrary", "Ptr", hModule)
}

IbSyncKeyStates(){
    DllCall("IbAhkSend\IbSendSyncKeyStates")
}

IbSend(keys){
    DllCall("IbAhkSend\IbSendInputHook", "Int", 1)  ;or IbSendMode(1)
    SendInput %keys%
    DllCall("IbAhkSend\IbSendInputHook", "Int", 0)  ;or IbSendMode(0)
}