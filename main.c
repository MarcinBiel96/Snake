#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#define TIMER 50

int main() {
	int curr_pos, color = 170, snake = 52,columns, rows, height=23, width=38;
	PCHAR_INFO POINTER, POINTER2;
	COORD buffSize = { 0 }, buffCoord = { 0 };
	SMALL_RECT rect = { 0 };
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE SB = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

	if (!(POINTER = malloc(columns * rows * sizeof(CHAR_INFO))))return -1;
	for (int i = 0;i <columns*rows;++i) {
		POINTER[i].Char.UnicodeChar = ' ';
		POINTER[i].Attributes = FOREGROUND_GREEN | BACKGROUND_BLUE;
	}
	if (!(POINTER2 = malloc(height * width * sizeof(CHAR_INFO))))return -1;
	for (int i = 0;i < height * width;++i) {
		POINTER2[i].Char.UnicodeChar = 9999;
		POINTER2[i].Attributes = FOREGROUND_GREEN | BACKGROUND_GREEN;
	}
	
	buffSize.X = columns;
	buffSize.Y = rows;
	buffCoord.X = 0;
	buffCoord.Y = 0;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = rows;
	rect.Right = columns;
	WriteConsoleOutput(SB, POINTER, buffSize, buffCoord, &rect);

	buffSize.X = width;
	buffSize.Y = height;
	buffCoord.X = 0;
	buffCoord.Y = 0;
	rect.Top = (rows - height) / 2;
	rect.Left = (columns - width) / 2;
	rect.Bottom = height + (rows - height) / 2;
	rect.Right = width + (columns - width) / 2;
	WriteConsoleOutput(SB, POINTER2, buffSize, buffCoord, &rect);

	SetConsoleCursorInfo(SB, &(CONSOLE_CURSOR_INFO){ 100, FALSE });
	SetConsoleActiveScreenBuffer(SB);

	for (int i = 0;i < columns;++i) {
		POINTER[i].Char.UnicodeChar = '1';
		POINTER[i + (rows - 1) * columns].Char.UnicodeChar = '1';
	}

	curr_pos = height * width / 2;
	for (int i = 0;i < 4;++i)POINTER2[curr_pos - i].Char.UnicodeChar = 49 + i;
	
	HANDLE IB = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD irInBuf[128] = { 0 };
	DWORD Num;
	DWORD EvNo;

	int dir = 2;
	int rng = rand() % (width * height);
	MSG msg;
	SetTimer(NULL, 1, TIMER, NULL);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message = 275) {
			GetNumberOfConsoleInputEvents(IB, &EvNo);
			if (EvNo) {
				for (int i = 0; i < 128; i++)irInBuf[i].EventType = 0;
				ReadConsoleInput(IB, irInBuf, 128, &Num);
				for (int i = 127;i >= 0;--i) {
					if ((irInBuf[i].EventType == KEY_EVENT) 
						&& (irInBuf[i].Event.KeyEvent.bKeyDown) 
						&& (irInBuf[i].Event.KeyEvent.wVirtualKeyCode > 36)
						&& (irInBuf[i].Event.KeyEvent.wVirtualKeyCode < 41)) {
						int tmp = dir;
						dir = irInBuf[i].Event.KeyEvent.wVirtualKeyCode - 37;
						if (((dir - tmp) == 2) || ((tmp - dir) == 2)) dir = tmp;
						break;
					}
				}	
			}						
		}	

		switch (dir) {
		case 0:
			if (curr_pos % width == 0) curr_pos += width;
			curr_pos = curr_pos - 1;
			break;
		case 2:
			if (curr_pos % width == (width - 1)) curr_pos -= width;
			curr_pos = curr_pos + 1;
			break;
		case 3:
			if (curr_pos >= width * (height - 1)) curr_pos -= width * height;
			curr_pos = curr_pos + width;
			break;
		case 1:
			if (curr_pos < width) curr_pos += width * height;
			curr_pos = curr_pos - width;
			break;
		}
		POINTER2[rng].Char.UnicodeChar = 32;
		if (POINTER2[curr_pos].Char.UnicodeChar == 32) {
			++snake;
			do rng = rand() % (width * height);
			while (POINTER2[rng].Char.UnicodeChar <= snake);
		}
		else if (POINTER2[curr_pos].Char.UnicodeChar < snake)color = 0;
		POINTER2[curr_pos].Char.UnicodeChar = '0';

			for (int i = 0;i < height * width;++i) {
				POINTER2[i].Char.UnicodeChar = (POINTER2[i].Char.UnicodeChar + 1 > 100) ? 10000 : POINTER2[i].Char.UnicodeChar + 1;
				POINTER2[i].Attributes = color;
				if (POINTER2[i].Char.UnicodeChar > snake)
					POINTER2[i].Attributes = FOREGROUND_GREEN | BACKGROUND_GREEN;
				else if (POINTER2[i].Char.UnicodeChar == 33)
					POINTER2[i].Attributes = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY|BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY;
			}
			WriteConsoleOutput(SB, POINTER2, buffSize, buffCoord, &rect);
			if (color == 0)break;		
	}
	KillTimer(NULL, 1);
	printf("Score: %i\n", (snake - 52));
	return 0;
}