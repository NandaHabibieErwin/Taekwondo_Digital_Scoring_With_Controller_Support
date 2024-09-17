	#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <unordered_map>

// Function declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SetupMatch(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    JuryConfig(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TestKeyboardProc(HWND, UINT, WPARAM, LPARAM);

// Global variables for the application
extern HINSTANCE hInst;
extern WCHAR szTitle[];
extern WCHAR szWindowClass[];

// Score-related variables
extern int scoreA;
extern int scoreB;

// Player Names
extern wchar_t playerAName[50];
extern wchar_t playerBName[50];

// Winner status flags
extern bool AWinner;
extern bool BWinner;
extern bool Tie;

// Jury devices
extern HANDLE jury1Device;
extern HANDLE jury2Device;
extern HANDLE jury3Device;

// Key pressed flags
extern bool jury1KeyPressed;
extern bool jury2KeyPressed;
extern bool jury3KeyPressed;

// Score input tracking
extern std::unordered_map<HANDLE, bool> numpad0;
extern std::unordered_map<HANDLE, bool> numpad1;
extern std::unordered_map<HANDLE, bool> numpad2;
extern std::unordered_map<HANDLE, bool> numpad3;
extern std::unordered_map<HANDLE, bool> numpad4;
extern std::unordered_map<HANDLE, bool> numpad5;
extern std::unordered_map<HANDLE, bool> numpad6;
extern std::unordered_map<HANDLE, bool> numpad7;
extern std::unordered_map<HANDLE, bool> numpad8;
extern std::unordered_map<HANDLE, bool> numpad9;
extern std::unordered_map<HANDLE, bool> numpadDec;
extern std::unordered_map<HANDLE, bool> numpadDiv;
extern std::unordered_map<HANDLE, bool> numpadMulti;
extern std::unordered_map<HANDLE, bool> numpadSub;
extern std::unordered_map<HANDLE, bool> numpadAdd;

extern std::unordered_map<HANDLE, bool> numpad;
extern std::unordered_map<HANDLE, bool> addedScoreA;
extern std::unordered_map<HANDLE, bool> addedScoreB;

extern bool giveScoreForA;

extern bool scoreOneAdded;
extern bool scoreTwoAdded;
extern bool scoreThreeAdded;
extern bool scoreFourAdded;
extern bool scoreFiveAdded;

// Jury configuration
extern int currentJury;
extern int selectedJury;
extern bool timerRunning;
extern int countdown;

// HWND variables for UI elements
extern HWND hTimerInput;
extern HWND hStartTimer;
extern HWND hResetScore;
extern HWND hFinish;

#endif // MAIN_H
