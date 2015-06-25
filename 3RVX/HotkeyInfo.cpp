#include "HotkeyInfo.h"

#include <sstream>

#include "HotkeyManager.h"
#include "Logger.h"

std::vector<std::wstring> HotkeyInfo::ActionNames = {
    L"Increase Volume",
    L"Decrease Volume",
    L"Set Volume",
    L"Mute",
    L"Show Volume Slider",
    L"Eject Drive",
    L"Eject Last Disk",
    L"Media Key",
    L"Run",
    L"Open Settings Dialog",
    L"Exit 3RVX",
};

std::vector<std::wstring> HotkeyInfo::MediaKeyNames = {
    L"Play/Pause",
    L"Stop",
    L"Next",
    L"Previous",
};

std::vector<unsigned short> HotkeyInfo::MediaKeyVKs = {
    VK_MEDIA_PLAY_PAUSE,
    VK_MEDIA_STOP,
    VK_MEDIA_NEXT_TRACK,
    VK_MEDIA_PREV_TRACK,
};

HotkeyInfo::HotkeyInfo() :
_cache(false) {

}

HotkeyInfo::VolumeKeyArgTypes HotkeyInfo::VolumeArgType(HotkeyInfo &hki) {
    if (hki.HasArgs() == false) {
        return VolumeKeyArgTypes::NoArgs;
    }

    if (hki.HasArg(1) == false) {
        /* No 2nd arg; assume units */
        return VolumeKeyArgTypes::Units;
    }

    int arg1 = hki.ArgToInt(1);
    return (VolumeKeyArgTypes) arg1;
}

int HotkeyInfo::ArgToInt(unsigned int argIdx) {
    if (_cache && _intArgs.count(argIdx) > 0) {
        return _intArgs[argIdx];
    }

    std::wistringstream str(args[argIdx]);
    int i;
    str >> i;

    if (_cache) {
        _intArgs[argIdx] = i;
    }

    return i;
}

double HotkeyInfo::ArgToDouble(unsigned int argIdx) {
    if (_cache && _doubleArgs.count(argIdx) > 0) {
        return _doubleArgs[argIdx];
    }

    std::wistringstream str(args[argIdx]);
    double d;
    str >> d;

    if (_cache) {
        _doubleArgs[argIdx] = d;
    }

    return d;
}

bool HotkeyInfo::HasArgs() {
    return args.empty() == false;
}

bool HotkeyInfo::HasArg(unsigned int argIdx) {
    if (args.empty()) {
        return false;
    }

    return args.size() - 1 >= argIdx;
}

void HotkeyInfo::AllocateArg(unsigned int argIdx) {
    unsigned int newSize = argIdx + 1;
    if (args.size() >= newSize) {
        return;
    }
    args.resize(newSize);
}

void HotkeyInfo::EnableArgCache() {
    _cache = true;
}

void HotkeyInfo::DisableArgCache() {
    _cache = false;
}

void HotkeyInfo::ClearArgCache() {
    _intArgs.clear();
    _doubleArgs.clear();
}

bool HotkeyInfo::Valid() {
    if (keyCombination <= 0) {
        CLOG(L"Invalid hotkey: no key combination");
        return false;
    }
    
    if (action < 0 || action > (int) ActionNames.size()) {
        CLOG(L"Invalid hotkey action");
        return false;
    }

    switch (action) {
    case HotkeyInfo::IncreaseVolume:
    case HotkeyInfo::DecreaseVolume:
    case HotkeyInfo::SetVolume: {
        if (args[0] == L"") {
            CLOG(L"Invalid hotkey: no first argument");
            return false;
        }

        int amount = ArgToInt(0);
        if (amount < 0 || amount > 100) {
            /* Amounts of 0 - 100 volume units or % are allowed */
            CLOG(L"Invalid hotkey: volume amount out of range");
            return false;
        }

        if (action != HotkeyInfo::SetVolume && amount == 0) {
            CLOG(L"Invalid hotkey: volume increment must be nonzero");
            return false;
        }

        if (HasArg(1)) {
            int type = ArgToInt(1);
            if (type < 0 || type > 2) {
                CLOG(L"Invalid hotkey: unknown increment type");
                return false;
            }
        }
            break;
    }

    case HotkeyInfo::EjectDrive:
    case HotkeyInfo::MediaKey:
    case HotkeyInfo::Run:
        if (HasArgs() == false) {
            CLOG(L"Invalid hotkey: argument required");
            return false;
        }
        break;
    }

    return true;
}

std::wstring HotkeyInfo::ToString() {
    std::wstring combination = HotkeyManager::HotkeysToString(keyCombination);
    std::wstring act = L"(none)";
    if (action >= 0 && (unsigned int) action < ActionNames.size()) {
        act = ActionNames[action];
    }
    std::wstring argStrs;
    for (std::wstring arg : args) {
        argStrs += L"'" + arg + L"' ";
    }
    return combination + L" -> " + act + L" [ " + argStrs + L"]";
}

