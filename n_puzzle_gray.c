/* default version
 * Font: consolas
 * Size: 22
 * loading size: 134x23 
 * game size: 100x28
 */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <dos.h>
#include <dir.h>

#define BEST_SCORE_PATH "BestScore"
#define SAVE_MAP_PATH "Map"

#define SCREEN_MAX_X 100
#define SCREEN_MAX_Y 28
#define MENU_MAX_X 45
#define MENU_MAX_Y 10
#define WIDTH 4
#define HEIGHT 1
#define MAX_N 10
#define SPEED 120 //200
#define SPEED_PULL_NUM 50

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3

#define BLUE 1
#define YELLOW 14
#define GREEN 10
#define LIGHTCYAN 11
#define RED 12
#define LIGHTMAGENTA 13
#define WHITE 15

typedef struct
{
	int y, x;
} toado;

typedef struct
{
	toado cursor;
	int map[MAX_N][MAX_N];
	int parent;
	int index;
} thongtin;

typedef struct
{
	thongtin a[500];
	int n;
} List;

int move[][2] = { {-1, 0}, {0, -1}, {0, 1}, {1, 0} };

List mo;
const char MENU[][35] = {"Play game", "Option", "Show score", "Quit"};
int MENU_SIZE = sizeof(MENU) / sizeof(MENU[0]);
int mapdau[MAX_N][MAX_N], mapcuoi[MAX_N][MAX_N];
int timeStart_fromLastMap, n = 3; // default
int screenX, screenY;
toado vitri, space;

void textcolor(int ForgC)
{
	WORD wColor;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
	return;
}

void gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if(!h)
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD c = { x, y };
	SetConsoleCursorPosition(h,c);
}

int getConsoleSize_Cols()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

int getConsoleSize_Rows()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void int_str(char *s, int n)
{
	if (n == 0)
	{
		s[0] = '\0';
		return;
	}
	char c[2] = {n % 10 + '0'};

	int_str(s, n / 10);
	strcat(s, c);
}

char* create_fileName(char path[], int value) // return path+value.txt
{
	char s[50];
	strcpy(s, path);

	char s_num[4];
	int_str(s_num, value);
	strcat(s, s_num);
	strcat(s, ".txt");

	return strdup(s);
}

void set_console_size(int col, int line)
{
	char s_num[4];
	char cmd[30] = "MODE CON COLS=";
	int_str(s_num, col);
	strcat(cmd, s_num);
	int_str(s_num, line);
	strcat(cmd, " LINES=");
	strcat(cmd, s_num);
	system(cmd);
}

int cmp_arr(int a[][MAX_N], int b[][MAX_N])
{
	int i, j;
	for (i = n - 1; i >= 0; i--)
	{
		for (j = n - 1; j >= 0; j--)
		{
			if (a[i][j] != b[i][j]) return 0;
		}
	}
	return 1;
}

void copy_arr(int arr_copy[][MAX_N], int arr[][MAX_N])
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			arr_copy[i][j] = arr[i][j];
		}
	}
}

void copy_thongtin(thongtin * a, thongtin b)
{
	copy_arr(a->map , b.map);
	a->cursor = b.cursor;
	a->index = b.index;
	a->parent = b.parent;
}

void print_menu(int key, const char menu[][35], int menu_size)
{
	int i;
//	system("cls");
	gotoxy(0, 0);
	for (i = 0; i < menu_size; i++)
	{
		if (i == key)
		{
			textcolor(YELLOW);
			printf(">> ");
		}
		else
		{
			textcolor(BLUE);
			printf("   ");
		}
		puts(menu[i]);
	}
}

int menu_process(const char menu[][35], int menu_size) // return key
{
	set_console_size(MENU_MAX_X, MENU_MAX_Y);
	system("cls");
	int key = 0;
	char c = ' ';
	print_menu(0, menu, menu_size);
	while (c != 13)
	{
		c = getch();
		switch (c)
		{
			case 72:
				key--;
				break;
			case 80:
				key++;
				break;
		}
		if (key < 0) key = menu_size - 1;
		if (key > menu_size - 1) key = 0;
		print_menu(key, menu, menu_size);
	}

	return key;
}

int game_mode()
{
	const char GAME_MODE[][35] = {"Mode 1: use move key and enter", "Mode 2: only use move key"};
	return menu_process(GAME_MODE, sizeof(GAME_MODE) / sizeof(GAME_MODE[0]));
}

int play_again()
{
	const char PLAY_AGAIN[][35] = {"Play again", "Solve", "Back to menu", "Quit"};
	return menu_process(PLAY_AGAIN, sizeof(PLAY_AGAIN) / sizeof(PLAY_AGAIN[0]));
}

int play_with_newMap()
{
	const char ASK[][35] = {"New map", "Continue with the last map"};
	return menu_process(ASK, sizeof(ASK) / sizeof(ASK[0]));
}

void readMap()
{
	FILE * file = fopen("sinh_test\\testCase.txt", "r");
	fscanf(file, "%d", &n);
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			fscanf(file, "%d", &mapdau[i][j]);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			fscanf(file, "%d", &mapcuoi[i][j]);
	fclose(file);
}

void write_lastMap_inFile(int map[][MAX_N], int write_map, int timePlay) // default: rows = cols = n
{
	// write_map == 1 --> ghi map vao file | write_map == 0 --> khong can ghi map
	char *fileName = create_fileName(SAVE_MAP_PATH, n);
	FILE * file = fopen(fileName, "w");

	fprintf(file, "%d %d\n", write_map, timePlay);

	if (write_map == 1)
	{
		int i, j;
		for (i = 0; i < n; i++){
			for (j = 0; j < n; j++)
				fprintf(file, "%2d ", map[i][j]);
			fprintf(file, "\n");
		}
	}

	fclose(file);
}

int read_lastMap_toContinue_inFile(int map[][MAX_N]) // default: rows = cols = n
{
	// return 1 --> file nay ton tai va co du lieu || return 0 --> file nay khong ton tai hoac khong co du lieu
	char *fileName = create_fileName(SAVE_MAP_PATH, n);
	FILE * file = fopen(fileName, "r");
	
	if (file == NULL) return 0; // file khong ton tai
	
	int tontai_file;
	fscanf(file, "%d %d\n", &tontai_file, &timeStart_fromLastMap);
	
	if (tontai_file == 0) // file khong co du lieu
	{
		fclose(file);
		return 0;
	}
	
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			fscanf(file, "%d", &map[i][j]);
			if (map[i][j] == 0)
			{
				space.x = j;
				space.y = i;
				vitri = space;
			}
		}
	}
	
	fclose(file);
	return 1;
}

int readScore(int map, int timePlay, int * bestTime) // 1 - ton tai file | 0 - khong ton tai file
{
	char *FileName = create_fileName(BEST_SCORE_PATH, map);

	// read file
	FILE * file = fopen(FileName, "r");
	int tontai = 0;

	if (file == NULL) // file khong ton tai --> tao file + ghi gia tri
	{
		ghiFile:;
		fclose(file);
		file = fopen(FileName, "w");
		fprintf(file, "%d", timePlay);
	}
	else // file ton tai --> doc file + so sanh gia tri + ghi gia tri
	{
		tontai = 1;
		fscanf(file, "%d", bestTime);
		if (*bestTime == INT_MAX) tontai = 0;
		if (*bestTime > timePlay)
		{
			*bestTime = timePlay;
			goto ghiFile;
		}
	}

	fclose(file);
	return tontai;
}

void show_score()
{
	set_console_size(MENU_MAX_X, MAX_N + 2);
	int map;
	system("cls");
	for (map = 3; map <= MAX_N; map++)
	{
		int bestTime;
		if (readScore(map, INT_MAX, &bestTime) == 1) // file dang doc ton tai
			printf("Map %02dx%02d  -----  Best score : %02d:%02d\n", map, map, bestTime/60, bestTime%60);
		else
			printf("Map %02dx%02d  -----  Best score :  ---\n", map, map);
	}
	printf("Press any key to go menu...");
	getch();
}

void chuyen_toado(int * x, int * y) // HEIGHT = 1
{
	int startX = getConsoleSize_Cols()/2 - (n*(WIDTH + 1) + 1)/2;
	int startY = getConsoleSize_Rows()/2 - (n*(HEIGHT + 1) + 1)/2;
	*x = startX + *x * (WIDTH + 1) + 2;
	*y = startY + *y * 2 + 1 - 3;
}

void print_text_in_coord(int y, int x, const char text[]) // y == -1 || x == -1 --> align center
{
	if (x == -1)
	{
		x = getConsoleSize_Cols()/2 - strlen(text)/2;
	}
	if (y == -1)
	{
		y = getConsoleSize_Rows()/2;
	}
	gotoxy(x, y);
	puts(text);
}

void print_char(char kitu, int soluong)
{
	int i;
	for (i = 0; i < soluong; i++)
	{
		printf("%c", kitu);
	}
}

void print_border(int startY, int startX, int row, int col, int width, int color) // height = 1
{
	int hang, cot;
	textcolor(color);
	if (startX == -1 || startY == -1)
	{
		startX = startY = 0;
		chuyen_toado(&startX, &startY);
		startX -= 2;
		startY -= 1;
	}

	for (hang = 1; hang <= row*2 + 1; hang++)
	{
		gotoxy(startX, startY + hang - 1);
		for (cot = 1; cot <= col*2 + 1; cot++)
		{
			if (hang == 1 && cot % 2 == 1) // nga 3, hang 1
				printf("%c", 203); // 194 203
			else if (hang == 2*row + 1 && cot % 2 == 1) // nga 3, hang cuoi
				printf("%c", 202); // 193 202
			else if (cot == 1 && hang % 2 == 1) // nga 3, cot 1
				printf("%c", 204); // 195 204
			else if (cot == 2*col + 1 && hang % 2 == 1) // nga 3, cot cuoi
				printf("%c", 185); // 180 185
			else if (cot % 2 == 1 && hang % 2 == 0) // ngan cach 2 so, doc
				printf("%c", 186); // 179 186
			else if (hang % 2 == 1 && cot % 2 == 0) // ngan cach 2 so, ngang
				print_char(205, width); // 196 205
			else if (hang % 2 == 0 && cot % 2 == 0) // vi tri so
				print_char(32, width);
			else
				printf("%c", 206); // nga 4  197 206
		}
		printf("\n");
	}
	// 4 goc
	gotoxy(startX, startY);
	printf("%c", 201); // 218 201
	gotoxy(startX, startY + row*2);
	printf("%c", 200); // 192 200
	gotoxy(startX + col*(width + 1), startY);
	printf("%c", 187); // 191 187
	gotoxy(startX + col*(width + 1), startY + row*2);
	printf("%c", 188); // 217 188
}

void loading()
{
	int startY = 3;
	set_console_size(134, 20 + startY);
	
	textcolor(15);
	print_text_in_coord(0 + startY, -1, "888b    888                8888888b.  888     888 8888888888P 8888888888P 888      8888888888\n");
	print_text_in_coord(1 + startY, -1, "8888b   888                888   Y88b 888     888       d88P        d88P  888      888       \n");
	textcolor(11);
	print_text_in_coord(2 + startY, -1, "88888b  888                888    888 888     888      d88P        d88P   888      888       \n");
	print_text_in_coord(3 + startY, -1, "888Y88b 888                888   d88P 888     888     d88P        d88P    888      8888888   \n");
	print_text_in_coord(4 + startY, -1, "888 Y88b888                8888888P\"  888     888    d88P        d88P     888      888       \n");
	textcolor(3);
	print_text_in_coord(5 + startY, -1, "888  Y88888     888888     888        888     888   d88P        d88P      888      888       \n");
	print_text_in_coord(6 + startY, -1, "888   Y8888                888        Y88b. .d88P  d88P        d88P       888      888       \n");
	print_text_in_coord(7 + startY, -1, "888    Y888                888         \"Y88888P\"  d8888888888 d8888888888 88888888 8888888888\n");
	
	
	
	textcolor(WHITE);
	int startX = (getConsoleSize_Cols() - 100) / 2;
	print_border(11 + startY, startX, 1, 1, 100, WHITE); // in khung theo format loading

	int i;
	for (i = 1; i <= 100; i++)
	{
		gotoxy(startX + i, 12 + startY);
		textcolor(GREEN);
		printf("%c", 219); // 221
		gotoxy((getConsoleSize_Cols() - 14) / 2, 15 + startY);
		textcolor(WHITE);
		printf("Loading : %d%%", i);
		Sleep(7);
	}
	print_text_in_coord(16 + startY, -1, "Press any key to start...");
	getch();
}

void hoanvi(int * a, int * b)
{
	int c = *a;
	*a = *b;
	*b = c;
}

void findValue(int value, int arr[][MAX_N], int *y, int *x)
{
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (value == arr[i][j])
			{
				*y = i;
				*x = j;
				return;
			}
}

void findValue2(int value, int map[][MAX_N], toado * start, toado * end, toado * cursor)
{
	findValue(0, map, &(cursor->y), &(cursor->x));
	findValue(value, map, &(start->y), &(start->x));
	findValue(value, mapcuoi, &(end->y), &(end->x));
}

int ktra_map()
{
	int i, j, spaceY, a[MAX_N*MAX_N];
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			a[i*n + j] = mapdau[i][j];
			if (mapdau[i][j] == 0)
			{
				spaceY = i + 1;
			}
		}
	}
	int dem = 0;
	for (i = 0; i < n*n - 1; i++)
	{
		if (a[i] == 0) continue;
		for (j = i + 1; j < n*n; j++)
		{
			if (a[j] == 0) continue;
			if (a[i] > a[j]) dem++;
		}
	}

	if (n % 2 == 1 && dem % 2 == 0) return 1;
	if (n % 2 == 0 && ((dem % 2) == (spaceY % 2))) return 1;
	return 0;
}

void create_map()
{
	int i, j, a[MAX_N * MAX_N];
	do
	{
		// khoi tao mang trung gian
		for (i = 0; i < n*n; i++)
			a[i] = i;

		// khoi tao map
		int a_size = n*n;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				int pos = rand() % a_size;
				mapdau[i][j] = a[pos];
				hoanvi(&a[pos], &a[a_size - 1]);
				a_size--;

				if (mapdau[i][j] == 0)
				{
					space.x = j;
					space.y = i;
				}
			}
		}
	}
	while (!ktra_map());
}

void print_number(int value, int x, int y, int color)
{
	chuyen_toado(&x, &y);
	gotoxy(x, y);

	textcolor(color);
	if (value == 0)
	{
		print_char(32, WIDTH - 2);
	}
	else
	{
		printf("%02d", value);
	}
}

void print_arr(int arr[][MAX_N], int status)
{
	int i, j, color = BLUE;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
//			int color = (vitri.x == j && vitri.y == i) ? GREEN : WHITE;
			color = (arr[i][j] == i*n + j + 1 ? RED : BLUE);
			print_number(arr[i][j], j, i, color);
		}
	}
	
	char s[50] = "Solving: ";
	switch(status)
	{
		case -1:
			strcat(s, ". . .");
			break;
		case 0:
			strcat(s, "Up   ");
			break;
		case 1:
			strcat(s, "Left ");
			break;			
		case 2:
			strcat(s, "Right");
			break;
		case 3:
			strcat(s, "Down ");
			break;
	}

	print_text_in_coord(getConsoleSize_Rows()/2 + (n*(HEIGHT + 1) + 1)/2, -1, s);
	Sleep(SPEED);
//	getch();
}

void print_map()
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			int color = (vitri.x == j && vitri.y == i) ? GREEN : BLUE;
			color = (mapdau[i][j] == i*n + j + 1 ? RED : color);
			print_number(mapdau[i][j], j, i, color);
		}
	}
}

//int play_with_lastMap()
//{
//	const char ASK[][35] = "";
//}

void init(int mode)
{
	system("cls");
	// choi lai map cu neu co
	int arr[MAX_N][MAX_N];
	if (read_lastMap_toContinue_inFile(arr) == 1)
	{
		switch (play_with_newMap())
		{
			case 0: // new map
				create_map();
				timeStart_fromLastMap = 0;
				break;
			case 1: // last map
				copy_arr(mapdau, arr);
				break;
		}
	}
	else create_map();

//	readMap(); // test case
	if (mode == 0) // use move key and enter to move number
	{
		vitri.x = vitri.y = 0;
	}
	else // only use move key to move number
	{
		findValue(0, mapdau, &vitri.y, &vitri.x);
	}
	
	set_console_size(SCREEN_MAX_X, SCREEN_MAX_Y);
	print_border(-1, -1, n, n, WIDTH, BLUE); // in khung theo format map
	print_map();
	screenX = getConsoleSize_Cols();
	screenY = getConsoleSize_Rows();
}

void tinhtien(int moveX, int moveY, int dodoi)
{
	int x = space.x;
	int y = space.y;
	int i;
	for (i = 1; i <= dodoi; i++)
	{
		hoanvi(&mapdau[y][x], &mapdau[y + moveY][x + moveX]);
		x += moveX;
		y += moveY;
		print_map();
		Sleep(SPEED_PULL_NUM);
	}
	space.x = x;
	space.y = y;
}

void enterClick()
{
	if (vitri.x != space.x && vitri.y != space.y) return; // khong cung hang hay cot
	if (mapdau[vitri.y][vitri.x] == 0) return;

	int moveY[] = {-1, 0, 0, 1};
	int moveX[] = {0, -1, 1, 0};
	// tim cach di tot nhat trong 4 cach di
	int i, bestSolution = -1, min = INT_MAX;
	for (i = 0; i < 4; i++)
	{
		int khoangCach = abs(vitri.x - (space.x + moveX[i])) + abs(vitri.y - (space.y + moveY[i]));
		if (bestSolution == -1 || min > khoangCach)
		{
			bestSolution = i;
			min = khoangCach;
		}
	}
	tinhtien(moveX[bestSolution], moveY[bestSolution], min + 1);
}

int ktra_ketthuc()
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i == n - 1 && j == n - 1)
			{
				return 1;
			}
			if (mapdau[i][j] != n*i + j + 1)
			{
				return 0;
			}
		}
	}
	return 1;
}

int pause_game()
{
	system("cls");
	set_console_size(50, 5);
	print_text_in_coord(-1, -1, "PAUSE GAME - CLICK ANY KEY TO CONTINUE");
	int pauseStart = time(NULL);
	getch();
	system("cls");
	set_console_size(SCREEN_MAX_X, SCREEN_MAX_Y);
	print_border(-1, -1, n, n, WIDTH, BLUE); // in khung theo format map
	print_map();
	return time(NULL) - pauseStart;
}

void event_changeConsoleSize()
{
	system("cls");
	screenX = getConsoleSize_Cols();
	screenY = getConsoleSize_Rows();
	print_border(-1, -1, n, n, WIDTH, BLUE); // in khung theo format map
	print_map();
}

void play_game()
{
	// chon che do choi
	int mode = game_mode();
		
//	set_console_size(SCREEN_MAX_X, SCREEN_MAX_Y);
	init(mode);
	int timeStart = time(NULL);
	int timePause = 0, timePlay;
	
	while (1)
	{
		if (kbhit())
		{
			char c = getch();

			int color = (mapdau[vitri.y][vitri.x] == vitri.y*n + vitri.x + 1 ? RED : BLUE);
			print_number(mapdau[vitri.y][vitri.x] ,vitri.x, vitri.y, color); // xoa trang thai cu

			switch (c)
			{
				case 13:
					event_enterClick:;
					enterClick();
					if (ktra_ketthuc())
					{
						timePlay = time(NULL) - timeStart - timePause + timeStart_fromLastMap;
						int bestTime, startX = n - 1, startY = n - 1;
						readScore(n, timePlay, &bestTime);
						chuyen_toado(&startX, &startY);
						print_text_in_coord(startY + 4, -1, "YOU WIN");
						gotoxy(getConsoleSize_Cols()/2 - 9, startY + 5);
						printf("Time play  : %02d:%02d\n", timePlay/60, timePlay%60);
						gotoxy(getConsoleSize_Cols()/2 - 9, startY + 6);
						printf("Best score : %02d:%02d", bestTime/60, bestTime%60);
						write_lastMap_inFile(mapdau, 0, 0); // delete map in file
						goto out_loop;
					}
					if (mode == 1) goto finish_event_enterClick;
					break;
				case '0': // save map and return menu	
					write_lastMap_inFile(mapdau, 1, time(NULL) - timeStart - timePause + timeStart_fromLastMap);
					goto out_loop;
				case 72:
					vitri.y--;
					break;
				case 75:
					vitri.x--;
					break;
				case 77:
					vitri.x++;
					break;
				case 80:
					vitri.y++;
					break;
				case 32:
				case 'p':
					timePause += pause_game();
					break;
			}

			if (mode == 0)
			{
				if (vitri.x < 0) vitri.x = n - 1;
				if (vitri.y < 0) vitri.y = n - 1;
				if (vitri.x > n - 1) vitri.x = 0;
				if (vitri.y > n - 1) vitri.y = 0;
			}
			else
			{
				if (vitri.x < 0) vitri.x = 0;
				if (vitri.y < 0) vitri.y = 0;
				if (vitri.x > n - 1) vitri.x = n - 1;
				if (vitri.y > n - 1) vitri.y = n - 1;
				goto event_enterClick;
			}
			finish_event_enterClick:;

			print_number(mapdau[vitri.y][vitri.x] ,vitri.x, vitri.y, GREEN); // in trang thai moi
		}
		
		if (screenX != getConsoleSize_Cols() || screenY != getConsoleSize_Rows())
		{
			event_changeConsoleSize();
		}
		
		int startX = n - 1, startY = n - 1;
		timePlay = time(NULL) - timeStart - timePause + timeStart_fromLastMap;
		textcolor(BLUE);
		chuyen_toado(&startX, &startY);
		gotoxy(getConsoleSize_Cols()/2 - 15, startY + 3);
		printf("Cursor: (%d,%d)  ---  Time: %02d:%02d", vitri.y, vitri.x, timePlay/60, timePlay%60);
		gotoxy(0, 0);
		Sleep(44);
	}
	out_loop:;
	
	
	getch();
}

void option()
{
	set_console_size(50, 5);
	system("cls");
	printf("Map %02dx%02d - %02d numbers         %c n=%2d %c", n, n, n*n - 1, 174, n, 175);
	char c = ' ';
	while (c != 13)
	{
		c = getch();
		switch (c)
		{
			case 75:
				n--;
				break;
			case 77:
				n++;
				break;
		}
		if (n < 3) n = 3;
		if (n > MAX_N) n = MAX_N;
		gotoxy(0, 0);
		printf("Map %02dx%02d - %02d numbers         %c n=%2d %c", n, n, n*n - 1, 174, n, 175);
	}
}

//=================================================================================


void dichuyen_huong(int map[][MAX_N], int *y, int *x, int status)
{
	int y2 = *y + move[status][0];
	int x2 = *x + move[status][1];
	hoanvi(&map[*y][*x], &map[y2][x2]);
	print_arr(map, status);
	*y = y2;
	*x = x2;
}

void bfs_fromStart_toEnd(toado start, toado end, int const_value) // di chuyen tu start den end && khong di chuyen den cac gia tri <= const_value
{
	// init -- push_back mo
	mo.n = 0;
	copy_arr(mo.a[0].map, mapdau);
	mo.a[0].index = 0;
	mo.a[0].parent = -1;
	mo.n++;
	int first = 0;
	int timthay = 0;
	mo.a[0].cursor = start; // list.thongtin[].cursor = start

	// bfs
	while (first < mo.n)
	{
		thongtin infor1;
		copy_thongtin(&infor1, mo.a[first]);
		first++;

		// sinh huong di chuyen
		int i;
		for (i = 0; i < 4; i++)
		{
			thongtin infor2;
			copy_thongtin(&infor2, infor1);

			int y1 = infor2.cursor.y;
			int x1 = infor2.cursor.x;
			int y2 = y1 + move[i][0];
			int x2 = x1 + move[i][1];

			if (x2 < 0 || y2 < 0 || x2 >= n || y2 >= n) continue; // out of range
			if (infor2.map[y2][x2] <= const_value) continue; // di chuyen den vi tri hang so
			if (y2 == end.y && x2 == end.x)
			{
				hoanvi(&(infor2.map[y1][x1]), &(infor2.map[y2][x2]));
				copy_arr(mapdau, infor2.map);
				timthay = 1;
				goto add_element;
			}

			hoanvi(&(infor2.map[y1][x1]), &(infor2.map[y2][x2]));

			int k;
			for (k = 0; k < mo.n; k++)
			{
				if (cmp_arr(mo.a[k].map, infor2.map)) goto continue_loop;
			}

			// add element
		add_element:;
			infor2.cursor.x = x2;
			infor2.cursor.y = y2;
			infor2.index = mo.n;
			infor2.parent = infor1.index;
//			printf("index = %d ___ parent = %d\n", infor2.index, infor2.parent);
//			print_arr(infor2.map);

			copy_thongtin(&(mo.a[mo.n]), infor2);
			(mo.n)++;

			if (timthay)
			{
//				printf("size = %d\n", mo.n);
				return;
			}

		continue_loop:;
		}
	}

	int startX = n - 1, startY = n - 1;
	chuyen_toado(&startX, &startY);
	print_text_in_coord(startY + 4, -1, "can't solve this map");
	getch();
	exit(101);
}

int get_status(int y1, int x1, int y2, int x2)
{
	int i;
	for (i = 0; i < 4; i++)
		if (y2 - y1 == move[i][0] && x2 - x1 == move[i][1])
			return i;
	return -1;
}

void truyvet(thongtin infor, List L) // chieu xuoi
{
	if (infor.parent == -1)
	{
//		print_arr(infor.map);
		return;
	}
	thongtin infor2;
	copy_thongtin(&infor2, L.a[infor.parent]);
	truyvet(infor2, L);
	print_arr(infor.map, get_status(infor2.cursor.y, infor2.cursor.x, infor.cursor.y, infor.cursor.x));
}

int L_status(int value, int map[][MAX_N])
{
	toado start, end, cursor;
	findValue2(value, map, &start, &end, &cursor);

	if (start.y == cursor.y && start.x == cursor.x + 1 && start.y == end.y + 1 && start.x == end.x - 1)
	{
		// do something
		int solve[] = {UP, RIGHT, DOWN, RIGHT, UP, LEFT, LEFT, DOWN}; //up right down right up left left down
		int i = 0;
		for (i = 0; i < sizeof(solve) / sizeof(solve[0]); i++)
			dichuyen_huong(map, &cursor.y, &cursor.x, solve[i]);
		return 1;
	}
	return 0;
}

int _0_corner(int value, int map[][MAX_N])
{
	toado start, end, cursor;
	findValue2(value, map, &start, &end, &cursor);

	if (start.y == cursor.y && start.x == cursor.x + 1 && start.y == end.y + 1 && start.x == end.x && end.x != n - 1)
	{
		int solve[] = {UP, RIGHT, DOWN, RIGHT, UP, LEFT, LEFT, DOWN, RIGHT, UP, RIGHT}; //up right down right up left left down right up right
		int i;
		for (i = 0; i < sizeof(solve) / sizeof(solve[0]); i++)
			dichuyen_huong(map, &cursor.y, &cursor.x, solve[i]);
		return 1;
	}
	return 0;
}

int _1_corner(int value, int map[][MAX_N])
{
	toado start, end, cursor;
	findValue2(value, map, &start, &end, &cursor);

	if (start.y == cursor.y && start.x == cursor.x + 1 && start.y == end.y + 1 && start.x == end.x && end.x >= 2)
	{
		int solve[] = {LEFT, UP, RIGHT, RIGHT, DOWN, LEFT, UP, LEFT, DOWN}; //left up right right down left up left down
		int i;
		for (i = 0; i < sizeof(solve) / sizeof(solve[0]); i++)
			dichuyen_huong(map, &cursor.y, &cursor.x, solve[i]);
		return 1;
	}
	return 0;
}

int kepgoc_saunaydoiten(int value, int map[][MAX_N])
{
	toado start, end, cursor;
	findValue2(value, map, &start, &end, &cursor);

	if (start.y == cursor.y && start.x == cursor.x + 1 && start.y == end.y + 1 && end.x - start.x >= 2)
	{
		// dua value ve dung vi tri
		int i, d = end.x - cursor.x, d2 = d;
		while (1)
		{
			// move up
			dichuyen_huong(map, &cursor.y, &cursor.x, UP);
			// move right d times
			for (i = 1; i <= d; i++)
				dichuyen_huong(map, &cursor.y, &cursor.x, RIGHT);
			// move down
			dichuyen_huong(map, &cursor.y, &cursor.x, DOWN);
			// move left d-1 times
			for (i = 1; i <= d - 1; i++)
				dichuyen_huong(map, &cursor.y, &cursor.x, LEFT);

			if (d == 1) break;
			d--;
		}
		// tra trang thai cho cac phan tu phia truoc value
		dichuyen_huong(map, &cursor.y, &cursor.x, LEFT);
		dichuyen_huong(map, &cursor.y, &cursor.x, UP);
		while (1)
		{
			for (i = 1; i <= d; i++)
				dichuyen_huong(map, &cursor.y, &cursor.x, LEFT);

			dichuyen_huong(map, &cursor.y, &cursor.x, DOWN);

			for (i = 1; i <= d; i++)
				dichuyen_huong(map, &cursor.y, &cursor.x, RIGHT);

			if (d == d2 - 1) break;
			dichuyen_huong(map, &cursor.y, &cursor.x, UP);
			d++;
		}
		return 1;
	}
	return 0;
}

void cursor_kepgiua(int value, int map[][MAX_N])
{
	toado start, end, cursor;
	findValue2(value, map, &start, &end, &cursor);

	if (cursor.y == start.y && cursor.x > start.x && cursor.y == end.y + 1 && cursor.x <= end.x)
	{
		while (start.x < cursor.x)
		{
			dichuyen_huong(map, &cursor.y, &cursor.x, LEFT);
		}
	}
}

int isValid(int y, int x, int status, int exception_value)
{
	if (status != -1)
	{
		y += move[status][0];
		x += move[status][1];
	}
	if (y < 0 || n <= y || x < 0 || n <= x) return 0;
	if (exception_value != -1 && mapdau[y][x] <= exception_value) return 0;
	return 1;
}

void moveCursor_nearNumber(toado start, toado end, int status) // move cursor(start.y, start.x) to NEAR end(end.y, end.x)
{
	if (abs(start.y - end.y) + abs(start.x - end.x) == 1) return;

	if (status != -1)
		dichuyen_huong(mapdau, &start.y, &start.x, status);

	if (start.x < end.x && isValid(start.y, start.x, RIGHT, mapdau[end.y][end.x]))
		moveCursor_nearNumber(start, end, RIGHT);
	else if (start.x > end.x && isValid(start.y, start.x, LEFT, mapdau[end.y][end.x]))
		moveCursor_nearNumber(start, end, LEFT);
	else if (start.y < end.y && isValid(start.y, start.x, DOWN, mapdau[end.y][end.x]))
		moveCursor_nearNumber(start, end, DOWN);
	else if (start.y > end.y && isValid(start.y, start.x, UP, mapdau[end.y][end.x]))
		moveCursor_nearNumber(start, end, UP);
}

void move_cursor(toado start, toado end) // call bfs function to solve: from cursor(y, x) to end(y, x) and don't move const_value
{
	toado cursor;
	findValue(0, mapdau, &cursor.y, &cursor.x);

	// solve: from cursor to end and dont move constant - bfs
	bfs_fromStart_toEnd(cursor, end, mapdau[start.y][start.x]);

	// solve: trace path
	truyvet(mo.a[mo.n - 1], mo);

	// solve: swap start to end
	hoanvi(&mapdau[start.y][start.x], &mapdau[end.y][end.x]);
	print_arr(mapdau, get_status(end.y, end.x, start.y, start.x));
}

void solve_number(toado start, toado end, int status) // move start(y, x) to end(y, x), step by step
{
	if (status != -1) // update start coord
	{
		// nguyen nhan tim duong that bai: duong di tu cursor den number qua dai
		toado start2 = {start.y + move[status][0], start.x + move[status][1]};
		toado cursor;
		findValue(0, mapdau, &cursor.y, &cursor.x);
		moveCursor_nearNumber(cursor, start, -1);
		move_cursor(start, start2);
		start = start2;
	}

	if (start.x < end.x) // move right
	{
		solve_number(start, end, RIGHT);
	}
	else if (start.x > end.x) // move left
	{
		solve_number(start, end, LEFT);
	}
	else if (start.y < end.y) // move down
	{
		solve_number(start, end, DOWN);
	}
	else if (start.y > end.y) // move up
	{
		solve_number(start, end, UP);
	}
}

int mo_size;
void dfs(int map[][MAX_N], int last_map[][MAX_N], int dosau, int dosaugioihan)
{
	copy_arr(mo.a[dosau].map, map); // trace

	if (cmp_arr(map, last_map)) // compare current map vs last map
	{
		mo_size = dosau;
		return;
	}

	if (dosau > dosaugioihan) return;
	int i, y1, x1, y2, x2;
	findValue(0, map, &y1, &x1);

	for (i = 0; i < 4; i++)
	{
		y2 = y1 + move[i][0];
		x2 = x1 + move[i][1];

		// out of range
		if (y2 < 0 || x2 < 0 || y2 >= n || x2 >= n) continue;
		if (n > 3 && (y2 < n - 2 || x2 < (n - 1) / 2 + (n - 1) % 2)) continue;

		// create new status
		hoanvi(&map[y1][x1], &map[y2][x2]);

		// compare same status
		int k;
		for (k = 0; k <= dosau; k++)
		{
			if (cmp_arr(mo.a[k].map, map))
			{
				// destroy current status and continue
				hoanvi(&map[y1][x1], &map[y2][x2]);
				goto continue_loop;
			}
		}

		// call dfs
		dfs(map, last_map, dosau + 1, dosaugioihan);

		// destroy current status
		hoanvi(&map[y1][x1], &map[y2][x2]);

		if (mo_size != 0) return; // status finish

continue_loop:
		;
	}
}

void ids_final_problem(int last_map[][MAX_N]) // from mapdau become last_map
{
	if (cmp_arr(mapdau, last_map)) return;
	mo_size = 0;
	int dosaugioihan = 0, startX = n - 1, startY = n - 1;
	chuyen_toado(&startX, &startY);

	print_text_in_coord(startY + 3, -1, "Wait for minute...");

	if (n == 9 || n == 10) dosaugioihan = 29;

	while (1)
	{
		// show do sau gioi han cua qua trinh tim kiem
		gotoxy(0, 0);
		printf("hMax = %d", dosaugioihan);
		dfs(mapdau, last_map, 0, dosaugioihan);
		if (mo_size != 0) break;
		dosaugioihan++;
	}

	print_text_in_coord(startY + 4, -1, "Press any key to cotinue solve...");
	gotoxy(0, 1);
	printf("h = %d", mo_size);

	getch();
	system("cls");
	print_border(-1, -1, n, n, WIDTH, BLUE); // in khung theo format map

	// trace
	int i;
	for (i = 0; i <= mo_size; i++)
	{
		print_arr(mo.a[i].map, get_status(mo.a[i - 1].cursor.y, mo.a[i - 1].cursor.x, mo.a[i].cursor.y, mo.a[i].cursor.x));
//		getch();
	}
}

void solve_last_line()
{
	int last_map[MAX_N][MAX_N];
	copy_arr(last_map, mapdau);

	int i, mid = (n - 1) / 2 + (n - 1) % 2; // first x in table 2
	int num = n*(n - 1) + 1;
	for (i = mid; i < n; i++)
		last_map[n - 1][i] = num++;
	for (i = n - 1; i >= mid; i--)
	{
		last_map[n - 2][i] = num;
		if (num == n*n) last_map[n - 2][i] = 0;
		num++;
	}
	if (n % 2 == 1) last_map[n - 2][mid] = n*(n - 1);

	ids_final_problem(last_map);
	copy_arr(mapdau, last_map); // return status
}

void solve_2_lastLine()
{
	// solve line n-2
	int i;
	toado cursor, start, end;
	for (i = 0; i < n; i++)
	{
		int value = mapcuoi[n-2][i];

		findValue(0, mapdau, &cursor.y, &cursor.x);
		findValue(value, mapdau, &start.y, &start.x);
		findValue(value, mapcuoi, &end.y, &end.x);
		if (start.x == i && start.y == n-2) continue;

		if (cursor.y == end.y && cursor.x == end.x && cursor.y + 1 == start.y && cursor.x == start.x)
		{
			hoanvi(&mapdau[start.y][start.x], &mapdau[end.y][end.x]);
			print_arr(mapdau, get_status(start.y, start.x, end.y, end.x));
			continue;
		}

		moveCursor_nearNumber(cursor, start, -1);

		findValue(0, mapdau, &cursor.y, &cursor.x);
		cursor_kepgiua(value, mapdau);

		// special status
		if (L_status(value, mapdau)) continue;
		if (_0_corner(value, mapdau)) continue;
		if (_1_corner(value, mapdau)) continue;
		if (kepgoc_saunaydoiten(value, mapdau)) continue;

		// normal status
		findValue(value, mapdau, &start.y, &start.x);
		findValue(value, mapcuoi, &end.y, &end.x);
		solve_number(start, end, -1);
	}

	// solve last line
	// left - up - right - down
	// di chuyen tuan hoan chieu xuoi
	findValue(0, mapdau, &cursor.y, &cursor.x);
	while (1)
	{
		while (!(cursor.x == 0))
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, LEFT);

		dichuyen_huong(mapdau, &cursor.y, &cursor.x, UP);

		while (!(cursor.x == n - 1))
		{
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, RIGHT);
			if (mapdau[n - 2][(n - 1)/2] == n*(n-1)) goto exit_while_xuoi;
		}

		dichuyen_huong(mapdau, &cursor.y, &cursor.x, DOWN);
	}
exit_while_xuoi:
	;

	// giai ben phai
	solve_last_line();

	// di chuyen tuan hoan chieu nguoc --> finish
	// left - down - right - up
	findValue(0, mapdau, &cursor.y, &cursor.x);
	while(1)
	{
		while (!(cursor.x == 0))
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, LEFT);

		dichuyen_huong(mapdau, &cursor.y, &cursor.x, DOWN);

		while (!(cursor.x == n - 1))
		{
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, RIGHT);
			if (mapdau[n - 2][0] == n*(n - 2) + 1 && mapdau[n - 1][n - 1] == 0) goto exit_while_nguoc;
		}

		dichuyen_huong(mapdau, &cursor.y, &cursor.x, UP);
	}
exit_while_nguoc:
	;
}

void create_mapcuoi()
{
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			mapcuoi[i][j] = i*n + j + 1;
	mapcuoi[n - 1][n - 1] = 0;
}

void solve_map()
{
	system("cls");
	set_console_size(SCREEN_MAX_X, SCREEN_MAX_Y);
	print_border(-1, -1, n, n, WIDTH, BLUE); // in khung theo format map
	print_arr(mapdau, -1);
	create_mapcuoi();

	Sleep(1000);

	if (n == 3)
	{
		ids_final_problem(mapcuoi);
		return;
	}

	int i, j;
	toado start;
	// xac dinh (start(y, x) in mapdau) va (end(y, x) in mapdau = start(y, x) in mapcuoi)
	// n-2 first lines
	for (i = 0; i < n - 2; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (mapcuoi[i][j] == mapdau[i][j]) continue;

			toado end, cursor;
			findValue2(mapcuoi[i][j], mapdau, &start, &end, &cursor);
			if (cursor.y == end.y && cursor.x == end.x && end.y + 1 == start.y && end.x == start.x)
			{
				hoanvi(&mapdau[start.y][start.x], &mapdau[end.y][end.x]);
				print_arr(mapdau, get_status(start.y, start.x, end.y, end.x));
				continue;
			}

			if (j == n - 1)	// 1-corner status
			{
				// dua so ve dung vi tri
				findValue(mapcuoi[i][j], mapdau, &start.y, &start.x);
				toado end = {i + 1, j};
				solve_number(start, end, -1);

				// dua cursor ve dung vi tri - chuyen tu cursor(y, x) den (i + 1, j - 1), const_value = mapcuoi[i][j]
//				printf("finish\n");
				start.x = j;
				start.y = i + 1;
				end.x = j - 1;
				end.y = i + 1;
				move_cursor(start, end);
				hoanvi(&mapdau[start.y][start.x], &mapdau[end.y][end.x]);
				print_arr(mapdau, get_status(start.y, start.x, end.y, end.x));

				// ap dung
//				thuc_thi_corner(i + 1, j - 1);
				_1_corner(mapcuoi[i][j], mapdau);
			}
			else // normal status - move start to end
			{
				findValue(mapcuoi[i][j], mapdau, &start.y, &start.x);
				toado end = {i, j};
				solve_number(start, end, -1);
			}
		}
	}

	// 2 last lines
	solve_2_lastLine();
}

//=================================================================================

int main()
{
	srand(time(NULL));
	loading();
	system("COLOR 71");
	home:;
	int key = menu_process(MENU, MENU_SIZE);
	switch (key)
	{
		case 0: // play game
			again:;
			play_game();
			switch (play_again())
			{
				case 0: // play again
					goto again;
					break;
				case 1: // solve map
					solve_map();
					write_lastMap_inFile(mapdau, 0, 0);

					int startX = n - 1, startY = n - 1;
					chuyen_toado(&startX, &startY);
					textcolor(GREEN);
					print_text_in_coord(getConsoleSize_Rows()/2 + (n*(HEIGHT + 1) + 1)/2, -1, "Solving: Complete!");
					getch();

					goto home;
					break;
				case 2: // back to menu
					goto home;
					break;
				case 3: // quit
					break;
			}
			break;
		case 1: // option
			option();
			goto home;
			break;
		case 2: // show score
			show_score();
			goto home;
			break;
		case 3: // quit
			break;
	}
	return 0;
}
