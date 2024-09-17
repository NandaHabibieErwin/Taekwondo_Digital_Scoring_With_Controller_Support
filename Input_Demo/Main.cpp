// Input_Demo.cpp : Defines the entry point for the application.
//

#include "Main.h"
#include "framework.h"
#include "Resource.h"
#include "Jury.h"
#include "Helper.h"     // For settings loading and saving
#include "Match.h"      // For match and timer configuration dialog
#include <windowsx.h>   // For additional Windows message handling macros
#include <tchar.h>      // For Unicode string manipulation
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <unordered_map>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Define and initialize global variables
int scoreA = 0;
int scoreB = 0;
wchar_t playerAName[50] = L"Player A";
wchar_t playerBName[50] = L"Player B";
bool AWinner = false;
bool BWinner = false;
bool Tie = false;
bool timerRunning = false;
int countdown = 0;
HWND hStartTimer = nullptr;
HWND hResetScore = nullptr;
HWND hFinish = nullptr;

// Variables to manage scoring
bool giveScoreForA = false;
bool scoreOneAdded = false;
bool scoreTwoAdded = false;
bool scoreThreeAdded = false;
bool scoreFourAdded = false;
bool scoreFiveAdded = false;

void* jury1Device = nullptr;
void* jury2Device = nullptr;
void* jury3Device = nullptr;

bool jury1KeyPressed = false;
bool jury2KeyPressed = false;
bool jury3KeyPressed = false;

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




WCHAR szTitle[MAX_LOADSTRING] = L"Input Demo";
WCHAR szWindowClass[MAX_LOADSTRING] = L"InputDemoClass";
HINSTANCE hInst = nullptr;

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
                // Nothing for now
            }
        }
    }


    return (int)msg.wParam;
}

void ShowResult(HDC hdc, int halfWidth, int windowHeight) {
    int fontSize = windowHeight / 20;
    HFONT hResult = CreateFont(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");    
    SelectObject(hdc, hResult);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 0));

    int textYOffset = fontSize * 2;

    if (AWinner) {
        TextOut(hdc, (halfWidth / 2) - 50, textYOffset, L"Winner", 6);
    }
    else if (BWinner) {
        TextOut(hdc, halfWidth + (halfWidth / 2) - 50, textYOffset, L"Winner", 6);
    }
    else if (Tie) {
        TextOut(hdc, (halfWidth) - 25, textYOffset, L"Tie", 3);
    }
    DeleteObject(hResult);
    
}

void CheckWinner() {
    if (AWinner || BWinner || Tie) {
        return;
    }
    if (scoreA > scoreB) {
        AWinner = true;
    }
    else if (scoreB > scoreA) {
        BWinner = true;
    }
    else if (scoreA = scoreB) {
        Tie = true;
    }
    if (AWinner || BWinner || Tie) {
        InvalidateRect(GetActiveWindow(), NULL, TRUE);
        UpdateWindow(GetActiveWindow());
    }
}



//
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

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    RegisterMultipleKeyboards(hWnd);

    hStartTimer = CreateWindowEx(0, L"BUTTON", L"Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 600, 100, 50, hWnd, (HMENU)START_TIMER, hInstance, NULL);

    hResetScore = CreateWindowEx(0, L"BUTTON", L"Reset", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
    // Create fonts for score and name
    HFONT hScoreFont = CreateFont(fontSize * 7, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT hNameFont = CreateFont(fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Arial");

    SetBkMode(hdc, TRANSPARENT);

    SetTextColor(hdc, RGB(255, 255, 255));

    SelectObject(hdc, hScoreFont);
    wchar_t scoreTextA[10];
    wsprintf(scoreTextA, L"%d", scoreA);
    SIZE scoreSizeA;
    GetTextExtentPoint32(hdc, scoreTextA, lstrlen(scoreTextA), &scoreSizeA);
    int scoreAX = (halfWidth / 2) - (scoreSizeA.cx / 2);  
    TextOut(hdc, scoreAX, scoreYOffset, scoreTextA, lstrlen(scoreTextA));

    SelectObject(hdc, hNameFont);
    SIZE nameSizeA;
    GetTextExtentPoint32(hdc, playerAName, lstrlen(playerAName), &nameSizeA);
    int nameAX = (halfWidth / 2) - (nameSizeA.cx / 2);
    TextOut(hdc, nameAX, (scoreYOffset * 7.5) + fontSize * 2, playerAName, lstrlen(playerAName));

    SelectObject(hdc, hScoreFont);
    wchar_t scoreTextB[10];
    wsprintf(scoreTextB, L"%d", scoreB);
    SIZE scoreSizeB;
    GetTextExtentPoint32(hdc, scoreTextB, lstrlen(scoreTextB), &scoreSizeB);
    int scoreBX = halfWidth + (halfWidth / 2) - (scoreSizeB.cx / 2);
    TextOut(hdc, scoreBX, scoreYOffset, scoreTextB, lstrlen(scoreTextB));

    SelectObject(hdc, hNameFont);
    SIZE nameSizeB;
    GetTextExtentPoint32(hdc, playerBName, lstrlen(playerBName), &nameSizeB);
    int nameBX = halfWidth + (halfWidth / 2) - (nameSizeB.cx / 2); 
    TextOut(hdc, nameBX, (scoreYOffset * 7.5) + fontSize * 2, playerBName, lstrlen(playerBName));

    // Clean up fonts
    DeleteObject(hScoreFont);
    DeleteObject(hNameFont);
}


const wchar_t* DisplayCreator(const wchar_t* name) {
    return name;
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
            if (!timerRunning && countdown == 0) {
               MessageBox(hWnd, L"Timer not set yet! Set the timer in the menu first", L"Timer not set", MB_OK | MB_ICONEXCLAMATION);
            }
            else if (!timerRunning) {  
                AWinner = false;
                BWinner = false;
                Tie = false;
                timerRunning = true;
                SetWindowText(hStartTimer, L"Pause"); 
                SetTimer(hWnd, TIMER_ID, 1000, NULL);
                UpdateDisplay(hWnd); 
            }            
            else {
                timerRunning = false;
                SetWindowText(hStartTimer, L"Start"); 
                KillTimer(hWnd, TIMER_ID); 
                UpdateDisplay(hWnd);
            }
        }
        break;
        case RESET_SCORE:
        { // Workaround for the freezing bug, must reset first before opening dialog
            HMENU hMenu = GetMenu(hWnd);
            int menuCount = GetMenuItemCount(hMenu);

            for (int i = 0; i < menuCount; i++) {
                EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_ENABLED);
            }

            DrawMenuBar(hWnd);
            AWinner = false;
            BWinner = false;
            Tie = false;
            scoreA = 0;
            scoreB = 0;
            UpdateDisplay(hWnd);

        }
        break;
        case FINISH:
         // scoreA = 0;
         // scoreB = 0;
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

        int buttonWidth = 50;  // Width of each button
        int buttonHeight = 30;  // Height of each button
        int buttonSpacing = 20; // Space between buttons


        int totalButtonWidth = (buttonWidth * 3) + (buttonSpacing * 2);


        int buttonStartX = (width - totalButtonWidth) / 2;


        SetWindowPos(hStartTimer, NULL, buttonStartX, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hResetScore, NULL, buttonStartX + buttonWidth + buttonSpacing, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hFinish, NULL, buttonStartX + (buttonWidth + buttonSpacing) * 2, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);

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
                HandleInput(hWnd, rawKB, hDevice, numpad1, scoreA, 1, scoreOneAdded,true);
                break;
            case VK_NUMPAD2:
                HandleInput(hWnd, rawKB, hDevice, numpad2, scoreA, 2, scoreTwoAdded, true);
                break;
            case VK_NUMPAD3:
                HandleInput(hWnd, rawKB, hDevice, numpad3, scoreA, 3, scoreThreeAdded, true);
                break;
            case VK_NUMPAD4:
                HandleInput(hWnd, rawKB, hDevice, numpad4, scoreA, 4, scoreFourAdded, true);
                break;
            case VK_NUMPAD5:
                HandleInput(hWnd, rawKB, hDevice, numpad5, scoreA, 5, scoreFiveAdded, true);
                break;
            case VK_NUMPAD6:
                HandleInput(hWnd, rawKB, hDevice, numpad6, scoreB, 1, scoreOneAdded, false);
                break;
            case VK_NUMPAD7:
                HandleInput(hWnd, rawKB, hDevice, numpad7, scoreB, 2, scoreOneAdded, false);
                break;
            case VK_NUMPAD8:
                HandleInput(hWnd, rawKB, hDevice, numpad8, scoreB, 3, scoreThreeAdded, false);
                break;
            case VK_NUMPAD9:
                HandleInput(hWnd, rawKB, hDevice, numpad9, scoreB, 4, scoreFourAdded, false);
                break;
            case VK_NUMPAD0:
                HandleInput(hWnd, rawKB, hDevice, numpad0, scoreB, 5, scoreFiveAdded, false);
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
                HMENU hMenu = GetMenu(hWnd);
                int menuCount = GetMenuItemCount(hMenu);

                for (int i = 0; i < menuCount; i++) {
                    EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_GRAYED);
                }
                DrawMenuBar(hWnd);
                countdown--;
                UpdateDisplay(hWnd);
            }            
            else {
                KillTimer(hWnd, TIMER_ID); 
                timerRunning = false;
                SetWindowText(hStartTimer, L"Start");
                PlaySound(TEXT("sound/matchover.wav"), NULL, SND_FILENAME | SND_SYNC);          
                CheckWinner();
                UpdateDisplay(hWnd);
            //  MessageBox(hWnd, L"Time's up!", L"Information", MB_OK | MB_ICONINFORMATION);   
            }
            if (countdown <= 10) {
               PlaySound(TEXT("sound/timerunningout.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                if (countdown == 0) {
                    PlaySound(nullptr, nullptr, 0);
                }
            }
        }
    }
    break;



    case IDM_SETTIMER:
        DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTIMER), hWnd, SetTimerProc);
        break;

    case WM_ERASEBKGND:
        return true;        

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, ps.rcPaint.right, ps.rcPaint.bottom);
        HGDIOBJ hOld = SelectObject(memDC, hbmMem);

        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int windowWidth = clientRect.right;
        int windowHeight = clientRect.bottom;

        int halfWidth = windowWidth / 2;

        RECT redSide = { 0, 0, halfWidth, windowHeight };
        RECT blueSide = { halfWidth, 0, windowWidth, windowHeight };

        PaintRedAndBlueBackground(memDC, redSide, blueSide);
        

        int fontSize = windowHeight / 10;
        int scoreYOffset = windowHeight / 14;

        DisplayPlayerInfo(memDC, scoreA, scoreB, playerAName, playerBName, halfWidth, windowHeight, fontSize, scoreYOffset);

        int timerYOffset = windowHeight / 9;
        DisplayTimer(memDC, countdown, windowWidth, timerYOffset, fontSize);

        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));  // Active jury color
        HBRUSH grayBrush = CreateSolidBrush(RGB(200, 200, 200)); // Inactive jury color

        int circleRadius = 25;
        int circleSpacing = 20;
        int totalWidth = (circleRadius * 2) + circleSpacing;
        int circleDiameter = circleRadius * 2;

   /*     for (int i = 0; i < currentJury; ++i) {
            HBRUSH juryBrush = grayBrush;
            if ((i == 0 && jury1KeyPressed) || (i == 1 && jury2KeyPressed) || (i == 2 && jury3KeyPressed)) {
                juryBrush = greenBrush;
            }
            SelectObject(memDC, juryBrush);

            Ellipse(memDC, circleSpacing, 10 + (i * (circleRadius * 2 + circleSpacing)),
                circleSpacing + (circleRadius * 2), 10 + (i * (circleRadius * 2 + circleSpacing)) + (circleRadius * 2));

            Ellipse(memDC, windowWidth - circleSpacing - (circleRadius * 2), 10 + (i * (circleRadius * 2 + circleSpacing)),
                windowWidth - circleSpacing, 10 + (i * (circleRadius * 2 + circleSpacing)) + (circleRadius * 2));
        } */

        for (int i = 0; i < currentJury; i++) {
            // Left side for Player A
            HBRUSH leftJuryBrush = grayBrush;
            HBRUSH rightJuryBrush = grayBrush;

            // Determine which jury circle should be active
            if ((i == 0 && jury1KeyPressed && giveScoreForA) || (i == 1 && jury2KeyPressed && giveScoreForA) || (i == 2 && jury3KeyPressed && giveScoreForA)) {
                leftJuryBrush = greenBrush;
            }
            if ((i == 0 && jury1KeyPressed && !giveScoreForA) || (i == 1 && jury2KeyPressed && !giveScoreForA) || (i == 2 && jury3KeyPressed && !giveScoreForA)) {
                rightJuryBrush = greenBrush;
            }

            // Draw circles for left side (Player A)
            SelectObject(memDC, leftJuryBrush);
            Ellipse(memDC, 50, 50 + (i * (circleDiameter + circleSpacing)), 50 + circleDiameter, 50 + circleDiameter + (i * (circleDiameter + circleSpacing)));

            // Draw circles for right side (Player B)
            SelectObject(memDC, rightJuryBrush);
            Ellipse(memDC, windowWidth - circleSpacing - circleDiameter, 50 + (i * (circleDiameter + circleSpacing)), windowWidth - circleSpacing, 50 + circleDiameter + (i * (circleDiameter + circleSpacing)));
        }

        if (AWinner || BWinner || Tie) {
            ShowResult(memDC, halfWidth, windowHeight);
            UpdateDisplay(hWnd);
        }


        HFONT hCreatorFont = CreateFont(windowHeight / 30, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");

        SelectObject(memDC, hCreatorFont);
        SetBkMode(memDC, TRANSPARENT);
        SetTextColor(memDC, RGB(255, 255, 255));

        const wchar_t* theCreator = DisplayCreator(L"Created by: Nanda Habibie Erwin");

        SIZE textSize;
        GetTextExtentPoint32(memDC, theCreator, lstrlen(theCreator), &textSize);
        int creatorTextX = (windowWidth - textSize.cx) / 2;  // Center horizontally
        int creatorTextY = windowHeight - textSize.cy - 10;  // 10 pixels above the bottom of the window

        TextOut(memDC, creatorTextX, creatorTextY, theCreator, lstrlen(theCreator));


        BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, memDC, 0, 0, SRCCOPY);

        // Cleanup
        DeleteObject(hCreatorFont);
        SelectObject(memDC, hOld);
        DeleteObject(hbmMem);
        DeleteDC(memDC);
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



// Currently has an issue where opening this will make the score non functional, resolving issue
INT_PTR CALLBACK TestKeyboardProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{   
    static HWND hMainWnd;
    switch (message)
    {
    case WM_INITDIALOG:
    {
        hMainWnd = (HWND)lParam;
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

        HBRUSH leftJury1Brush = (jury1KeyPressed && giveScoreForA) ? greenBrush : grayBrush;
        SelectObject(hdc, leftJury1Brush);
        Ellipse(hdc, 50, 50, 100, 100);

        HBRUSH rightJury1Brush = (jury1KeyPressed && !giveScoreForA) ? greenBrush : grayBrush;
        SelectObject(hdc, rightJury1Brush);
        Ellipse(hdc, 150, 50, 200, 100);

        // Jury 2
        HBRUSH leftjury2Brush = jury2KeyPressed && giveScoreForA ? greenBrush : grayBrush;
        SelectObject(hdc, leftjury2Brush);
        Ellipse(hdc, 50, 110, 100, 160);

        // Jury 2
        HBRUSH rightjury2Brush = jury2KeyPressed && !giveScoreForA ? greenBrush : grayBrush;
        SelectObject(hdc, rightjury2Brush);
        Ellipse(hdc, 50, 110, 100, 160);

        // Jury 3
        HBRUSH leftjury3Brush = jury3KeyPressed && giveScoreForA ? greenBrush : grayBrush;
        SelectObject(hdc, leftjury3Brush);
        Ellipse(hdc, 50, 170, 100, 220);

        HBRUSH rightjury3Brush = jury3KeyPressed && !giveScoreForA ? greenBrush : grayBrush;
        SelectObject(hdc, rightjury3Brush);
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
            RAWINPUTDEVICE rid[1];
            rid[0].usUsagePage = 0x01;
            rid[0].usUsage = 0x06;
            rid[0].dwFlags = RIDEV_REMOVE;
            rid[0].hwndTarget = NULL;

            RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

            RegisterMultipleKeyboards(hMainWnd);            
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