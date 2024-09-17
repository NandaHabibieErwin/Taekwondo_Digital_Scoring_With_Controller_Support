#pragma once
#ifndef JURY_H
#define JURY_H

#include <windows.h>
#include <unordered_map>

// Jury-related global variables (declared as extern for shared usage)
extern int currentJury;
extern int selectedJury;

// Function declarations
void HandleInput(HWND hWnd, RAWKEYBOARD& rawKB, HANDLE hDevice,
    std::unordered_map<HANDLE, bool>& numpadState, int& score,
    int points, bool& scoreAdded, bool isPlayerA);

void RegisterMultipleKeyboards(HWND hwnd);
INT_PTR CALLBACK JuryConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif // JURY_H
