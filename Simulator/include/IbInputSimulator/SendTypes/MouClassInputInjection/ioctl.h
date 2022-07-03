/*++

Copyright (c) 2019 changeofpace. All rights reserved.

Use of this source code is governed by the MIT license. See the 'LICENSE' file
for more information.

--*/

#pragma once

#if defined(_KERNEL_MODE)
#include <fltKernel.h>
#else
#include <Windows.h>
#endif

#include <devioctl.h>
#include <ntddmou.h>

//=============================================================================
// Names
//=============================================================================
#define DRIVER_NAME_U           L"MouClassInputInjection"
#define LOCAL_DEVICE_PATH_U     (L"\\\\.\\" DRIVER_NAME_U)
#define NT_DEVICE_NAME_U        (L"\\Device\\" DRIVER_NAME_U)
#define SYMBOLIC_LINK_NAME_U    (L"\\DosDevices\\" DRIVER_NAME_U)

//=============================================================================
// Ioctls
//=============================================================================
#define FILE_DEVICE_MOUCLASS_INPUT_INJECTION    48781ul

#define IOCTL_INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT \
    CTL_CODE(                                       \
        FILE_DEVICE_MOUCLASS_INPUT_INJECTION,       \
        2600,                                       \
        METHOD_BUFFERED,                            \
        FILE_ANY_ACCESS)

#define IOCTL_INJECT_MOUSE_BUTTON_INPUT         \
    CTL_CODE(                                   \
        FILE_DEVICE_MOUCLASS_INPUT_INJECTION,   \
        2850,                                   \
        METHOD_BUFFERED,                        \
        FILE_ANY_ACCESS)

#define IOCTL_INJECT_MOUSE_MOVEMENT_INPUT       \
    CTL_CODE(                                   \
        FILE_DEVICE_MOUCLASS_INPUT_INJECTION,   \
        2851,                                   \
        METHOD_BUFFERED,                        \
        FILE_ANY_ACCESS)

#define IOCTL_INJECT_MOUSE_INPUT_PACKET         \
    CTL_CODE(                                   \
        FILE_DEVICE_MOUCLASS_INPUT_INJECTION,   \
        2870,                                   \
        METHOD_BUFFERED,                        \
        FILE_ANY_ACCESS)

//=============================================================================
// IOCTL_INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT
//=============================================================================
typedef struct _MOUSE_CLASS_BUTTON_DEVICE_INFORMATION {
    USHORT UnitId;
} MOUSE_CLASS_BUTTON_DEVICE_INFORMATION,
*PMOUSE_CLASS_BUTTON_DEVICE_INFORMATION;

typedef struct _MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION {
    USHORT UnitId;
    BOOLEAN AbsoluteMovement;
    BOOLEAN VirtualDesktop;
} MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION,
*PMOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION;

typedef struct _MOUSE_DEVICE_STACK_INFORMATION {
    MOUSE_CLASS_BUTTON_DEVICE_INFORMATION ButtonDevice;
    MOUSE_CLASS_MOVEMENT_DEVICE_INFORMATION MovementDevice;
} MOUSE_DEVICE_STACK_INFORMATION, *PMOUSE_DEVICE_STACK_INFORMATION;

typedef struct _INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY {
    MOUSE_DEVICE_STACK_INFORMATION DeviceStackInformation;
} INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY,
*PINITIALIZE_MOUSE_DEVICE_STACK_CONTEXT_REPLY;

//=============================================================================
// IOCTL_INJECT_MOUSE_BUTTON_INPUT
//=============================================================================
typedef struct _INJECT_MOUSE_BUTTON_INPUT_REQUEST {
    ULONG_PTR ProcessId;
    USHORT ButtonFlags;
    USHORT ButtonData;
} INJECT_MOUSE_BUTTON_INPUT_REQUEST, *PINJECT_MOUSE_BUTTON_INPUT_REQUEST;

//=============================================================================
// IOCTL_INJECT_MOUSE_MOVEMENT_INPUT
//=============================================================================
typedef struct _INJECT_MOUSE_MOVEMENT_INPUT_REQUEST {
    ULONG_PTR ProcessId;
    USHORT IndicatorFlags;
    LONG MovementX;
    LONG MovementY;
} INJECT_MOUSE_MOVEMENT_INPUT_REQUEST, *PINJECT_MOUSE_MOVEMENT_INPUT_REQUEST;

//=============================================================================
// IOCTL_INJECT_MOUSE_INPUT_PACKET
//=============================================================================
typedef struct _INJECT_MOUSE_INPUT_PACKET_REQUEST {
    ULONG_PTR ProcessId;
    BOOLEAN UseButtonDevice;
    MOUSE_INPUT_DATA InputPacket;
} INJECT_MOUSE_INPUT_PACKET_REQUEST, *PINJECT_MOUSE_INPUT_PACKET_REQUEST;
