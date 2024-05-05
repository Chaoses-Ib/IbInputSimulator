#pragma once
#include "base.hpp"
#include "MouClassInputInjection/ioctl.h"

namespace Send::Type::Internal {
    class MouClassInputInjection final : virtual public Base {
    public:
        MouClassInputInjection() {}
        
        Error create(ULONG_PTR pid) {
            device = CreateFileW(
                LOCAL_DEVICE_PATH_U,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
            );
            if (device == INVALID_HANDLE_VALUE)
                return Error::DeviceNotFound;
            
            DWORD bytes_returned;
            if (!DeviceIoControl(
                device,
                IOCTL_INITIALIZE_MOUSE_DEVICE_STACK_CONTEXT,
                nullptr, 0,
                &device_info, sizeof(device_info),
                &bytes_returned,
                nullptr
            )) {
                CloseHandle(device);
                return Error::DeviceOpenFailed;
            }

            this->pid = pid;

            return Error::Success;
        }
        
        void destroy() override {
            CloseHandle(device);
        }

        bool send_mouse_input(const MOUSEINPUT& mi) override {
            if (mi.dwFlags & MOUSEEVENTF_MOVE) {
                USHORT flags = 0;
                POINT move{ mi.dx, mi.dy };
                
                // MOUSE_MOVE_RELATIVE: 0
                if (device_info.MovementDevice.AbsoluteMovement) {
                    flags |= MOUSE_MOVE_ABSOLUTE;
                    if (!(mi.dwFlags & MOUSEEVENTF_ABSOLUTE)) {
                        POINT cursor;
                        GetCursorPos(&cursor);
                        move.x += cursor.x;
                        move.y += cursor.y;
                    }
                    else {
                        //@todo mouse_absolute_to_screen(move);
                    }
                }
                else {
                    if (mi.dwFlags & MOUSEEVENTF_ABSOLUTE) {
                        mouse_absolute_to_screen(move);
                        mouse_screen_to_relative(move);
                    }
                }
                    
                //@todo mi.dwFlags & MOUSEEVENTF_VIRTUALDESK
                if (device_info.MovementDevice.VirtualDesktop)
                    flags |= MOUSE_VIRTUAL_DESKTOP;
                
                // MOUSE_ATTRIBUTES_CHANGED
                
                if (mi.dwFlags & MOUSEEVENTF_MOVE_NOCOALESCE)
                    flags |= MOUSE_MOVE_NOCOALESCE;
                
                // MOUSE_TERMSRV_SRC_SHADOW: unknown
                
                INJECT_MOUSE_MOVEMENT_INPUT_REQUEST request{
                    .ProcessId = pid,
                    .IndicatorFlags = flags,
                    .MovementX = move.x,
                    .MovementY = move.y
                };
                DWORD bytes_returned;
                DeviceIoControl(
                    device,
                    IOCTL_INJECT_MOUSE_MOVEMENT_INPUT,
                    &request, sizeof(request),
                    nullptr, 0,
                    &bytes_returned,
                    nullptr
                );
            }

            if (mi.dwFlags & 0x7E) {
                DWORD keys[] = { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP };
                for (DWORD key : keys)
                    if (mi.dwFlags & key)
                        inject_button(key >> 1);
            }
            if (mi.dwFlags & (MOUSEEVENTF_XDOWN | MOUSEEVENTF_XUP)) {
                bool down = mi.dwFlags & MOUSEEVENTF_XDOWN;
                switch (mi.mouseData) {
                case XBUTTON1: inject_button(down ? MOUSE_BUTTON_4_DOWN : MOUSE_BUTTON_4_UP); break;
                case XBUTTON2: inject_button(down ? MOUSE_BUTTON_5_DOWN : MOUSE_BUTTON_5_UP); break;
                }
            }
            else if (mi.dwFlags & MOUSEEVENTF_WHEEL)
                return false;

            //@todo
            return true;
        }

        bool send_keyboard_input(const KEYBDINPUT& ki) override {
            return false;
        }
        
    private:
        HANDLE device;
        MOUSE_DEVICE_STACK_INFORMATION device_info;
        ULONG_PTR pid;

        bool inject_button(USHORT button) {
            INJECT_MOUSE_BUTTON_INPUT_REQUEST request{
                .ProcessId = pid,
                .ButtonFlags = button,
                .ButtonData = 0
            };
            DWORD bytes_returned;
            return DeviceIoControl(
                device,
                IOCTL_INJECT_MOUSE_BUTTON_INPUT,
                &request, sizeof(request),
                nullptr, 0,
                &bytes_returned,
                nullptr
            );
        }
    };
}