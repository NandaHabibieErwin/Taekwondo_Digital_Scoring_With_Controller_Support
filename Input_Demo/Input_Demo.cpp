// Input_Demo.cpp : Defines the entry point for the application.
//

#include <windowsx.h>
#include <unordered_map>
#include <windows.h>
#include <hidsdi.h>
#include <mmsystem.h>
#include <iostream>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Shlwapi.lib")

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


HANDLE jury1Device = NULL;
HANDLE jury2Device = NULL;
HANDLE jury3Device = NULL;


bool jury1KeyPressed = false;
bool jury2KeyPressed = false;
bool jury3KeyPressed = false;

int requiredKeyPresses = 1;

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
int selectedJury;
static bool timerRunning = false;




int countdown = 0;

HWND hTimerInput;
HWND hStartTimer;
HWND hResetScore;
HWND hFinish;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SetupMatch(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    JuryConfig(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TestKeyboardProc(HWND, UINT, WPARAM, LPARAM);
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
            if (timerRunning)
            {
                // No need for additional timer handling if WM_TIMER is handled correctly in WndProc
            }
        }
    }


    return (int)msg.wParam;
}

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
    if (currentJury < 1 || currentJury > 4) {
        currentJury = 1;
        MessageBox(NULL, L"Invalid jury count found, defaulting to 1.", L"Warning", MB_OK | MB_ICONWARNING);
    }


    if (GetPrivateProfileString(L"Settings", L"PlayerA", L"Player A", playerAName, 50, INI_FILE_PATH) == 0) {
        MessageBox(NULL, L"Failed to load Player A's name. Using default value.", L"Warning", MB_OK | MB_ICONWARNING);
        wcscpy_s(playerAName, L"Player A");
    }

    if (GetPrivateProfileString(L"Settings", L"PlayerB", L"Player B", playerBName, 50, INI_FILE_PATH) == 0) {
        MessageBox(NULL, L"Failed to load Player B's name. Using default value.", L"Warning", MB_OK | MB_ICONWARNING);
        wcscpy_s(playerBName, L"Player B");
    }


    wchar_t timerValue[10];
    GetPrivateProfileString(L"Settings", L"Timer[Second]", L"0", timerValue, 10, INI_FILE_PATH);
    countdown = _wtoi(timerValue);
    if (countdown < 0) {
        countdown = 0;
        MessageBox(NULL, L"Invalid timer value found, defaulting to 0.", L"Warning", MB_OK | MB_ICONWARNING);
    }
}

void HandleInput(HWND hWnd, RAWKEYBOARD& rawKB, HANDLE hDevice,
    std::unordered_map<HANDLE, bool>& numpadState, int& score,
    int points, bool& scoreAdded)
{
    if (rawKB.Message == WM_KEYDOWN) {
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


    if (pressedCount >= currentJury && !scoreAdded) {
        score += points;
        PlaySound(TEXT("sound/scorehit.wav"), NULL, SND_FILENAME | SND_ASYNC);
        scoreAdded = true;
        UpdateDisplay(hWnd);
    }


    if (pressedCount == 0) {
        scoreAdded = false;
    }
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

bool FileExists(const wchar_t* filePath) {
    DWORD fileAttr = GetFileAttributes(filePath);
    return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));
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

    if (!FileExists(INI_FILE_PATH)) {
        MessageBox(NULL, L"Settings file not found, using default settings.", L"Warning", MB_OK | MB_ICONWARNING);
    }
    else {
        LoadSettings();
    }

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

    hFinish = CreateWindowEx(0, L"BUTTON", L"Finish", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        230, 600, 100, 50, hWnd, (HMENU)FINISH, hInstance, NULL);

    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    PathRemoveFileSpec(exePath);
    SetCurrentDirectory(exePath);

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
    InvalidateRect(hWnd, NULL, TRUE);
    UpdateWindow(hWnd);               
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

    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    FillRect(hdc, &redSide, redBrush);
    DeleteObject(redBrush);


    HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
    FillRect(hdc, &blueSide, blueBrush);
    DeleteObject(blueBrush);
}

void DisplayPlayerInfo(HDC hdc, int scoreA, int scoreB, LPCWSTR playerAName, LPCWSTR playerBName, int halfWidth, int windowHeight, int fontSize, int scoreYOffset) {

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
    TextOut(hdc, halfWidth / 4, scoreYOffset, playerAName, lstrlen(playerAName));
    wchar_t scoreTextA[10];
    wsprintf(scoreTextA, L"%d", scoreA);
    TextOut(hdc, halfWidth / 4, scoreYOffset + fontSize, scoreTextA, lstrlen(scoreTextA));


    SetTextColor(hdc, RGB(255, 255, 255)); 
    TextOut(hdc, halfWidth + halfWidth / 4, scoreYOffset, playerBName, lstrlen(playerBName));
    wchar_t scoreTextB[10];
    wsprintf(scoreTextB, L"%d", scoreB);
    TextOut(hdc, halfWidth + halfWidth / 4, scoreYOffset + fontSize, scoreTextB, lstrlen(scoreTextB));  


    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
}

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
        }
        break;
        case ID_CONFIG_JURY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_JURYCONFIG), hWnd, JuryConfig);
            break;
        case START_TIMER:
        {
            if (!timerRunning) {

                timerRunning = true;
                SetWindowText(hStartTimer, L"Stop Timer"); 
                SetTimer(hWnd, TIMER_ID, 1000, NULL);
                UpdateDisplay(hWnd); 
            }
            else {
                timerRunning = false;
                SetWindowText(hStartTimer, L"Start Timer"); 
                KillTimer(hWnd, TIMER_ID); 
                UpdateDisplay(hWnd);
            }
        }
        break;
        case RESET_SCORE:
        {
            scoreA = 0;
            scoreB = 0;
            UpdateDisplay(hWnd);

        }
        break;
        case FINISH:
            scoreA = 0;
            scoreB = 0;
            countdown = 0;
            UpdateDisplay(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {

        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        SetWindowPos(hStartTimer, NULL, 10, height - 100, 100, 50, SWP_NOZORDER);

        SetWindowPos(hResetScore, NULL, 120, height - 100, 100, 50, SWP_NOZORDER);

        SetWindowPos(hFinish, NULL, 230, height - 100, 100, 50, SWP_NOZORDER);


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

            switch (rawKB.VKey) {
            case VK_NUMPAD1:
                HandleInput(hWnd, rawKB, hDevice, numpad1, scoreA, 1, scoreOneAdded);
                break;
            case VK_NUMPAD2:
                HandleInput(hWnd, rawKB, hDevice, numpad2, scoreA, 2, scoreTwoAdded);
                break;
            case VK_NUMPAD3:
                HandleInput(hWnd, rawKB, hDevice, numpad3, scoreA, 3, scoreThreeAdded);
                break;
            case VK_NUMPAD4:
                HandleInput(hWnd, rawKB, hDevice, numpad4, scoreA, 4, scoreFourAdded);
                break;
            case VK_NUMPAD5:
                HandleInput(hWnd, rawKB, hDevice, numpad5, scoreA, 5, scoreFiveAdded);
                break;
            case VK_NUMPAD6:
                HandleInput(hWnd, rawKB, hDevice, numpad6, scoreB, 1, scoreOneAdded);
                break;
            case VK_NUMPAD7:
                HandleInput(hWnd, rawKB, hDevice, numpad7, scoreB, 2, scoreOneAdded);
                break;
            case VK_NUMPAD8:
                HandleInput(hWnd, rawKB, hDevice, numpad8, scoreB, 3, scoreThreeAdded);
                break;
            case VK_NUMPAD9:
                HandleInput(hWnd, rawKB, hDevice, numpad9, scoreB, 4, scoreFourAdded);
                break;
            case VK_NUMPAD0:
                HandleInput(hWnd, rawKB, hDevice, numpad0, scoreB, 5, scoreFiveAdded);
                break;
            }
            if (jury1Device == NULL) {
                jury1Device = hDevice;
            }
            else if (jury2Device == NULL && hDevice != jury1Device) {
                jury2Device = hDevice;
            }
            else if (jury3Device == NULL && hDevice != jury1Device && hDevice != jury2Device) {
                jury3Device = hDevice;
            }


            if (hDevice == jury1Device) {
                jury1KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }
            else if (hDevice == jury2Device) {
                jury2KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }
            else if (hDevice == jury3Device) {
                jury3KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }

            UpdateDisplay(hWnd);
        }

        delete[] lpb;
        return 0;
    }


    case WM_TIMER:
    {
        if (wParam == TIMER_ID) {
            if (countdown > 0) {
                countdown--;
                UpdateDisplay(hWnd);
            }
            else {
                KillTimer(hWnd, TIMER_ID); 
                timerRunning = false;
                SetWindowText(hStartTimer, L"Start Timer");
                MessageBox(hWnd, L"Time's up!", L"Information", MB_OK | MB_ICONINFORMATION);
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

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int windowWidth = clientRect.right;
        int windowHeight = clientRect.bottom;

        int halfWidth = windowWidth / 2;

        RECT redSide = { 0, 0, halfWidth, windowHeight };
        RECT blueSide = { halfWidth, 0, windowWidth, windowHeight };

        PaintRedAndBlueBackground(hdc, redSide, blueSide);

        int fontSize = windowHeight / 10;
        int scoreYOffset = windowHeight / 4;

        DisplayPlayerInfo(hdc, scoreA, scoreB, playerAName, playerBName, halfWidth, windowHeight, fontSize, scoreYOffset);

        int timerYOffset = windowHeight / 2;
        DisplayTimer(hdc, countdown, windowWidth, timerYOffset, fontSize);

        // Jury indicator color change based on input and responsive positioning
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));  // Active jury color
        HBRUSH grayBrush = CreateSolidBrush(RGB(200, 200, 200)); // Inactive jury color

        // Set the size and spacing for the jury circles
        int circleRadius = 25;  // Circle radius
        int circleSpacing = 20; // Spacing between circles
        int totalWidth = (circleRadius * 2 * 3) + (circleSpacing * 2); // Total width for 3 circles and spacing

        int startX = (windowWidth - totalWidth) / 2; // Center the circles horizontally

        // Loop through the number of jurors and draw circles dynamically
        for (int i = 0; i < currentJury; ++i) {
            // Determine if the current jury is active
            HBRUSH juryBrush = grayBrush;
            if ((i == 0 && jury1KeyPressed) || (i == 1 && jury2KeyPressed) || (i == 2 && jury3KeyPressed)) {
                juryBrush = greenBrush;
            }

            // Select the appropriate brush and draw the circle
            SelectObject(hdc, juryBrush);
            Ellipse(hdc, startX + (i * (circleRadius * 2 + circleSpacing)), 10,
                startX + (i * (circleRadius * 2 + circleSpacing)) + (circleRadius * 2),
                10 + (circleRadius * 2));
        }

        // Cleanup
        DeleteObject(greenBrush);
        DeleteObject(grayBrush);

        EndPaint(hWnd, &ps);
    }
    break;




    case WM_DESTROY:
        SaveSettings();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK SetupMatch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hMainWnd;

    switch (message)
    {
    case WM_INITDIALOG:
        hMainWnd = (HWND)lParam;
        if (playerAName != NULL && playerAName[0] != L'\0') {
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PLAYER_A), playerAName);
        }

        if (playerBName != NULL && playerBName[0] != L'\0') {
            SetWindowText(GetDlgItem(hDlg, IDC_EDIT_PLAYER_B), playerBName);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {

            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_A, playerAName, 50);
            GetDlgItemText(hDlg, IDC_EDIT_PLAYER_B, playerBName, 50);

            wchar_t minutesValue[10], secondsValue[10];
            GetDlgItemText(hDlg, IDC_EDIT_MINUTES, minutesValue, 10);
            GetDlgItemText(hDlg, IDC_EDIT_SECONDS, secondsValue, 10);

            int minutes = _wtoi(minutesValue);
            int seconds = _wtoi(secondsValue);

            if (minutes < 0 || seconds < 0 || seconds >= 60) {
                MessageBox(hDlg, L"Invalid timer", L"Error", MB_OK | MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            countdown = (minutes * 60) + seconds;  
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



INT_PTR CALLBACK JuryConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {

        selectedJury = currentJury - 1;

        HWND hComboBox = GetDlgItem(hDlg, IDC_JURY_NUMBER);
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"1 Jury");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"2 Juries");
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)L"3 Juries");
        SendMessage(hComboBox, CB_SETCURSEL, selectedJury, 0);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_TEST)
        {

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


INT_PTR CALLBACK TestKeyboardProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Register raw input for detecting key presses
        RAWINPUTDEVICE rid[1];
        rid[0].usUsagePage = 0x01;  // Generic desktop controls
        rid[0].usUsage = 0x06;      // Keyboard
        rid[0].dwFlags = RIDEV_INPUTSINK; // Capture input even if the window is not in focus
        rid[0].hwndTarget = hDlg;   // Target the dialog box

        RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
        return (INT_PTR)TRUE;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);

        // Draw green circles for active jurors, gray for inactive
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
        HBRUSH grayBrush = CreateSolidBrush(RGB(200, 200, 200));

        // Jury 1
        HBRUSH jury1Brush = jury1KeyPressed ? greenBrush : grayBrush;
        SelectObject(hdc, jury1Brush);
        Ellipse(hdc, 50, 50, 100, 100);

        // Jury 2
        HBRUSH jury2Brush = jury2KeyPressed ? greenBrush : grayBrush;
        SelectObject(hdc, jury2Brush);
        Ellipse(hdc, 50, 110, 100, 160);

        // Jury 3
        HBRUSH jury3Brush = jury3KeyPressed ? greenBrush : grayBrush;
        SelectObject(hdc, jury3Brush);
        Ellipse(hdc, 50, 170, 100, 220);

        // Cleanup
        DeleteObject(greenBrush);
        DeleteObject(grayBrush);

        EndPaint(hDlg, &ps);
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


            if (jury1Device == NULL) {
                jury1Device = hDevice;
            }
            else if (jury2Device == NULL && hDevice != jury1Device) {
                jury2Device = hDevice;
            }
            else if (jury3Device == NULL && hDevice != jury1Device && hDevice != jury2Device) {
                jury3Device = hDevice;
            }


            if (hDevice == jury1Device) {
                jury1KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;  
            }
            else if (hDevice == jury2Device) {
                jury2KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }
            else if (hDevice == jury3Device) {
                jury3KeyPressed = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }

            InvalidateRect(hDlg, NULL, TRUE);
        }

        delete[] lpb;
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
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