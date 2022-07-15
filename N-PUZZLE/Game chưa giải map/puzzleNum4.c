#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
//#include <dos.h>
//#include <dir.h>

#define PATH "BestScore"
#define YELLOW 14
#define GREEN 10
#define RED 12
#define WHITE 15
#define WIDTH 4
#define MAX_N 10
#define SPEED 100

struct toado {
	int x, y;
};
typedef struct toado TOADO;

const char MENU[][35] = {"Play game", "Option", "Show score", "Quit"};
int MENU_SIZE = sizeof(MENU) / sizeof(MENU[0]);
int map[MAX_N][MAX_N];
int n = 3; // default
TOADO vitri, space;

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

void print_char(char kitu, int soluong)
{
	int i;
	for (i = 0; i < soluong; i++)
	{
		printf("%c", kitu);
	}
}

void print_border(int row, int col, int width) // height = 1
{
	int hang, cot;
	for (hang = 1; hang <= row*2 + 1; hang++)
	{
		for (cot = 1; cot <= col*2 + 1; cot++)
		{
			if (hang == 1 && cot % 2 == 1) // nga 3, hang 1
				printf("%c", 194);
			else if (hang == 2*row + 1 && cot % 2 == 1) // nga 3, hang cuoi
				printf("%c", 193);
			else if (cot == 1 && hang % 2 == 1) // nga 3, cot 1
				printf("%c", 195);
			else if (cot == 2*col + 1 && hang % 2 == 1) // nga 3, cot cuoi
				printf("%c", 180);
			else if (cot % 2 == 1 && hang % 2 == 0) // ngan cach 2 so, doc
				printf("%c", 179);
			else if (hang % 2 == 1 && cot % 2 == 0) // ngan cach 2 so, ngang
				print_char(196, width);
			else if (hang % 2 == 0 && cot % 2 == 0) // vi tri so
				print_char(32, width);
			else 
				printf("%c", 197); // nga 4
		}
		printf("\n");
	}
	// 4 goc
	gotoxy(0, 0);
	printf("%c", 218);
	gotoxy(0, row*2 );
	printf("%c", 192);
	gotoxy(col*(width + 1), 0);
	printf("%c", 191);
	gotoxy(col*(width + 1), row*2);
	printf("%c", 217);
}

void chuyen_toado(int * x, int * y) // HEIGHT = 1
{
	*x = *x * (WIDTH + 1) + 2;
	*y = *y * 2 + 1;
}

void hoanvi(int * a, int * b)
{
	int c = *a;
	*a = *b;
	*b = c;
}

int ktra_map()
{
	int i, j, a[MAX_N*MAX_N];
	int a_size = n*n;
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			a[i * n + j] = map[i][j];
			
	int s = 0, spaceY;
	for (i = 0; i < n*n; i++)
	{
		if (a[i] == 0) 
		{
			spaceY = i / 3 + 1;
			continue;
		}
		for (j = i + 1; j < n*n; j++)
		{
			if (a[j] == 0) continue;
			if (a[i] > a[j]) s++;
		}
	}
	
	if (n % 2 == 1)
	{
		if (s % 2 == 0) return 1;
	}
	else // n chan
	{
		if (s % 2 == 0 && spaceY % 2 == 0) return 1;
		if (s % 2 == 1 && spaceY % 2 == 1) return 1;
	}
	return 0;
}

void create_map()
{
	int i, j, a[MAX_N * MAX_N];
	do {
		// khoi tao mang trung gian
		for (i = 0; i < n*n; i++)
			a[i] = i;
		
		// khoi tao map 2 chieu
		int a_size = n*n;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				int pos = rand() % a_size;
				map[i][j] = a[pos];
				hoanvi(&a[pos], &a[a_size - 1]);
				a_size--;
				
				if (map[i][j] == 0)
				{
					space.x = j;
					space.y = i;
				}
			}
		}
	} while (!ktra_map());
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

void print_map()
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			int color = (vitri.x == j && vitri.y == i) ? GREEN : WHITE;
			print_number(map[i][j], j, i, color);
		}
	}
}

void init()
{
	system("cls");
	srand(time(NULL));
	vitri.x = vitri.y = 0;
	create_map();
	print_border(n, n, WIDTH);
	print_map();
}

void tinhtien(int moveX, int moveY, int dodoi)
{
	int x = space.x;
	int y = space.y;
	int i;
	for (i = 1; i <= dodoi; i++)
	{
		hoanvi(&map[y][x], &map[y + moveY][x + moveX]);
		x += moveX;
		y += moveY;
		print_map();
		Sleep(SPEED);
	}
	space.x = x;
	space.y = y;
}

//void enterClick()
//{
//	// 0-len  1-trai  2-phai  3-xuong
//	int moveY[] = {-1, 0, 0, 1};
//	int moveX[] = {0, -1, 1, 0};
//	int dodoi = abs(vitri.x - space.x) + abs(vitri.y - space.y);
//	
//	if (vitri.x == space.x) // cung cot
//	{
//		if (vitri.y < space.y) // tinh tien len tren
//		{
//			tinhtien(moveX[0], moveY[0], dodoi);
//		}
//		else // tinh tien xuong duoi
//		{
//			tinhtien(moveX[3], moveY[3], dodoi);
//		}
//	}
//	else
//	{
//		if (vitri.y == space.y) // cung hang
//		{
//			if (vitri.x < space.x) // tinh tien ve ben trai
//			{
//				tinhtien(moveX[1], moveY[1], dodoi);
//			}
//			else // tinh tien ve ben phai
//			{
//				tinhtien(moveX[2], moveY[2], dodoi);
//			}
//		}
//	}
//}

void enterClick()
{
	if (vitri.x != space.x && vitri.y != space.y) return; // khong cung hang hay cot
	if (map[vitri.y][vitri.x] == 0) return;
	
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
			if (map[i][j] != n*i + j + 1)
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
	printf("PAUSE GAME - CLICK ANY KEY TO CONTINUE");
	int pauseStart = time(NULL);
	getch();
	system("cls");
	print_border(n, n, WIDTH);
	print_map();
	return time(NULL) - pauseStart;
}

void insert_first(int num, char s[]) // chuyen num sang ki tu, them ki tu vao dau chuoi s
{
	int i = 0;
	while (s[i] != '\0') i++;
	int n = i;
	for (i = n; i > 0; i--)
		s[i] = s[i - 1];
	s[0] = num + '0';
}

char* int_str(int n) // chuyen tu so sang chuoi 
{
	char s[4] = "";
	while (n > 0)
	{
		int lastNum = n % 10;
		insert_first(lastNum, s);
		n /= 10;
	}
	return strdup(s);
}

int readScore(int map, int timePlay, int * bestTime) // 1 - ton tai file | 0 - khong ton tai file
{
	// create FileName string
	char * FileName = strdup(PATH);
	strcat(FileName, int_str(map));
	strcat(FileName, ".txt");
	
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

void play_game()
{
	init();
	int timeStart = time(NULL);
	int timePause = 0, timePlay;
	
	while (1)
	{
		if (kbhit())
		{
			char c = getch();
			
			print_number(map[vitri.y][vitri.x] ,vitri.x, vitri.y, WHITE); // xoa trang thai cu
			
			switch (c)
			{
				case 13:
					enterClick();
					if (ktra_ketthuc())
					{
						gotoxy(0, 2*n + 2);
						timePlay = time(NULL) - timeStart - timePause;
						int bestTime;
						readScore(n, timePlay, &bestTime);
						printf("YOU WIN\n");
						printf("Time play  : %02d:%02d\n", timePlay/60, timePlay%60);
						printf("Best score : %02d:%02d", bestTime/60, bestTime%60);
						goto out_loop;
					}
					break;
				case '0':
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
			
			if (vitri.x < 0) vitri.x = n - 1;
			if (vitri.y < 0) vitri.y = n - 1;
			if (vitri.x > n - 1) vitri.x = 0;
			if (vitri.y > n - 1) vitri.y = 0;

			print_number(map[vitri.y][vitri.x] ,vitri.x, vitri.y, GREEN); // in trang thai moi
		}
		gotoxy(0, 2*n + 1);
		timePlay = time(NULL) - timeStart - timePause;
		textcolor(WHITE);
		printf("Cursor: (%d,%d)  ---  Time: %02d:%02d", vitri.y, vitri.x, timePlay/60, timePlay%60);
	}
	out_loop:;
	getch();
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
			textcolor(WHITE);
			printf("   ");
		}
		puts(menu[i]);
	}
}

int menu_process(const char menu[][35], int menu_size) // return key
{
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

int play_again()
{
	const char PLAY_AGAIN[][35] = {"Play again", "Back to menu", "Quit"};
	return menu_process(PLAY_AGAIN, sizeof(PLAY_AGAIN) / sizeof(PLAY_AGAIN[0]));
}

void option()
{
	system("cls");
	printf("Map %02dx%02d - %02d numbers         %c n=%02d %c", n, n, n*n - 1, 174, n, 175);
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
		printf("Map %02dx%02d - %02d numbers         %c n=%02d %c", n, n, n*n - 1, 174, n, 175);
	}
}

void show_score()
{
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

void loading()
{
	system("MODE CON COLS=102 LINES=6");
	print_border(1, 1, 100);
	
	int i;
	for (i = 1; i <= 100; i++)
	{
		gotoxy(i, 1);
		textcolor(GREEN);
		printf("%c", 219); // 221
		gotoxy(45, 3);
		textcolor(WHITE);
		printf("Loading : %d%%", i);
		Sleep(10);
	}
	gotoxy(40, 4);
	printf("Press any key to start...");
	getch();
}

int main()
{
	loading();	
	
	system("MODE CON COLS=100 LINES=40");
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
				case 1: // back to menu
					goto home;
					break;
				case 2: // quit
					goto out_game;
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
			goto out_game;
			break;
	}
	
	out_game:;
	gotoxy(0, 2*n + 3);
	printf("\nket thuc game !!!");
	return 0;
}
