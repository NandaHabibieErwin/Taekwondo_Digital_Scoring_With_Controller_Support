#include <Windows.h>
#include <tchar.h>
#include "Helper.h"

#define INI_FILE_PATH L".\\config.ini"


void SaveSettings() {

    wchar_t juryCountStr[10];
    _itow_s(currentJury, juryCountStr, 10);
    if (!WritePrivateProfileString(L"Settings", L"JuryCount", juryCountStr, INI_FILE_PATH)) {
        MessageBox(NULL, L"Failed to save jury count.", L"Error", MB_OK | MB_ICONERROR);
    }


    if (!WritePrivateProfileString(L"Settings", L"PlayerA", playerAName, INI_FILE_PATH)) {
        MessageBox(NULL, L"Failed to save Player A's name.", L"Error", MB_OK | MB_ICONERROR);
    }

    if (!WritePrivateProfileString(L"Settings", L"PlayerB", playerBName, INI_FILE_PATH)) {
        MessageBox(NULL, L"Failed to save Player B's name.", L"Error", MB_OK | MB_ICONERROR);
    }


    wchar_t timerValue[10];
    _itow_s(countdown, timerValue, 10);
    if (!WritePrivateProfileString(L"Settings", L"Timer[Second]", timerValue, INI_FILE_PATH)) {
        MessageBox(NULL, L"Failed to save timer value.", L"Error", MB_OK | MB_ICONERROR);
    }
}


void LoadSettings() {

    currentJury = GetPrivateProfileInt(L"Settings", L"JuryCount", 1, INI_FILE_PATH);

    GetPrivateProfileString(L"Settings", L"PlayerA", L"Player A", playerAName, 50, INI_FILE_PATH);
    GetPrivateProfileString(L"Settings", L"PlayerB", L"Player B", playerBName, 50, INI_FILE_PATH);

    wchar_t timerValue[10];
    GetPrivateProfileString(L"Settings", L"Timer[Second]", L"0", timerValue, 10, INI_FILE_PATH);
    countdown = _wtoi(timerValue);
}
