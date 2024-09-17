#pragma once
#ifndef MATCH_H
#define MATCH_H

#include <windows.h>

// Function declarations
INT_PTR CALLBACK SetupMatch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI SetupMatchThread(LPVOID lpParam);
void DisplayTimer(HDC hdc, int countdown, int windowWidth, int timerYOffset, int fontSize);

// Global variables
extern int currentJury;
extern int selectedJury;
extern int countdown;

extern wchar_t playerAName[50];
extern wchar_t playerBName[50];

#endif // MATCH_H
