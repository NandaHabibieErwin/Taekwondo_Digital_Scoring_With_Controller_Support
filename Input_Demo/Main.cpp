
// Input_Demo.cpp : Defines the entry point for the application.
//

#include "Main.h"
#include "framework.h"
#include "Resource.h"
#include "Jury.h"
#include "Match.h"
#include "Helper.h"     // For settings loading and saving
#include "Match.h"      // For match and timer configuration dialog
#include <windowsx.h>   // For additional Windows message handling macros
#include <tchar.h>      // For Unicode string manipulation
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <unordered_map>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")

    // Define and initialize global variables
int scoreA = 0;
int scoreB = 0;
wchar_t playerAName[50] = L"Player A";
wchar_t playerBName[50] = L"Player B";
int PlayerATotalScore = 0;
int PlayerBTotalScore = 0;

bool AWinner = false;
bool BWinner = false;
bool Tie = false;
bool timerRunning = false;
int countdown = 0;
int Round_Timer = 120;
HWND hStartTimer = nullptr;
HWND hResetScore = nullptr;
HWND hFinish = nullptr;
HWND hNextRound = nullptr;

// Variables to manage scoring
bool giveScoreForA = false;
bool scoreOneAdded = false;
bool scoreTwoAdded = false;
bool scoreThreeAdded = false;
bool scoreFourAdded = false;
bool scoreFiveAdded = false;
bool scoreOneReduced = false;

void* jury1Device = nullptr;
void* jury2Device = nullptr;
void* jury3Device = nullptr;

bool jury1KeyPressed = false;
bool jury2KeyPressed = false;
bool jury3KeyPressed = false;

HFONT hButtonFont = NULL;



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

Match match;

std::unordered_map<HANDLE, bool> numpad;
std::unordered_map<HANDLE, bool> addedScoreA;
std::unordered_map<HANDLE, bool> addedScoreB;

// Convert wide string to UTF-8 string
std::string WideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()),
        NULL, 0, NULL, NULL
    );
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0], static_cast<int>(wstr.size()),
        &strTo[0], sizeNeeded, NULL, NULL
    );
    return strTo;
}

static std::list<std::unique_ptr<sf::Sound>> activeSounds;



void PlayAudio(const std::wstring& audio, bool loop) {
    static sf::SoundBuffer buffer;
    static bool isBufferLoaded = false;

    // Get the full path of the executable
    wchar_t moduleFileName[MAX_PATH];
    GetModuleFileNameW(NULL, moduleFileName, MAX_PATH);
    PathRemoveFileSpecW(moduleFileName);

    // Construct the path to the sound file
    std::wstring soundFilePath = moduleFileName;
    soundFilePath += L"\\sound\\" + audio;

    // Convert wide string to UTF-8 std::string
    std::string soundFilePathStr = WideStringToString(soundFilePath);

    // Load the sound buffer only once
    if (!isBufferLoaded) {
        if (!buffer.loadFromFile(soundFilePathStr)) {
            MessageBox(NULL, L"Failed to load sound file.", L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        isBufferLoaded = true;
    }

    // Clean up sounds that have finished playing
    activeSounds.remove_if([](const std::unique_ptr<sf::Sound>& sound) {
        return sound->getStatus() == sf::Sound::Stopped;
        });

    // Create a new sound object
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(buffer);
    sound->setLoop(loop);
    sound->play();

    // Add the sound to the list to keep it alive
    activeSounds.emplace_back(std::move(sound));
}


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
        Round_Timer = countdown;
    }

    hButtonFont = CreateFont(
        18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    RegisterMultipleKeyboards(hWnd);

    // Start Button
    hStartTimer = CreateWindowEx(0, L"BUTTON", L"Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 600, 100, 50, hWnd, (HMENU)START_TIMER, hInstance, NULL);
    SendMessage(hStartTimer, WM_SETFONT, (WPARAM)hButtonFont, TRUE);

    // Reset Button
    hResetScore = CreateWindowEx(0, L"BUTTON", L"Reset", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        120, 600, 100, 50, hWnd, (HMENU)RESET_SCORE, hInstance, NULL);
    SendMessage(hResetScore, WM_SETFONT, (WPARAM)hButtonFont, TRUE);

    // Finish Button
    hFinish = CreateWindowEx(0, L"BUTTON", L"Finish", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        230, 600, 100, 50, hWnd, (HMENU)FINISH, hInstance, NULL);
    SendMessage(hFinish, WM_SETFONT, (WPARAM)hButtonFont, TRUE);

    // Next Button
    hNextRound = CreateWindowEx(0, L"BUTTON", L"Next", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        340, 600, 100, 50, hWnd, (HMENU)NEXT_ROUND, hInstance, NULL);
    SendMessage(hNextRound, WM_SETFONT, (WPARAM)hButtonFont, TRUE);


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
    // Gradient fill for the red side
    TRIVERTEX vertRed[2];
    GRADIENT_RECT gRectRed;

    // Light to dark red gradient
    vertRed[0].x = redSide.left;
    vertRed[0].y = redSide.top;
    vertRed[0].Red = 0xD000; // Light red
    vertRed[0].Green = 0x2000;
    vertRed[0].Blue = 0x2000;
    vertRed[0].Alpha = 0x0000;

    vertRed[1].x = redSide.right;
    vertRed[1].y = redSide.bottom;
    vertRed[1].Red = 0x8000; // Dark red
    vertRed[1].Green = 0x0000;
    vertRed[1].Blue = 0x0000;
    vertRed[1].Alpha = 0x0000;

    gRectRed.UpperLeft = 0;
    gRectRed.LowerRight = 1;

    GradientFill(hdc, vertRed, 2, &gRectRed, 1, GRADIENT_FILL_RECT_V);

    // Gradient fill for the blue side
    TRIVERTEX vertBlue[2];
    GRADIENT_RECT gRectBlue;

    // Light to dark blue gradient
    vertBlue[0].x = blueSide.left;
    vertBlue[0].y = blueSide.top;
    vertBlue[0].Red = 0x2000;
    vertBlue[0].Green = 0x2000;
    vertBlue[0].Blue = 0xD000; // Light blue
    vertBlue[0].Alpha = 0x0000;

    vertBlue[1].x = blueSide.right;
    vertBlue[1].y = blueSide.bottom;
    vertBlue[1].Red = 0x0000;
    vertBlue[1].Green = 0x0000;
    vertBlue[1].Blue = 0x8000; // Dark blue
    vertBlue[1].Alpha = 0x0000;

    gRectBlue.UpperLeft = 0;
    gRectBlue.LowerRight = 1;

    GradientFill(hdc, vertBlue, 2, &gRectBlue, 1, GRADIENT_FILL_RECT_V);
}

void DisplayPlayerInfo(HDC hdc, int scoreA, int scoreB, LPCWSTR playerAName, LPCWSTR playerBName, int halfWidth, int windowHeight, int fontSize, int scoreYOffset) {
    // Use the same font sizes as before
    int scoreFontSize = fontSize * 7;
    int nameFontSize = fontSize;

    // Create fonts for score and name with "Segoe UI" and ClearType
    HFONT hScoreFont = CreateFont(scoreFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, L"Segoe UI");
    HFONT hNameFont = CreateFont(nameFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, L"Segoe UI");

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    // Draw Player A's score
    SelectObject(hdc, hScoreFont);
    wchar_t scoreTextA[10];
    wsprintf(scoreTextA, L"%d", scoreA);
    SIZE scoreSizeA;
    GetTextExtentPoint32(hdc, scoreTextA, lstrlen(scoreTextA), &scoreSizeA);
    int scoreAX = (halfWidth - scoreSizeA.cx) / 3;
    TextOut(hdc, scoreAX, scoreYOffset, scoreTextA, lstrlen(scoreTextA));

    // Draw Player A's name
    SelectObject(hdc, hNameFont);
    SIZE nameSizeA;
    GetTextExtentPoint32(hdc, playerAName, lstrlen(playerAName), &nameSizeA);
    int nameAX = (halfWidth - nameSizeA.cx) / 3;
    TextOut(hdc, nameAX, scoreYOffset + scoreSizeA.cy + 10, playerAName, lstrlen(playerAName));

    // Draw Player B's score
    SelectObject(hdc, hScoreFont);
    wchar_t scoreTextB[10];
    wsprintf(scoreTextB, L"%d", scoreB);
    SIZE scoreSizeB;
    GetTextExtentPoint32(hdc, scoreTextB, lstrlen(scoreTextB), &scoreSizeB);
    int scoreBX = halfWidth + (halfWidth - scoreSizeB.cx) / 1.5;
    TextOut(hdc, scoreBX, scoreYOffset, scoreTextB, lstrlen(scoreTextB));

    // Draw Player B's name
    SelectObject(hdc, hNameFont);
    SIZE nameSizeB;
    GetTextExtentPoint32(hdc, playerBName, lstrlen(playerBName), &nameSizeB);
    int nameBX = halfWidth + (halfWidth - nameSizeB.cx) / 1.5;
    TextOut(hdc, nameBX, scoreYOffset + scoreSizeB.cy + 10, playerBName, lstrlen(playerBName));

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
            Round_Timer = countdown;
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
        case WM_CTLCOLORBTN:
        {
            HDC hdcButton = (HDC)wParam;
            HWND hwndButton = (HWND)lParam;

            // Check if the button is one of our custom buttons
            if (hwndButton == hStartTimer || hwndButton == hResetScore ||
                hwndButton == hFinish || hwndButton == hNextRound)
            {
                SetTextColor(hdcButton, RGB(255, 255, 255)); // White text
                SetBkMode(hdcButton, TRANSPARENT);
                // Return a brush with the desired background color
                static HBRUSH hBrush = CreateSolidBrush(RGB(0, 122, 204)); // Blue background
                return (INT_PTR)hBrush;
            }
            break;
        }
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
        case NEXT_ROUND:
            
                CurrentRound++;
                countdown = Round_Timer; // Reset countdown to initial value
                timerRunning = false;    // Ensure timer is not running
                SetWindowText(hStartTimer, L"Start"); // Update button text
                UpdateDisplay(hWnd);

            break;
        case FINISH:
            timerRunning = false; // Stop the timer
            KillTimer(hWnd, TIMER_ID);
            // scoreA = 0;
            // scoreB = 0;
            countdown = 0;
            SetWindowText(hStartTimer, L"Start");
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
        int buttonSpacing = 10; // Space between buttons


        int totalButtonWidth = (buttonWidth * 4) + (buttonSpacing * 2);


        int buttonStartX = (width - totalButtonWidth) / 2;


        SetWindowPos(hStartTimer, NULL, buttonStartX, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hResetScore, NULL, buttonStartX + buttonWidth + buttonSpacing, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hFinish, NULL, buttonStartX + (buttonWidth + buttonSpacing) * 2, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);
        SetWindowPos(hNextRound, NULL, buttonStartX + (buttonWidth + buttonSpacing) * 3, height - 100, buttonWidth, buttonHeight, SWP_NOZORDER);

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
                HandleInput(hWnd, rawKB, hDevice, numpad1, scoreA, 1, scoreOneAdded, true);
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
            case VK_SUBTRACT:
                HandleInput(hWnd, rawKB, hDevice, numpadDec, scoreA, -1, scoreOneReduced, true);
                break;
            case VK_ADD:
                HandleInput(hWnd, rawKB, hDevice, numpadAdd, scoreB, -1, scoreOneReduced, false);
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
                HMENU hMenu = GetMenu(hWnd);
                int menuCount = GetMenuItemCount(hMenu);
                for (int i = 0; i < menuCount; i++) {
                    EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_GRAYED);
                }
                DrawMenuBar(hWnd);

                if (countdown <= 10 && countdown > 0) {
                    // Play timerunningout.wav in loop
                    PlaySound(TEXT("sound/timerunningout.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
                }
            }
            else {
                // Stop any playing sounds
                PlaySound(NULL, NULL, 0);

                // Stop the timer
                KillTimer(hWnd, TIMER_ID);
                timerRunning = false;
                SetWindowText(hStartTimer, L"Start");

                // Play matchover.wav asynchronously
                PlaySound(TEXT("sound/matchover.wav"), NULL, SND_FILENAME | SND_ASYNC);

                // Check the winner and update display
                match.CheckWinner();
                UpdateDisplay(hWnd);
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

        int timerYOffset = windowHeight / 4; // Adjust to position the timer
        int roundYOffset = timerYOffset + (fontSize * 3) + 20; // Position below the timer

        DisplayTimer(memDC, countdown, windowWidth, timerYOffset, fontSize);
        DisplayRound(memDC, windowWidth, roundYOffset, fontSize, Round);
        HBRUSH greenBrush;
        if (scoreOneReduced) {
            greenBrush = CreateSolidBrush(RGB(200, 0, 0));  // Active jury color
        }
        else {
            greenBrush = CreateSolidBrush(RGB(0, 200, 0));  // Active jury color
        }
        HBRUSH grayBrush = CreateSolidBrush(RGB(180, 180, 180)); // Inactive jury color

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
            int yOffset = 30;
            int leftXOffset = 20;
            // Determine which jury circle should be active
            if ((i == 0 && jury1KeyPressed && giveScoreForA) || (i == 1 && jury2KeyPressed && giveScoreForA) || (i == 2 && jury3KeyPressed && giveScoreForA)) {
                leftJuryBrush = greenBrush;
            }
            if ((i == 0 && jury1KeyPressed && !giveScoreForA) || (i == 1 && jury2KeyPressed && !giveScoreForA) || (i == 2 && jury3KeyPressed && !giveScoreForA)) {
                rightJuryBrush = greenBrush;
            }

            // Draw circles for left side (Player A)
           // Draw circles for left side (Player A)
              // Adjust this value to move circles further left
            SelectObject(memDC, leftJuryBrush);
            Ellipse(memDC, leftXOffset, 50 - yOffset + (i * (circleDiameter + circleSpacing)),
                leftXOffset + circleDiameter, 50 + circleDiameter - yOffset + (i * (circleDiameter + circleSpacing)));

            // Draw circles for right side (Player B)
            SelectObject(memDC, rightJuryBrush);
            Ellipse(memDC, windowWidth - circleSpacing - circleDiameter, 50 - yOffset + (i * (circleDiameter + circleSpacing)),
                windowWidth - circleSpacing, 50 + circleDiameter - yOffset + (i * (circleDiameter + circleSpacing)));
        }


        if (AWinner || BWinner || Tie) {
            match.ShowResult(memDC, halfWidth, windowHeight);
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
        if (hButtonFont)
        {
            DeleteObject(hButtonFont);
        }
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

