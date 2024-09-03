// Input_Demo.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Input_Demo.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
int scoreA = 0;
int scoreB = 0;
bool aPress = false;
bool bPress = false;
bool cPress = false;
bool dPress = false;
static bool timerRunning = false;




int countdown = 0; // Countdown starting value in seconds

HWND hTimerInput;
HWND hStartTimer;
HWND hResetScore;


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    hTimerInput = CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 120, 70, 100, 25, hWnd, (HMENU)TIMER_INPUT, hInstance, NULL);
    hStartTimer = CreateWindowEx(0, L"BUTTON", L"Start Timer", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 70, 100, 25, hWnd, (HMENU)START_TIMER, hInstance, NULL);
    hResetScore = CreateWindowEx(0, L"BUTTON", L"Reset Score", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 100, 100, 25, hWnd, (HMENU)RESET_SCORE, hInstance, NULL);


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
        case START_TIMER:
        {
            wchar_t timerValue[10];
            GetWindowText(hTimerInput, timerValue, 10);
            countdown = _wtoi(timerValue);
            if (countdown > 0 && !timerRunning) {
                timerRunning = true;
                SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);
            }
            MessageBox(hWnd, L"Timer have been set.", L"Run", MB_OK | MB_ICONINFORMATION);
            UpdateDisplay(hWnd); // Force immediate repaint after starting the timer
        }
        break;
        case RESET_SCORE:
        {
            scoreA = 0;
            scoreB = 0;
            UpdateDisplay(hWnd); // Force immediate repaint after starting the timer
            MessageBox(hWnd, L"Score have been reset.", L"Success", MB_OK | MB_ICONINFORMATION);
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_KEYDOWN:
        if (wParam == VK_NUMPAD4) {
            aPress = true;           
        }
        else if (wParam == VK_NUMPAD7) {
            bPress = true;            
        }
        else if (wParam == VK_NUMPAD9) {
            cPress = true;            
        }
        else if (wParam == VK_NUMPAD6) {
            dPress = true;            
        }
        if (aPress && bPress) {
            scoreA++;
            aPress = false;
                bPress = false;
            UpdateDisplay(hWnd); // Force immediate repaint
        }

        if (cPress && dPress) {
            scoreB++;
            cPress = false;
                dPress = false;
            UpdateDisplay(hWnd); // Force immediate repaint
        }
        break;

    case WM_KEYUP:
        if (wParam == VK_NUMPAD4) {
            aPress = false;
        }
        else if (wParam == VK_NUMPAD7) {
            bPress = false;
        }
        else if (wParam == VK_NUMPAD6) {
            cPress = false;
        }
        else if (wParam == VK_NUMPAD9) {
            dPress = false;
        }
        break;

    case WM_TIMER:
    {
        if (wParam == TIMER_ID) {
            if (countdown > 0) {
                countdown--;
                // Force the window to repaint
                UpdateDisplay(hWnd);
            }
            else {
                KillTimer(hWnd, TIMER_ID); // Stop the timer
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
        hdc = BeginPaint(hWnd, &ps);

        showScore(hdc);       // Draw the scores
        showTimer(hdc, countdown); // Draw the timer

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




// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

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

