// Input_Demo.cpp : Defines the entry point for the application.
//

#include <unordered_map>
#include <windows.h>
#include <hidsdi.h>
#include "framework.h"
#include "Input_Demo.h"
#define INI_FILE_PATH L".\\config.ini"
#define MAX_LOADSTRING 100
#define SECTION_NAME L"Settings"
#define KEY_JURY_COUNT L"JuryCount"

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Score Variable
int scoreA = 0;
int scoreB = 0;

wchar_t playerAName[50] = L"Player A";
wchar_t playerBName[50] = L"Player B";

std::unordered_map<HANDLE, bool> numpad0;
std::unordered_map<HANDLE, bool> numpad1;
std::unordered_map<HANDLE, bool> numpad2;
std::unordered_map<HANDLE, bool> numpad3;
std::unordered_map<HANDLE, bool> numpad4;
std::unordered_map<HANDLE, bool> numpad5;
std::unordered_map<HANDLE, bool> numpad6;
std::unordered_map<HANDLE, bool> numpad7;
std::unordered_map<HANDLE, bool> numpad8;
std::unordered_map<HANDLE, bool> numpad9;
std::unordered_map<HANDLE, bool> numpadDec;
std::unordered_map<HANDLE, bool> numpadDiv;
std::unordered_map<HANDLE, bool> numpadMulti;
std::unordered_map<HANDLE, bool> numpadSub;
std::unordered_map<HANDLE, bool> numpadAdd;

std::unordered_map<HANDLE, bool> numpad;
std::unordered_map<HANDLE, bool> addedScoreA;
std::unordered_map<HANDLE, bool> addedScoreB;

bool scoreOneAdded = false;
bool scoreTwoAdded = false;
bool scoreThreeAdded = false;
bool scoreFourAdded = false;
bool scoreFiveAdded = false;

int currentJury;
static bool timerRunning = false;




int countdown = 0;

HWND hTimerInput;
HWND hStartTimer;
HWND hResetScore;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SetupMatch(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    JuryConfig(HWND, UINT, WPARAM, LPARAM);
void PaintRedAndBlueBackground(HDC hdc, RECT redSide, RECT blueSide);
void DisplayPlayerInfo(HDC hdc, int scoreA, int scoreB, LPCWSTR playerAName, LPCWSTR playerBName);
void DisplayTimer(HDC hdc, int countdown);
void UpdateDisplay(HWND hWnd);
void StartTimer(HWND hWnd);
void ResetScore(HWND hWnd);
void HandleScoreInput(HWND hWnd, RAWKEYBOARD rawKB, HANDLE hDevice);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INPUTDEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INPUTDEMO));

    MSG msg;

    // Main message loop:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // Handle timer processing directly
            if (timerRunning)
            {
                // This block will be called when the window is idle
                // No need for additional timer handling if WM_TIMER is handled correctly in WndProc
            }
        }
    }


    return (int)msg.wParam;
}


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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INPUTDEMO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_INPUTDEMO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);

}



//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    
    int juryCount = GetPrivateProfileInt(SECTION_NAME, KEY_JURY_COUNT, 1, INI_FILE_PATH);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    RegisterMultipleKeyboards(hWnd);

    hStartTimer = CreateWindowEx(0, L"BUTTON", L"Start Timer", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 600, 100, 50, hWnd, (HMENU)START_TIMER, hInstance, NULL);

    hResetScore = CreateWindowEx(0, L"BUTTON", L"Reset Score", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, 600, 100, 50, hWnd, (HMENU)RESET_SCORE, hInstance, NULL);



    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

void UpdateDisplay(HWND hWnd) {
    InvalidateRect(hWnd, NULL, TRUE); // Trigger a repaint of the entire window
    UpdateWindow(hWnd);               // Force immediate repaint
}

void showScore(HDC hdc) {
    wchar_t scoreTextA[50];
    wchar_t scoreTextB[50];
    wsprintf(scoreTextA, L"A: %d", scoreA);
    wsprintf(scoreTextB, L"B: %d", scoreB);

    TextOut(hdc, 10, 10, scoreTextA, lstrlen(scoreTextA));
    TextOut(hdc, 10, 30, scoreTextB, lstrlen(scoreTextB));
}

void PaintRedAndBlueBackground(HDC hdc, RECT redSide, RECT blueSide) {
    // Red background for player A
    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    FillRect(hdc, &redSide, redBrush);
    DeleteObject(redBrush);

    // Blue background for player B
    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
    FillRect(hdc, &blueSide, blueBrush);
    DeleteObject(blueBrush);
}

void DisplayPlayerInfo(HDC hdc, int scoreA, int scoreB, LPCWSTR playerAName, LPCWSTR playerBName, int halfWidth, int windowHeight, int fontSize, int scoreYOffset) {
    // Create the font
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Arial");

    // Check if font creation was successful
    if (hFont == NULL) {
        MessageBox(NULL, L"Font creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Select the font into the HDC
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    // Set text background to transparent
    SetBkMode(hdc, TRANSPARENT);

    // Red player info (Player A)
    SetTextColor(hdc, RGB(255, 255, 255));  // White text
    TextOut(hdc, halfWidth / 4, scoreYOffset, playerAName, lstrlen(playerAName));  // Player A name
    wchar_t scoreTextA[10];
    wsprintf(scoreTextA, L"%d", scoreA);
    TextOut(hdc, halfWidth / 4, scoreYOffset + fontSize, scoreTextA, lstrlen(scoreTextA));  // Player A score

    // Blue player info (Player B)
    SetTextColor(hdc, RGB(255, 255, 255));  // White text
    TextOut(hdc, halfWidth + halfWidth / 4, scoreYOffset, playerBName, lstrlen(playerBName));  // Player B name
    wchar_t scoreTextB[10];
    wsprintf(scoreTextB, L"%d", scoreB);
    TextOut(hdc, halfWidth + halfWidth / 4, scoreYOffset + fontSize, scoreTextB, lstrlen(scoreTextB));  // Player B score

    // Restore the original font and delete the new font
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

void DisplayTimer(HDC hdc, int countdown, int windowWidth, int timerYOffset, int fontSize) {
    HFONT hFont = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Arial");

    // Check if font creation was successful
    if (hFont == NULL) {
        MessageBox(NULL, L"Font creation failed!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    SetBkMode(hdc, TRANSPARENT);  // Ensure no background fill
    SetTextColor(hdc, RGB(255, 255, 255));  // White text for timer

    // Convert countdown seconds into MM:SS format
    int minutes = countdown / 60;
    int seconds = countdown % 60;

    wchar_t timerText[10];
    wsprintf(timerText, L"%02d:%02d", minutes, seconds);  // MM:SS format
    TextOut(hdc, windowWidth / 2 - fontSize, timerYOffset, timerText, lstrlen(timerText));  // Center the timer

    SelectObject(hdc, oldFont);  // Restore original font
    DeleteObject(hFont);  // Clean up the font
}

void showTimer(HDC hdc, int countdown) {
    wchar_t timerText[50];
    int minutes = countdown / 60;
    int seconds = countdown % 60;
    wsprintf(timerText, L"Time: %02d:%02d", minutes, seconds);

    TextOut(hdc, 10, 50, timerText, lstrlen(timerText));
}


INT_PTR CALLBACK SetTimerProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            wchar_t timerValue[10];
            GetDlgItemText(hDlg, IDC_EDIT_TIMER, timerValue, 10);
            countdown = _wtoi(timerValue);
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case ID_MATCH_SETUP:
        {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SETUPMATCH), hWnd, SetupMatch);

            // After the dialog, set up the timer if a valid countdown value was entered
            if (countdown > 0 && !timerRunning) {
                timerRunning = true;
                SetTimer(hWnd, TIMER_ID, 1000, NULL);  // Start the timer with 1 second interval
            }
        }
        break;
        case ID_CONFIG_JURY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_JURYCONFIG), hWnd, JuryConfig);
        break;
        case START_TIMER:
        {
            wchar_t timerValue[10];
            GetWindowText(hTimerInput, timerValue, 10);
            countdown = _wtoi(timerValue);
            if (countdown > 0 && !timerRunning) {
                timerRunning = true;
                SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
            }
           // MessageBox(hWnd, L"Timer have been set.", L"Run", MB_OK | MB_ICONINFORMATION);
            UpdateDisplay(hWnd); // Force immediate repaint after starting the timer
        }
        break;
        case RESET_SCORE:
        {
            scoreA = 0;
            scoreB = 0;
            UpdateDisplay(hWnd); // Force immediate repaint after starting the timer
          //  MessageBox(hWnd, L"Score have been reset.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        // Get the new window dimensions
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        // Reposition the Start Timer button at the bottom-left
        SetWindowPos(hStartTimer, NULL, 10, height - 100, 100, 50, SWP_NOZORDER);

        // Reposition the Reset Score button next to the Start Timer button
        SetWindowPos(hResetScore, NULL, 120, height - 100, 100, 50, SWP_NOZORDER);

        // Ensure the window is repainted correctly
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
    }
    break;
    case WM_INPUT:
    {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

        LPBYTE lpb = new BYTE[dwSize];
        if (lpb == NULL) {
            return 0;
        }

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
            delete[] lpb;
            return 0;
        }

        RAWINPUT* raw = (RAWINPUT*)lpb;
        if (raw->header.dwType == RIM_TYPEKEYBOARD) {
            RAWKEYBOARD rawKB = raw->data.keyboard;
            HANDLE hDevice = raw->header.hDevice;


            if (rawKB.VKey == VK_NUMPAD1) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad1[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad1[hDevice] = false;
                }

    
                int numpad1PressedCount = 0;
                for (const auto& state : numpad1) {
                    if (state.second) {
                        numpad1PressedCount++;
                    }
                }

                
                if (numpad1PressedCount >= 2 && !scoreOneAdded) {
                    scoreA++;  
                    scoreOneAdded = true;  
                    UpdateDisplay(hWnd); 
                }

                if (numpad1PressedCount == 0) {
                    scoreOneAdded = false;
                }
            }


            if (rawKB.VKey == VK_NUMPAD2) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad2[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad2[hDevice] = false;
                }


                int numpad2PressedCount = 0;
                for (const auto& state : numpad2) {
                    if (state.second) {
                        numpad2PressedCount++;
                    }
                }

                
                if (numpad2PressedCount >= 2 && !scoreTwoAdded) {
                    scoreA += 2;
                    scoreTwoAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad2PressedCount == 0) {
                    scoreTwoAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD3) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad3[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad3[hDevice] = false;
                }


                int numpad3PressedCount = 0;
                for (const auto& state : numpad3) {
                    if (state.second) {
                        numpad3PressedCount++;
                    }
                }


                if (numpad3PressedCount >= 2 && !scoreThreeAdded) {
                    scoreA += 3;
                    scoreThreeAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad3PressedCount == 0) {
                    scoreThreeAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD4) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad4[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad4[hDevice] = false;
                }


                int numpad4PressedCount = 0;
                for (const auto& state : numpad4) {
                    if (state.second) {
                        numpad4PressedCount++;
                    }
                }


                if (numpad4PressedCount >= 2 && !scoreFourAdded) {
                    scoreA += 4;
                    scoreFourAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad4PressedCount == 0) {
                    scoreFourAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD5) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad5[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad5[hDevice] = false;
                }


                int numpad5PressedCount = 0;
                for (const auto& state : numpad5) {
                    if (state.second) {
                        numpad5PressedCount++;
                    }
                }


                if (numpad5PressedCount >= 2 && !scoreFiveAdded) {
                    scoreA += 5;
                    scoreFiveAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad5PressedCount == 0) {
                    scoreFiveAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD6) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad6[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad6[hDevice] = false;
                }


                int numpad6PressedCount = 0;
                for (const auto& state : numpad6) {
                    if (state.second) {
                        numpad6PressedCount++;
                    }
                }


                if (numpad6PressedCount >= 2 && !scoreOneAdded) {
                    scoreB++;
                    scoreOneAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad6PressedCount == 0) {
                    scoreOneAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD7) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad7[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad7[hDevice] = false;
                }


                int numpad7PressedCount = 0;
                for (const auto& state : numpad7) {
                    if (state.second) {
                        numpad7PressedCount++;
                    }
                }


                if (numpad7PressedCount >= 2 && !scoreOneAdded) {
                    scoreB += 2;
                    scoreOneAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad7PressedCount == 0) {
                    scoreOneAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD8) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad8[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad8[hDevice] = false;
                }


                int numpad8PressedCount = 0;
                for (const auto& state : numpad8) {
                    if (state.second) {
                        numpad8PressedCount++;
                    }
                }


                if (numpad8PressedCount >= 2 && !scoreThreeAdded) {
                    scoreB += 3;
                    scoreThreeAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad8PressedCount == 0) {
                    scoreThreeAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD9) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad9[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad9[hDevice] = false;
                }


                int numpad9PressedCount = 0;
                for (const auto& state : numpad9) {
                    if (state.second) {
                        numpad9PressedCount++;
                    }
                }


                if (numpad9PressedCount >= 2 && !scoreFourAdded) {
                    scoreB += 4;
                    scoreFourAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad9PressedCount == 0) {
                    scoreFourAdded = false;
                }
            }

            if (rawKB.VKey == VK_NUMPAD0) {
                if (rawKB.Message == WM_KEYDOWN) {
                    numpad0[hDevice] = true;
                }
                else if (rawKB.Message == WM_KEYUP) {
                    numpad0[hDevice] = false;
                }


                int numpad0PressedCount = 0;
                for (const auto& state : numpad0) {
                    if (state.second) {
                        numpad0PressedCount++;
                    }
                }


                if (numpad0PressedCount >= 2 && !scoreFiveAdded) {
                    scoreB += 5;
                    scoreFiveAdded = true;
                    UpdateDisplay(hWnd);
                }


                if (numpad0PressedCount == 0) {
                    scoreFiveAdded = false;
                }
            }

        }

        delete[] lpb;
        return 0;
    }

    case WM_TIMER:
    {
        if (wParam == TIMER_ID) {
            if (countdown > 0) {
                countdown--;
                UpdateDisplay(hWnd);  // Repaint to show updated timer
            }
            else {
                KillTimer(hWnd, TIMER_ID); // Stop the timer when it reaches 0
                timerRunning = false;
            }
        }
    }
    break;


    case IDM_SETTIMER:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTIMER), hWnd, SetTimerProc);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Get the current window size
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int windowWidth = clientRect.right;
        int windowHeight = clientRect.bottom;

        // Calculate half-width for the red and blue sections
        int halfWidth = windowWidth / 2;

        // Define red and blue sections
        RECT redSide = { 0, 0, halfWidth, windowHeight };
        RECT blueSide = { halfWidth, 0, windowWidth, windowHeight };

        // Paint red and blue backgrounds
        PaintRedAndBlueBackground(hdc, redSide, blueSide);

        // Calculate and display player information and score based on window size
        int fontSize = windowHeight / 10;  // Font size scales with window height
        int scoreYOffset = windowHeight / 4;  // Y offset for score display

        // Use the global variables playerAName and playerBName here
        DisplayPlayerInfo(hdc, scoreA, scoreB, playerAName, playerBName, halfWidth, windowHeight, fontSize, scoreYOffset);

        // Display Countdown Timer at the center of the window
        int timerYOffset = windowHeight / 2;
        DisplayTimer(hdc, countdown, windowWidth, timerYOffset, fontSize);

        EndPaint(hWnd, &ps);
    }
    break;



    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK SetupMatch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hMainWnd;  // Store the main window handle

    switch (message)
    {
    case WM_INITDIALOG:
        // Retrieve the main window handle passed through lParam
        hMainWnd = (HWND)lParam;
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            // Get player names
            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_A, playerAName, 50);
            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_B, playerBName, 50);

            // Get minutes and seconds from the dialog's edit controls
            wchar_t minutesValue[10], secondsValue[10];
            GetDlgItemText(hDlg, IDC_EDIT_MINUTES, minutesValue, 10);
            GetDlgItemText(hDlg, IDC_EDIT_SECONDS, secondsValue, 10);

            int minutes = _wtoi(minutesValue);
            int seconds = _wtoi(secondsValue);

            if (minutes < 0 || seconds < 0 || seconds >= 60) {
                MessageBox(hDlg, L"Please enter valid minutes and seconds (seconds should be 0-59).", L"Error", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            countdown = (minutes * 60) + seconds;
            EndDialog(hDlg, IDOK); // Close dialog

            // Now call UpdateDisplay using the main window handle
            InvalidateRect(hMainWnd, NULL, TRUE);  // Forces a repaint
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

INT_PTR CALLBACK JuryConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hComboBox = GetDlgItem(hDlg, IDC_JURY_NUMBER);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Jury");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"2 Jurors");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 Jurors");
        SendMessage(hComboBox, CB_SETCURSEL, 0, 0); // Default selection
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            HWND hComboBox = GetDlgItem(hDlg, IDC_JURY_NUMBER);
            int selIndex = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);

            // Convert the selected index to a string
            wchar_t juryCountStr[2];
            _itow_s(selIndex + 1, juryCountStr, 10);

            // Save to the INI file
            WritePrivateProfileString(L"Settings", L"JuryCount", juryCountStr, INI_FILE_PATH);

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) 
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

