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

extern int RestTime;

extern bool WinnerDecided;

class Match{
public:
	void ShowResult(HDC hdc, int halfWidth, int windowHeight);
	void CheckWinner();

	enum MATCH_STATE {
		MATCH_IDLE = 0,
		MATCH_PLAYING = 1,
		MATCH_REST = 2,
		MATCH_OVER = 3,
	};


	};	

extern Match::MATCH_STATE CurrentMatchState;
// Score-related variables
extern int scoreA;
extern int scoreB;

extern int Round_Timer;
extern bool WinnerDecided;

// Winner status flags
extern bool AWinner;
extern bool BWinner;
extern bool Tie;
\

#endif // MATCH_H
