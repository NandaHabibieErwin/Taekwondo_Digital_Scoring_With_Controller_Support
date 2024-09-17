#include "Jury.h"
#include "Resource.h";
#include "framework.h"  // Framework header if used across the project
#include "Main.h"       // Access global variables
#include <unordered_map>
#include <windows.h>
#include <tchar.h>
#include <algorithm>
#include <vector>
#include <memory>
#include<SFML/Audio.hpp>
#include <string>
#include <Shlwapi.h> // For PathRemoveFileSpec
#include <codecvt>
#pragma comment(lib, "Shlwapi.lib") // Link to Shlwapi.lib for PathRemoveFileSpec

int currentJury = 1;
int selectedJury = 0;



// Function to handle input for score management
void HandleInput(HWND hWnd, RAWKEYBOARD& rawKB, HANDLE hDevice,
    std::unordered_map<HANDLE, bool>& numpadState, int& score,
    int points, bool& scoreUpdated, bool isPlayerA)
{
    if (rawKB.Message == WM_KEYDOWN) {
        giveScoreForA = isPlayerA;  // Track whether it's Player A or B being scored
        numpadState[hDevice] = true;
    }
    else if (rawKB.Message == WM_KEYUP) {
        numpadState[hDevice] = false;
    }

    int pressedCount = 0;
    for (const auto& state : numpadState) {
        if (state.second) {
            pressedCount++;
        }
    }

    // If exactly 1 jury presses the key and we want to reduce the score
    if (pressedCount == 1 && points < 0 && !scoreUpdated && score + points >= 0) {
        score += points;  // Reduce score
        scoreUpdated = true;
        PlayAudio(L"scorehit.wav", false);  // Play non-looping sound

        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }

    // Normal score update logic for adding points
    if (pressedCount >= currentJury && !scoreUpdated && points > 0) {
        score += points;
        scoreUpdated = true;
        PlayAudio(L"scorehit.wav", false);  // Play non-looping sound

        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }



    if (pressedCount == 0) {
        scoreUpdated = false;
    }
}


// Function to register multiple keyboards for input handling
void RegisterMultipleKeyboards(HWND hwnd) {
    RAWINPUTDEVICE rid[1];

    rid[0].usUsagePage = 0x01; // Generic desktop controls
    rid[0].usUsage = 0x06;     // Keyboard
    rid[0].dwFlags = RIDEV_INPUTSINK; // Capture input even when not in focus
    rid[0].hwndTarget = hwnd; // Target the main window

    if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
        MessageBox(NULL, L"Failed to register raw input devices.", L"Error", MB_OK);
    }
}

// Jury configuration dialog callback function
INT_PTR CALLBACK JuryConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hMainWnd;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        hMainWnd = (HWND)lParam;
        selectedJury = currentJury - 1;

        HWND hComboBox = GetDlgItem(hDlg, IDC_JURY_NUMBER);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Jury");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"2 Juries");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 Juries");
        SendMessage(hComboBox, CB_SETCURSEL, (selectedJury <= 2 ? selectedJury : 0), 0);  // Ensure valid range

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_TEST)
        {
            // Testing keyboard input dialog
            DialogBox(hInst, MAKEINTRESOURCE(IDC_TEST_KEYBOARD), hDlg, TestKeyboardProc);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDOK)
        {
            HWND hComboBox = GetDlgItem(hDlg, IDC_JURY_NUMBER);
            int selectedJuryIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            currentJury = selectedJuryIndex + 1;

            wchar_t juryCountStr[10];
            _itow_s(currentJury, juryCountStr, 10);

            if (!WritePrivateProfileString(L"Settings", L"JuryCount", juryCountStr, INI_FILE_PATH))
            {
                MessageBox(NULL, L"Failed to save jury count.", L"Error", MB_OK | MB_ICONERROR);
            }

            EndDialog(hDlg, IDOK);
            InvalidateRect(hMainWnd, NULL, TRUE);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}
