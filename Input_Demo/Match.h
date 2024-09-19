#pragma once
#ifndef MATCH_H
#define MATCH_H

#include <windows.h>
#include "MatchState.h"
// Function declarations
INT_PTR CALLBACK SetupMatch(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI SetupMatchThread(LPVOID lpParam);
void DisplayTimer(HDC hdc, int countdown, int windowWidth, int timerYOffset, int fontSize);
void DisplayRound(HDC hdc, int windowWidth, int roundYOffset, int fontSize, int currentRound, int Round);
void DisplayTotalScore(HDC hdc, int totalScoreA, int totalScoreB, int halfWidth, int windowHeight);


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

extern int Round_Timer;


// Winner status flags
extern bool AWinner;
extern bool BWinner;
extern bool Tie;
\

#endif // MATCH_H
