#include "Match.h"
#include "Helper.h"
#include <tchar.h>
#include "resource.h"

// Function to display the timer on the screen
void DisplayTimer(HDC hdc, int countdown, int windowWidth, int timerYOffset, int fontSize) {
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Arial");

    if (hFont == NULL) {
        MessageBox(NULL, L"Font creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    int minutes = countdown / 60;
    int seconds = countdown % 60;

    wchar_t timerText[10];
    wsprintf(timerText, L"%02d:%02d", minutes, seconds);
    TextOut(hdc, windowWidth / 2 - fontSize, timerYOffset, timerText, lstrlen(timerText));

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

// Function to load the setup match dialog settings in a separate thread
DWORD WINAPI SetupMatchThread(LPVOID lpParam) {
    HWND hDlg = (HWND)lpParam;

    // Load player names from global variables
    if (playerAName != NULL && playerAName[0] != L'\0') {
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PLAYER_A), playerAName);
    }

    if (playerBName != NULL && playerBName[0] != L'\0') {
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PLAYER_B), playerBName);
    }

    try {
        int minutes = countdown / 60;
        int seconds = countdown % 60;
        wchar_t minutesStr[10], secondsStr[10];
        _itow_s(minutes, minutesStr, 10);
        _itow_s(seconds, secondsStr, 10);
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_MINUTES), minutesStr);
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_SECONDS), secondsStr);
    }
    catch (...) {
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_MINUTES), L"0");
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT_SECONDS), L"0");
    }

    return 0;
}

// Setup match dialog procedure
INT_PTR CALLBACK SetupMatch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hMainWnd;
    switch (message) {
    case WM_INITDIALOG:
        hMainWnd = (HWND)lParam;

        // Create a new thread to load dialog settings asynchronously
        CreateThread(NULL, 0, SetupMatchThread, (LPVOID)hDlg, 0, NULL);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t validatePlayerAName[50];
            wchar_t validatePlayerBName[50];

            // Retrieve player names
            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_A, validatePlayerAName, 50);
            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_B, validatePlayerBName, 50);

            // Validate player name lengths
            if (wcslen(validatePlayerAName) >= 18) {
                MessageBox(hDlg, L"Player A name exceeded limit of 18 characters", L"Exceeded Limit", MB_OK | MB_ICONEXCLAMATION);
            }
            else if (wcslen(validatePlayerBName) >= 18) {
                MessageBox(hDlg, L"Player B name exceeded limit of 18 characters", L"Exceeded Limit", MB_OK | MB_ICONEXCLAMATION);
            }
            else {
                // Update global player names
                wcscpy_s(playerAName, validatePlayerAName);
                wcscpy_s(playerBName, validatePlayerBName);

                // Retrieve timer values
                wchar_t minutesValue[10], secondsValue[10];
                GetDlgItemText(hDlg, IDC_EDIT_MINUTES, minutesValue, 10);
                GetDlgItemText(hDlg, IDC_EDIT_SECONDS, secondsValue, 10);

                int minutes = _wtoi(minutesValue);
                int seconds = _wtoi(secondsValue);

                // Validate timer values
                if (minutes < 0 || seconds < 0 || seconds >= 60) {
                    MessageBox(hDlg, L"Invalid timer", L"Error", MB_OK | MB_ICONERROR);
                    return (INT_PTR)TRUE;
                }

                // Save the settings and update the countdown
                SaveSettings();
                countdown = (minutes * 60) + seconds;
                EndDialog(hDlg, IDOK);

                InvalidateRect(hMainWnd, NULL, TRUE);
                return (INT_PTR)TRUE;
            }
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
