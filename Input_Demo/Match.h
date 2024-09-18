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
extern int Round;
extern int CurrentRound;

class Match{
public:
	void ShowResult(HDC hdc, int halfWidth, int windowHeight);
	void CheckWinner();
	};	
// Score-related variables
extern int scoreA;
extern int scoreB;


// Winner status flags
extern bool AWinner;
extern bool BWinner;
extern bool Tie;
\

#endif // MATCH_H
