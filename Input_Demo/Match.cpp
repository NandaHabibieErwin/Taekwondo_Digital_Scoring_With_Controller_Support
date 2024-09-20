#include "Match.h"
#include "Helper.h"
#include <tchar.h>
#include "resource.h"
#include "Main.h"

int Round = 1;
bool WinnerDecided = false;
int CurrentRound = 1;
HFONT hResultFont;



void Match::ShowResult(HDC hdc, int halfWidth, int windowHeight) {
    static bool fontInitialized = false;
    static HFONT hResultFont = nullptr; // Static to persist across calls

    // Only create the font once
    if (!fontInitialized) {
        int fontSize = windowHeight / 20;
        hResultFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
        fontInitialized = true;
    }

    // If the font was successfully created, select it into the HDC
    if (hResultFont != nullptr) {
        SelectObject(hdc, hResultFont);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 0));

    int textYOffset = windowHeight / 10;

    if (AWinner) {
        TextOut(hdc, (halfWidth / 2) - 50, textYOffset, L"Winner", 6);
    }
    else if (BWinner) {
        TextOut(hdc, halfWidth + (halfWidth / 2) - 50, textYOffset, L"Winner", 6);
    }
    else if (Tie) {
        TextOut(hdc, (halfWidth)-25, textYOffset, L"Tie", 3);
    }
}

void Match::CheckWinner() {
    // If winner has already been checked, return early
    if (WinnerDecided) {
        return; // Early exit if the winner is already determined
    }

    if (scoreA > scoreB) {
        AWinner = true;
        BWinner = false;
        Tie = false;
    }
    else if (scoreB > scoreA) {
        BWinner = true;
        AWinner = false;
        Tie = false;
    }
    else {
        Tie = true;
        AWinner = false;
        BWinner = false;
    }

    // Set the winnerChecked flag to true to prevent future calls
    WinnerDecided = true;
}



// Function to display the timer on the screen
void DisplayTimer(HDC hdc, int countdown, int windowWidth, int timerYOffset, int fontSize) {
    // Create fonts for the timer text
    HFONT hFont = CreateFont(fontSize * 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, L"Segoe UI");

    if (hFont == NULL) {
        MessageBox(NULL, L"Font creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);

    int minutes = countdown / 60;
    int seconds = countdown % 60;

    wchar_t timerText[10];
    wsprintf(timerText, L"%02d:%02d", minutes, seconds);

    // Measure the size of the timer text
    SIZE timerSize;
    GetTextExtentPoint32(hdc, timerText, lstrlen(timerText), &timerSize);

    // Calculate the position to center the timer
    int timerX = (windowWidth - timerSize.cx) / 2;
    int timerY = timerYOffset;

    // Set up the yellow background for the timer
    HBRUSH hYellowBrush = CreateSolidBrush(RGB(255, 255, 0));
    RECT timerRect = { timerX - 10, timerY - 5, timerX + timerSize.cx + 10, timerY + timerSize.cy + 5 };
    FillRect(hdc, &timerRect, hYellowBrush);

    // Draw the timer text
    SetTextColor(hdc, RGB(0, 0, 0)); // Black text for timer
    TextOut(hdc, timerX, timerY, timerText, lstrlen(timerText));

    // Clean up
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    DeleteObject(hYellowBrush);
}

void DisplayRound(HDC hdc, int windowWidth, int roundYOffset, int fontSize, int currentRound, int Round) {
    // Create font for the round text
    HFONT hFont = CreateFont(fontSize * 1.5, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, L"Segoe UI");

    if (hFont == NULL) {
        MessageBox(NULL, L"Font creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SetBkMode(hdc, TRANSPARENT);
    
    wchar_t roundText[20];
    if (CurrentMatchState == Match::MATCH_REST) {
        wsprintf(roundText, L"Rest Time");
    }
    else if (CurrentMatchState == Match::MATCH_OVER) {
        wsprintf(roundText, L"Match Over");
    }
    else {

    
    wsprintf(roundText, L"Round %d/%d", currentRound, Round);
    }
    // Measure the size of the round text
    SIZE roundSize;
    GetTextExtentPoint32(hdc, roundText, lstrlen(roundText), &roundSize);

    // Calculate the position to center the round text
    int roundX = (windowWidth - roundSize.cx) / 2;
    int roundY = roundYOffset;

    // Draw the round text
    SetTextColor(hdc, RGB(255, 255, 255)); // White text
    TextOut(hdc, roundX, roundY, roundText, lstrlen(roundText));

    // Clean up
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

        HWND hComboBox = GetDlgItem(hDlg, IDC_MATCH_ROUND);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Round");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"2 Round");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 Round");
        SendMessage(hComboBox, CB_SETCURSEL, (Round <= 3 ? (Round - 1) : 0), 0);
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

                int SelectedRound = (int)SendMessage(GetDlgItem(hDlg, IDC_MATCH_ROUND), CB_GETCURSEL, 0, 0);
                Round = SelectedRound + 1;

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
                Round_Timer = countdown;
                if (CurrentRound == Round) {
                    DisableButton(hNextRound);
                }
                else {
                    EnableButton(hNextRound);
                }
                CurrentMatchState = Match::MATCH_IDLE;
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