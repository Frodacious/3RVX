#pragma once

#include <Windows.h>
#include <unordered_set>

#include "HotkeyManager.h"
#include "KeyReceiver.h"

class KeyGrabber {
public:
    static KeyGrabber *Instance();

    void Grab();
    void Stop();
    int KeyCombination();
    void SetHwnd(HWND updateHwnd);

private:
    HHOOK _keyHook;
    HHOOK _mouseHook;
    HWND _updateHwnd;
    int _keyCombination;

    KeyReceiver *_receiver;

    bool Hook();
    bool Unhook();

    std::wstring VKToString(unsigned int vk, bool extendedKey = false);

    LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK 
        LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);

    LRESULT CALLBACK KeyProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK 
        LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    static KeyGrabber *instance;
};