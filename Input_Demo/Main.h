	#pragma once
#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <memory>
#include<SFML/Audio.hpp>
#include <string>
#include <Shlwapi.h>
#include <codecvt>
#include <tchar.h>
#include "Match.h"
#pragma comment(lib, "Shlwapi.lib") 


// Function declarations
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SetupMatch(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    JuryConfig(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TestKeyboardProc(HWND, UINT, WPARAM, LPARAM);

void EnableButton(HWND Button);
void DisableButton(HWND Button);

// Global variables for the application
extern HINSTANCE hInst;
extern WCHAR szTitle[];
extern WCHAR szWindowClass[];

extern HWND hStartTimer;
extern HWND hResetScore;
extern HWND hFinish;
extern HWND hNextRound;

extern int PlayerATotalScore;
extern int PlayerBTotalScore;

extern bool timerRunning;

extern int RestTimer;

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



extern bool scoreOneReduced;

// HWND variables for UI elements
extern HWND hTimerInput;
extern HWND hStartTimer;
extern HWND hResetScore;
extern HWND hFinish;

std::string WideStringToString(const std::wstring& wstr);
void PlayAudio(const std::wstring& audio, bool loop);

#endif // MAIN_H
