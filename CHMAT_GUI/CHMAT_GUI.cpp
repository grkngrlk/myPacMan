#include <windows.h>  
#include "chmat.h"
#include "pacman.h"
#include <conio.h>
#include <iostream>
#include <Mmsystem.h>
#include <mciapi.h>
#include <chrono>
#include <string> 
#include <atlstr.h>
#pragma comment(lib, "Winmm.lib")



#define IDI_ICON1 101
#define IDC_OPEN	3000
#define IDC_BUTTON1 3001
#define IDC_BUTTON2 3002
#define IDC_BUTTON3 3003

HINSTANCE hInst;   // current instance
HWND     hWnd, hEdit, hEditScoreLabel, hEditScoreDisplayer, hEditPacmanLifeLabel, hEditPacmanLifeDisplayer, HButton1, HButton2, HButton3, Hmainbmp;    //parent window //

LPCTSTR lpszAppName = "GEN32";
LPCTSTR lpszTitle = "PacMan";
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	MSG      msg;
	WNDCLASSEX wc;
	HMENU MainMenu, FileMenu;
	MainMenu = CreateMenu();
	FileMenu = CreatePopupMenu();
	AppendMenu(FileMenu, MF_STRING, IDC_OPEN, "Open");
	AppendMenu(MainMenu, MF_POPUP, (UINT_PTR)FileMenu, "FILE");

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = lpszAppName;
	wc.lpszClassName = lpszAppName;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm = (HICON)LoadImage(hInstance, lpszAppName,
		IMAGE_ICON, 16, 16,
		LR_DEFAULTCOLOR);

	if (!RegisterClassEx(&wc))
		return(FALSE);

	hInst = hInstance;
	hWnd = CreateWindowEx(0, lpszAppName,
		lpszTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		590, 750,
		NULL,
		MainMenu,
		hInstance,
		NULL
		);


	if (!hWnd)
		return(FALSE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
}

//   !!!!!!!!!!!!!!!!!!!Global definition makes it easier!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
CHMAT MAP(560, 620, CHMAT_INT);
CHMAT checker(560, 620, CHMAT_INT);
Pacman pacman(261, 131, 5, 0xffff00);
Pacman ghost1(261, 371, 5, 0xffff00);
Pacman ghost2(301, 321, 5, 0xffa500);
Pacman ghost3(261, 321, 5, 0xffb6c1);
Pacman ghost4(221, 321, 2, 0x00ffff);
HANDLE hTimerQueue = NULL;
HANDLE hTimerQueue2 = NULL;
CString t;

bool IS_LOCATION_KNOWN = false;
bool GAME_PAUSE = false;
int KEY_PRESSED;
bool DID_PACMAN_EAT_FRUIT = false; 
int SIREN_SOUND_MODE = 1; 
int PACMAN_SPEED;
int COUNTER_FOR_G2 = 0;
int COUNTER_FOR_G4 = 0;
int EATEN_FOOD_COUNTER = 0;
int HUNTING_MODE_TIMER = 5000;
int HUNTING_MODE_COUNTER = 1;
int PACMAN_KILL_SCORE = 200;

void pause(int time) {

	GAME_PAUSE = true;
	Sleep(time);
	GAME_PAUSE = false;

}

DWORD WINAPI playSirenSoundWithThread(LPVOID lpParam) {
	while (true) {
		if (SIREN_SOUND_MODE == 0) {
		}
		else if (SIREN_SOUND_MODE == 1) {
			mciSendString("play sounds\\siren_1.wav", NULL, 0, NULL);
		}
		else if (SIREN_SOUND_MODE == 2) {
			mciSendString("play sounds\\siren_2.wav", NULL, 0, NULL);
		}
		else if (SIREN_SOUND_MODE == 3) {
			mciSendString("play sounds\\siren_3.wav", NULL, 0, NULL);
		}
		else if (SIREN_SOUND_MODE == 4) {
			mciSendString("play sounds\\siren_4.wav", NULL, 0, NULL);
		}
		else if (SIREN_SOUND_MODE == 5) {
			mciSendString("play sounds\\siren_5.wav", NULL, 0, NULL);
		}
		else if (SIREN_SOUND_MODE == 6) {// TODO: hunter mode sound delay
			mciSendString("play sounds\\power_pellet.wav", NULL, 0, NULL);
		}
	}
	return 0;
}

void playPackmanEatFruitSound() { 

	if (!DID_PACMAN_EAT_FRUIT) {
		const char* WAV = "sounds\\credit.wav";
		DID_PACMAN_EAT_FRUIT = true;
		DID_PACMAN_EAT_FRUIT = !PlaySound(WAV, NULL, SND_FILENAME);

	}
}

void playPackmanDeadSound() { 
	const char* WAV = "sounds\\pacman_death.wav";
	PlaySound(WAV, NULL, SND_FILENAME || SND_ASYNC);
	SIREN_SOUND_MODE = 0;
	
}


void eatGhostSound() { 
	const char* WAV = "sounds\\eat_ghost.wav";// cow.wav is a piano song in hard drive
	PlaySound(WAV, GetModuleHandle(NULL), SND_FILENAME || SND_ASYNC);
}

void playBeginningSound() { 
	const char* WAV = "sounds\\pacman_beginning.wav";// cow.wav is a piano song in hard drive
	PlaySound(WAV, GetModuleHandle(NULL), SND_FILENAME || SND_ASYNC);
}

void initiatePackman() { 
	MAP.printPacman(pacman.getX(), pacman.getY(), pacman.getSize(), pacman.getColor(), pacman.getMoveTo() ,&pacman.step,true);
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
}


void ghostsClear() {
	MAP.ghostClear(checker, ghost1.getX(), ghost1.getY(), ghost1.getSize(),ghost1.getMod());
	MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(),ghost2.getMod());
	MAP.ghostClear(checker, ghost3.getX(), ghost3.getY(), ghost3.getSize(),ghost3.getMod());
	MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(),ghost4.getMod());

	//printRemaningFoods();// when clearing the ghost, it deletes the food behind the ghost, it doesn't fix it 

}

void restartObjects() {// restarts evey object(pacman and ghosts)

	ghost1.setMod(0);
	ghost1.setX(ghost1.getStartX());
	ghost1.setY(ghost1.getStartY());

	ghost2.setMod(0);
	ghost2.setX(ghost2.getStartX());
	ghost2.setY(ghost2.getStartY());
	ghost2.setMoveTo(2);


	ghost3.setMod(3);
	ghost3.setX(ghost3.getStartX());
	ghost3.setY(ghost3.getStartY());

	ghost4.setMod(0);
	ghost4.setX(ghost4.getStartX());
	ghost4.setY(ghost4.getStartY());
	ghost4.setMoveTo(2);


	COUNTER_FOR_G2= 0;
	COUNTER_FOR_G4 = 0;


	Sleep(1500);
	MAP.clearPacman(pacman.getX(), pacman.getY(), pacman.getSize());
	pacman.setX(pacman.getStartX());
	pacman.setY(pacman.getStartY());

	MAP.printPacman(pacman.getX(), pacman.getY(), pacman.getSize(), pacman.getColor(), pacman.getMoveTo(), &pacman.step, true);

}

void pacmanLevelUp() { 
	if (HUNTING_MODE_TIMER > 1000)//less hunting time
		HUNTING_MODE_TIMER -= 500;
	EATEN_FOOD_COUNTER= 0;
	GAME_PAUSE = true;
	SIREN_SOUND_MODE = 0;
	ghostsClear();
	MAP.clearPacman(pacman.getX(),pacman.getY(),pacman.getSize());
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
	MAP.drawFoods(checker);
	restartObjects();
	Sleep(300);
	SIREN_SOUND_MODE = 1;
	GAME_PAUSE = false;

}

void pacmanDies() {
	playPackmanDeadSound();
	pacman.setLife(pacman.getLife() - 1);
	t.Format(_T("%d"), pacman.getLife());		
	SetWindowText(hEditPacmanLifeDisplayer, t); 

	//TODO: foods are getting deleted after cleaning the ghosts
	ghostsClear();
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);


	GAME_PAUSE = true;

	restartObjects();


	if (pacman.getLife() != 0)
		GAME_PAUSE = false;


}


DWORD WINAPI startHuntingMode(LPVOID lpParam) {


	if (ghost1.getMod() != 2)
		ghost1.setMod(1);
	if (ghost2.getMod() != 2)
		ghost2.setMod(1);
	if (ghost3.getMod() != 2)
		ghost3.setMod(1);
	if (ghost4.getMod() != 2)
		ghost4.setMod(1);

	pacman.setMod(1);
	SIREN_SOUND_MODE = 6;//start hunting sound

	ghost1.reverse();//reverse the directions
	ghost2.reverse();
	ghost3.reverse();
	ghost4.reverse();

	for (int i = 0; i < HUNTING_MODE_COUNTER; i++)
		Sleep(HUNTING_MODE_TIMER);
	HUNTING_MODE_COUNTER = 1;
	SIREN_SOUND_MODE = 1;// end hunting sound
	PACMAN_KILL_SCORE = 200;

	if (ghost1.getMod() == 1)//goes back to normal
		ghost1.setMod(0);
	if (ghost2.getMod() == 1)
		ghost2.setMod(0);
	if (ghost3.getMod() == 1)
		ghost3.setMod(0);
	if (ghost4.getMod() == 1)
		ghost4.setMod(0);

	pacman.setMod(0);

	return 0;
}

int whereToMove (Pacman ghost) {
	int holder;
	if (checker(ghost.getX() + 19, ghost.getY() + 17) == 13) {
		if (IS_LOCATION_KNOWN && ghost.getMod() == 0) {
			holder = checker.ghostMove(pacman.getX(), pacman.getY(), ghost.getX(), ghost.getY());
			if (holder != 0) {
				return(holder);
			}
			else {
				return(checker.ghostMove(ghost.getX(), ghost.getY(), ghost.getMoveTo()));
			}
		}
		else {
			return(checker.ghostMove(ghost.getX(), ghost.getY(), ghost.getMoveTo()));
		}
	}
	else {
		return(checker.ghostMove(ghost.getX(), ghost.getY(), ghost.getMoveTo()));
	}

}

int ghostGoBack(Pacman ghost) {

	if (checker(ghost.getX() + 19, ghost.getY() + 9) != 0 && checker(ghost.getX() + 19, ghost.getY() + 17) == 13) {
		return (checker(ghost.getX() + 19, ghost.getY() + 9) - 10);
	}
	if (ghost.getX() == 261 && ghost.getY() <= 371 && ghost.getY() > 321) {
		return(4);
	}
	else if (checker.isMovable(ghost.getX(), ghost.getY(), ghost.getMoveTo())) {
		return(ghost.getMoveTo());
	}
	else {
		return(checker.ghostMove(ghost.getX(), ghost.getY(), ghost.getMoveTo()));
	}
}

VOID CALLBACK playGhost1(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	
	if (!GAME_PAUSE && (ghost1.getMod() == 1 || ghost1.getMod() == 0)) {
		int holder, notMove;

		ghost1.setMoveTo(whereToMove(ghost1));
		MAP.ghostClear(checker, ghost1.getX(), ghost1.getY(), ghost1.getSize(),ghost1.getMod());
		
		ghost1.move();
		MAP.printGhost(ghost1.getX(), ghost1.getY(), ghost1.getSize(), ghost1.getMoveTo(), 0xff0000, ghost1.getMod());

		if (ghost1.didGhostKill(pacman.getX(), pacman.getY()) && ghost1.getMod() != 2) {
			if (ghost1.getMod() == 0) {
				pacmanDies();
			}
			else if (ghost1.getMod() == 1) {
				ghost1.setMod(2);
				eatGhostSound();
				pacman.setScore(pacman.getScore() +  PACMAN_KILL_SCORE);
				PACMAN_KILL_SCORE *= 2;
				t.Format(_T("%d"), pacman.getScore());
				SetWindowText(hEditScoreDisplayer, t);
				pause(1000);
			}
		}
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
	}
	else if (ghost1.getMod() == 2) { // ghost goes into the home
		for (int i = 0; i < 3; i++)
		{
			if (ghost1.getX() == 261 && ghost1.getY() == 321) ghost1.setMod(3);
			ghost1.setMoveTo(ghostGoBack(ghost1));
			MAP.ghostClear(checker, ghost1.getX(), ghost1.getY(), ghost1.getSize(), ghost1.getMod());
			ghost1.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost1.getX(), ghost1.getY(), ghost1.getSize(), ghost1.getMoveTo(), 0xff0000, ghost1.getMod());
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}

	}
	else if (ghost1.getMod() == 3 && !GAME_PAUSE) {//ghost leaves the home
		if (ghost1.getX() == 261 && ghost1.getY() == 371) {
			ghost1.setMod(0);
		}
		else {
			ghost1.setMoveTo(2);
			MAP.ghostClear(checker, ghost1.getX(), ghost1.getY(), ghost1.getSize(), ghost1.getMod());
			ghost1.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost1.getX(), ghost1.getY(), ghost1.getSize(), ghost1.getMoveTo(), 0xff0000, ghost1.getMod());
		}

		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);

	}
	
}

VOID CALLBACK playGhost2(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	
	if (!GAME_PAUSE && (ghost2.getMod() == 1 || ghost2.getMod() == 0))
	{

		if (COUNTER_FOR_G2 < 500)
		{
			COUNTER_FOR_G2++;
			if (checker.isMovable(ghost2.getX(), ghost2.getY(), ghost2.getMoveTo()))
			{
				MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(),ghost2.getMod());
				ghost2.move();
				MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());
			}
			else
			{
				if (ghost2.getMoveTo() == 4)
					ghost2.setMoveTo(2);
				else
					ghost2.setMoveTo(4);
			}
		}
		else if (COUNTER_FOR_G2 < 540)
		{
			COUNTER_FOR_G2++;
			ghost2.setMoveTo(3);
			MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMod());
			ghost2.move();
			MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());
		}
		else if (COUNTER_FOR_G2 < 542)
		{
			ghost2.setMoveTo(2);
			MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMod());
			ghost2.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());
			if (ghost2.getY() == 371)
				COUNTER_FOR_G2 = 543;

		}
		else
		{
			int holder, notMove;

			ghost2.setMoveTo(whereToMove(ghost2));
			MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMod());

			ghost2.move();
			MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());

			if (ghost2.didGhostKill(pacman.getX(), pacman.getY()) && ghost2.getMod() != 2) {
				if (ghost2.getMod() == 0) {
					pacmanDies();
				}
				else if (ghost2.getMod() == 1) {
					ghost2.setMod(2);
					eatGhostSound();
					pacman.setScore(pacman.getScore() + PACMAN_KILL_SCORE);
					PACMAN_KILL_SCORE *= 2;
					t.Format(_T("%d"), pacman.getScore()); 
					SetWindowText(hEditScoreDisplayer, t);
					pause(1000);

				}
			}
		}
		
	}
	else if (ghost2.getMod() == 2) {
		for (int i = 0; i < 3; i++)
		{
			if (ghost2.getX() == 261 && ghost2.getY() == 321) ghost2.setMod(3);
			ghost2.setMoveTo(ghostGoBack(ghost2));
			MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMod());
			ghost2.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}

	}
	else if (ghost2.getMod() == 3 && !GAME_PAUSE) {//ghost leaves the home
		if (ghost2.getX() == 261 && ghost2.getY() == 371) {
			ghost2.setMod(0);
		}
		else {
			ghost2.setMoveTo(2);
			MAP.ghostClear(checker, ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMod());
			ghost2.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost2.getX(), ghost2.getY(), ghost2.getSize(), ghost2.getMoveTo(), ghost2.getColor(), ghost2.getMod());
		}
	}
	SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
}

VOID CALLBACK playGhost3(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (!GAME_PAUSE && (ghost3.getMod() == 1 || ghost3.getMod() == 0))
	{
		
		int holder, notMove;

		ghost3.setMoveTo(whereToMove(ghost3));
		MAP.ghostClear(checker, ghost3.getX(), ghost3.getY(), ghost3.getSize(),ghost3.getMod());

		ghost3.move();
		MAP.printGhost(ghost3.getX(), ghost3.getY(), ghost3.getSize(), ghost3.getMoveTo(), ghost3.getColor(), ghost3.getMod());

		if (ghost3.didGhostKill(pacman.getX(), pacman.getY()) && ghost3.getMod() != 2) {
			if (ghost3.getMod() == 0) {
				pacmanDies();
			}
			else if (ghost3.getMod() == 1) {
				ghost3.setMod(2);
				eatGhostSound();
				pacman.setScore(pacman.getScore() + PACMAN_KILL_SCORE);
				PACMAN_KILL_SCORE *= 2;
				t.Format(_T("%d"), pacman.getScore());
				SetWindowText(hEditScoreDisplayer, t);
				pause(1000);
			}
		}
		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		
	}
	else if (ghost3.getMod() == 2) {
		for (int i = 0; i < 3; i++)
		{
			if (ghost3.getX() == 261 && ghost3.getY() == 321) ghost3.setMod(3);
			ghost3.setMoveTo(ghostGoBack(ghost3));
			MAP.ghostClear(checker, ghost3.getX(), ghost3.getY(), ghost3.getSize(), ghost3.getMod());
			ghost3.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost3.getX(), ghost3.getY(), ghost3.getSize(), ghost3.getMoveTo(), ghost3.getColor(), ghost3.getMod());
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}

	}
	else if (ghost3.getMod() == 3 && !GAME_PAUSE) {//ghost leaves the home
		if (ghost3.getX() == 261 && ghost3.getY() == 371) {
			ghost3.setMod(0);
		}
		else {
			ghost3.setMoveTo(2);
			MAP.ghostClear(checker, ghost3.getX(), ghost3.getY(), ghost3.getSize(), ghost3.getMod());
			ghost3.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost3.getX(), ghost3.getY(), ghost3.getSize(), ghost3.getMoveTo(), ghost3.getColor(), ghost3.getMod());
		}

		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);

	}
}

VOID CALLBACK playGhost4(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (!GAME_PAUSE && (ghost4.getMod() == 1 || ghost4.getMod() == 0))
	{

		if (COUNTER_FOR_G4 < 200)
		{
			COUNTER_FOR_G4++;
			if (checker.isMovable(ghost4.getX(), ghost4.getY(), ghost4.getMoveTo()))
			{
				MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(),ghost4.getMod());
				ghost4.move();
				MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());
			}
			else
			{
				if (ghost4.getMoveTo() == 4)
					ghost4.setMoveTo(2);
				else
					ghost4.setMoveTo(4);
			}
		}
		else if (COUNTER_FOR_G4 < 240)
		{
			COUNTER_FOR_G4++;
			ghost4.setMoveTo(1);
			MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMod());
			ghost4.move();
			MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());
		}
		else if (COUNTER_FOR_G4 < 242)
		{
			ghost4.setMoveTo(2);
			MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMod());
			ghost4.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());
			if (ghost4.getY() == 371)
				COUNTER_FOR_G4 = 243;

		}
		else 
		{
			int holder, notMove;

			ghost4.setMoveTo(whereToMove(ghost4));
			MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMod());

			ghost4.move();
			MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());

			if (ghost4.didGhostKill(pacman.getX(), pacman.getY()) && ghost4.getMod() != 2) {
				if (ghost4.getMod() == 0) {
					pacmanDies();
				}
				else if (ghost4.getMod() == 1) {
					ghost4.setMod(2);
					eatGhostSound();
					pacman.setScore(pacman.getScore() + PACMAN_KILL_SCORE);
					PACMAN_KILL_SCORE *= 2;
					t.Format(_T("%d"), pacman.getScore());
					SetWindowText(hEditScoreDisplayer, t);
					pause(1000);
				}
			}
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}
	}
	else if (ghost4.getMod() == 2) {
		for (int i = 0; i < 3; i++)
		{
			if (ghost4.getX() == 261 && ghost4.getY() == 321) ghost4.setMod (3);
			ghost4.setMoveTo(ghostGoBack(ghost4));
			MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMod());
			ghost4.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}

	}
	else if (ghost4.getMod() == 3 && !GAME_PAUSE) {//ghost leaves the home
		if (ghost4.getX() == 261 && ghost4.getY() == 371) {
			ghost4.setMod(0);
		}
		else {
			ghost4.setMoveTo(2);
			MAP.ghostClear(checker, ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMod());
			ghost4.move();
			MAP.Rectangle(260, 365, 40, 5, 0xFADBD8, true);
			MAP.printGhost(ghost4.getX(), ghost4.getY(), ghost4.getSize(), ghost4.getMoveTo(), ghost4.getColor(), ghost4.getMod());
		}

		SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);

	}
}

VOID CALLBACK playPacman(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	if (pacman.getMod() == 1) PACMAN_SPEED = 2;
	else PACMAN_SPEED = 1;
	for (int i = 0; i < PACMAN_SPEED; i++)
	{
		if (!GAME_PAUSE) {
			int moveTo = 0, lastMove = 0;
			switch (KEY_PRESSED)
			{
			case MAP.RightArrow:
				moveTo = 1;
				break;
			case MAP.UpArrow:
				moveTo = 2;
				break;
			case MAP.LeftArrow:
				moveTo = 3;
				break;
			case MAP.DownArrow:
				moveTo = 4;
				break;

			}
			if (checker.isMovable(pacman.getX(), pacman.getY(), moveTo)) {// checks if pacman can move to moveTo(if moveTo == 1, means go right, 2 means go up, 3 means go left,4 means go down) 
				pacman.setMoveTo(moveTo);
				MAP.Ellipse(pacman.getX() + 4, pacman.getY() + 4, pacman.getSize() - 8, pacman.getSize() - 8, 0, true);
				pacman.move();
				MAP.printPacman(pacman.getX(), pacman.getY(), pacman.getSize(), pacman.getColor(), pacman.getMoveTo(), &pacman.step, true);
				lastMove = moveTo;
			}
			else if (checker.isMovable(pacman.getX(), pacman.getY(), pacman.getMoveTo())) {
					MAP.Ellipse(pacman.getX() + 4, pacman.getY() + 4, pacman.getSize() - 8, pacman.getSize() - 8, 0, true);
					pacman.move();
					MAP.printPacman(pacman.getX(), pacman.getY(), pacman.getSize(), pacman.getColor(), pacman.getMoveTo(), &pacman.step, true);
			}

			switch (checker.pacmanEatFood(pacman.getX(), pacman.getY(), pacman.getMoveTo()))
			{
			case 10:
				pacman.setScore(10+pacman.getScore());
				playPackmanEatFruitSound();
				EATEN_FOOD_COUNTER++;

				t.Format(_T("%d"), pacman.getScore());	

				SetWindowText(hEditScoreDisplayer, t);			

				if (EATEN_FOOD_COUNTER == 242)
				{
					pacmanLevelUp();

				}
				break;
			case 50:
				pacman.setScore(50 + pacman.getScore());
				t.Format(_T("%d"), pacman.getScore());

				EATEN_FOOD_COUNTER++;
				if (pacman.getMod() == 1) {
					HUNTING_MODE_COUNTER++;
				}
				else {
					DWORD   dwThreadId;
					HANDLE  h_thread;

					h_thread = CreateThread(NULL,0,startHuntingMode,NULL,0,&dwThreadId);   

				}

				SetWindowText(hEditScoreDisplayer, t);	

				if (EATEN_FOOD_COUNTER == 242)
				{
					pacmanLevelUp();
				}
				break;
			}

			
			if (pacman.isGhostClose(ghost1.getX(), ghost1.getY()) || pacman.isGhostClose(ghost2.getX(), ghost2.getY()) || pacman.isGhostClose(ghost3.getX(), ghost3.getY()) || pacman.isGhostClose(ghost4.getX(), ghost4.getY()))
				IS_LOCATION_KNOWN = true;
			else
				IS_LOCATION_KNOWN = false;
			SendMessage(Hmainbmp, STM_SETIMAGE, 0, (LPARAM)MAP.HBitmap);
		}

	}
}

void initiateTheGame(HWND hWnd) {
		
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0)) ;
	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);	

	t.Format(_T("%d"), pacman.getLife());	
	SetWindowText(hEditPacmanLifeDisplayer, t);	
	SetWindowText(hEditScoreLabel, "Score");	
	SetWindowText(hEditPacmanLifeLabel, "Life");	
	
	MAP = 0;
	checker = 0;

	MAP.drawBoard(0x2e27ff);
	MAP.drawFoods(checker);

	checker.drawBoard(1);
	checker.setTurningPoint();
	initiatePackman();
	checker.goingBackPoints();

	ghost3.setMod(3);

	DWORD   dwThreadId;
	HANDLE  hThread;

	hThread = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size
		playSirenSoundWithThread,       // thread function name
		NULL,          // argument to thread function
		0,                      // use default creation flags
		&dwThreadId);   // returns the thread identifier

	//Starting Timer
	HANDLE hTimer = NULL;

	hTimerQueue = CreateTimerQueue();
	if (NULL != hTimerQueue)
	{
		//starts 1000 ms later calls thread every 30 ms
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)playPacman, NULL, 0, 10, 0);
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)playGhost1, NULL, 0, 11, 0);
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)playGhost2, NULL, 0, 11, 0);
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)playGhost3, NULL, 0, 11, 0);
		CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)playGhost4, NULL, 0, 11, 0);
	}
		playBeginningSound();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_CREATE:
	{		
		hEditScoreLabel = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "", WS_CHILD | WS_VISIBLE |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | EM_SETREADONLY, 5, 660, 75, 20, hWnd, NULL, hInst, NULL);
		hEditScoreDisplayer = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "", WS_CHILD | WS_VISIBLE |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | EM_SETREADONLY, 85, 660, 75, 20, hWnd, NULL, hInst, NULL);
		hEditPacmanLifeLabel = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "", WS_CHILD | WS_VISIBLE |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | EM_SETREADONLY, 350, 660, 75, 20, hWnd, NULL, hInst, NULL);
		hEditPacmanLifeDisplayer = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "", WS_CHILD | WS_VISIBLE |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | EM_SETREADONLY, 430, 660, 75, 20, hWnd, NULL, hInst, NULL);
		HButton1 = CreateWindowEx(NULL, "BUTTON", "Play Again", WS_CHILD | WS_VISIBLE | SS_CENTER,
			2, 2, 80, 20, hWnd, (HMENU)IDC_BUTTON1, hInst, NULL);
		HButton2 = CreateWindowEx(NULL, "BUTTON", "Exit", WS_CHILD | WS_VISIBLE | SS_CENTER,
			82, 2, 80, 20, hWnd, (HMENU)IDC_BUTTON2, hInst, NULL);
		Hmainbmp = CreateWindowEx(NULL, "STATIC", "", WS_CHILD | WS_VISIBLE | SS_BITMAP | WS_THICKFRAME, 1, 23, 600, 500, hWnd, NULL, hInst, NULL);

		initiateTheGame(hWnd); 
	}
	break;
	case WM_CTLCOLOREDIT:	
	{
		HDC hdc = (HDC)wParam;
		HWND hwnd = (HWND)lParam;
		LRESULT lRet = 0; // return value for our WindowProc.
		const COLORREF blackColor = RGB(0, 0, 0); // use RED for Background.
		COLORREF whiteColor = RGB(255, 250, 250);
		
		SetTextColor(hdc, whiteColor);
		SetBkColor(hdc, blackColor); // Set to red
		lRet = (LRESULT)GetStockObject(DC_BRUSH); // return a DC brush.
	}
	break;
	case WM_KEYDOWN:
	{

		KEY_PRESSED = (int)wParam;
		
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_OPEN:
		{
			OPENFILENAME ofn;

			char szFile[260];
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			//
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			//
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			GetOpenFileName(&ofn);
		}
		break;
		case IDC_BUTTON1:
		{
			pacman.setScore(0);
			pacman.setLife(3);
			pacmanLevelUp();// click somewhere out of the window and open the window back
			HUNTING_MODE_TIMER = 5000;
			t.Format(_T("%d"), pacman.getLife());
			SetWindowText(hEditPacmanLifeDisplayer, t);
			SetFocus(hWnd);
		}
		break;
		case IDC_BUTTON2:
		{
			SetFocus(hWnd);
			exit(0);
		}
		break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}

	return(0L);
}


