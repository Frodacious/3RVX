#include "stdafx.h"
#include "SettingsUI.h"
#include "General.h"
#include "afxdialogex.h"

#include "../3RVX/SkinInfo.h"

#define KEY_NAME L"3RVXv3"
#define STARTUP_KEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"

IMPLEMENT_DYNAMIC(General, CPropertyPage)

General::General(Settings *settings) :
CPropertyPage(General::IDD),
_settings(settings) {
}

General::~General()
{
}

void General::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, CHK_STARTUP, _startup);
    DDX_Control(pDX, CHK_NOTIFY, _notify);
    DDX_Control(pDX, CHK_SOUNDS, _sounds);
    DDX_Control(pDX, CMB_LANG, _lang);
    DDX_Control(pDX, GRP_SKIN, _skinGrp);
    DDX_Control(pDX, GRP_BEHAVIOR, _behaviorGrp);
    DDX_Control(pDX, GRP_LANGUAGE, _languageGrp);
    DDX_Control(pDX, LBL_AUTHOR, _author);
    DDX_Control(pDX, CMB_SKIN, _skins);
    DDX_Control(pDX, BTN_WEBSITE, _website);
}

BOOL General::OnInitDialog() {
    CPropertyPage::OnInitDialog();

    LoadSettings();

    /* Determine which skins are available */
    std::list<CString> skins = FindSkins(L"../3RVX/Skins");
    for (CString skin : skins) {
        _skins.AddString(skin);
    }

    /* Update the combo box with the current skin */
    std::wstring current = _settings->GetText("skin");
    int idx = _skins.SelectString(0, current.c_str());
    if (idx == CB_ERR) {
        current = L"Default";
        _skins.SelectString(0, L"Default");
    }

    std::wstring skinXML = L"../3RVX/" SKINS_DIR L"/" + current + L"/" SKIN_XML;
    OutputDebugString(skinXML.c_str());
    SkinInfo s(skinXML);

    CString authorText(L"Author:");
    authorText.Append(L" ");
    authorText.Append(s.Author().c_str());
    _author.SetWindowTextW(authorText);

    _url = s.URL();
    if (_url == L"") {
        _website.EnableWindow(false);
    }

    /* Are we set to run on startup in the registry? */
    CRegKey rk;
    int result = rk.Open(HKEY_CURRENT_USER, STARTUP_KEY, KEY_READ);
    if (result == ERROR_SUCCESS) {
        CString str;
        ULONG bufLen = 1024;
        LPTSTR buf = str.GetBufferSetLength(bufLen);

        int queryResult = rk.QueryStringValue(KEY_NAME, buf, &bufLen);
        if (queryResult == ERROR_SUCCESS) {
            _startup.SetCheck(true);
        }
    }

    return TRUE;
}

std::list<CString> General::FindSkins(CString dir) {
    std::list<CString> skins;

    CFileFind ff;
    dir += L"\\*";
    BOOL result = ff.FindFile(dir);
    while (result) {
        result = ff.FindNextFile();
        if (ff.IsDots()) {
            continue;
        }

        if (ff.IsDirectory()) {
            CFileFind inDir;
            CString dirPath = ff.GetFilePath();
            dirPath += L"\\skin.xml";
            if (inDir.FindFile(dirPath)) {
                /* We found a skin XML file; add the skin dir to our list. */
                skins.push_back(ff.GetFileName());
            }
        }
    }

    return skins;
}

void General::LoadSettings() {
    _notify.SetCheck(_settings->IsEnabled("notifyIcon"));
    _sounds.SetCheck(_settings->IsEnabled("soundEffects"));
}

void General::EnableRunOnStartup() {
    wchar_t path[1024];
    GetModuleFileName(NULL, path, 1024);
    PathRemoveFileSpec(path);
    CString exePath(path);
    exePath.Append(L"\\3RVX.exe");

    CRegKey rk;
    int result = rk.Open(HKEY_CURRENT_USER, STARTUP_KEY, KEY_WRITE);
    if (result == ERROR_SUCCESS) {
        rk.SetStringValue(KEY_NAME, exePath, REG_SZ);
    }
}

void General::OnBnClickedWebsite() {
    ShellExecute(NULL, L"open", _url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

BEGIN_MESSAGE_MAP(General, CPropertyPage)
    ON_BN_CLICKED(BTN_WEBSITE, &General::OnBnClickedWebsite)
END_MESSAGE_MAP()