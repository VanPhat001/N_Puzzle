#include <stdio.h>
#include <windows.h>

#define UP 0
#define LEFT 1
#define RIGHT 2
#define DOWN 3
#define maxN 10
#define SPEED 0
#define WHITE 15
#define GREEN 10

typedef struct {
	int y, x;
} toado;

typedef struct {
	toado cursor;
	int map[maxN][maxN];
	int parent;
	int index;
} thongtin;

typedef struct {
	thongtin a[500];
	int n;
} List;

int move[][2] = { {-1, 0}, {0, -1}, {0, 1}, {1, 0} };

List mo;
int mapdau[maxN][maxN], mapcuoi[maxN][maxN];
int n;

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

void print_arr(int arr[][maxN])
{
	int i, j;
	system("cls");
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (arr[i][j] == 0) textcolor(GREEN);
			printf("%2d ", arr[i][j]);
			textcolor(WHITE);
		}
		printf("\n");
	}
	printf("\n");
	Sleep(SPEED);
//	getch();
}

int cmp_arr(int a[][maxN], int b[][maxN])
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j< n; j++)
		{
			if (a[i][j] != b[i][j]) return 0;
		}
	}
	return 1;
}

void copy_arr(int arr_copy[][maxN], int arr[][maxN])
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

void readData()
{
	FILE * file = fopen("testCase.txt", "r");
	fscanf(file, "%d", &n);
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			fscanf(file, "%d", &mapdau[i][j]);
		}
	}
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			fscanf(file, "%d", &mapcuoi[i][j]);
		}
	}
	fclose(file);
}

void findValue(int value, int arr[][maxN], int *y, int *x)
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

void findValue2(int value, int map[][maxN], toado * start, toado * end, toado * cursor)
{
	findValue(0, map, &(cursor->y), &(cursor->x));
	findValue(value, map, &(start->y), &(start->x));
	findValue(value, mapcuoi, &(end->y), &(end->x));	
}

void hoanvi(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void dichuyen_huong(int map[][maxN], int *y, int *x, int status)
{
	int y2 = *y + move[status][0];
	int x2 = *x + move[status][1];
	hoanvi(&map[*y][*x], &map[y2][x2]);
	print_arr(map);
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
	printf("can't find this state");
	getch();
	exit(101);
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
	print_arr(infor.map);
}

int L_status(int value, int map[][maxN])
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

int _0_corner(int value, int map[][maxN])
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

int _1_corner(int value, int map[][maxN])
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

int kepgoc_saunaydoiten(int value, int map[][maxN])
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

void cursor_kepgiua(int value, int map[][maxN])
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
	print_arr(mapdau);
}

void solve_number(toado start, toado end, int status) // move start(y, x) to end(y, x), step by step  
{
	if (status != -1) // update start coord
	{ // nguyen nhan tim duong that bai: duong di tu cursor den number qua dai
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
void dfs(int map[][maxN], int last_map[][maxN], int dosau, int dosaugioihan)
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
		
		continue_loop:;
	}
}

void ids_final_problem(int last_map[][maxN]) // from mapdau become last_map
{
	if (cmp_arr(mapdau, last_map)) return;
	mo_size = 0;
	int dosaugioihan = 0;
	printf("wait for minute...\n");
	while (1)
	{
		dfs(mapdau, last_map, 0, dosaugioihan);
		if (mo_size != 0) break;
		dosaugioihan++;
	}
	
	printf("press any key to cotinue solve...\n");
	getch();
	
	// trace
	int i;
	for (i = 0; i <= mo_size; i++)
	{
		print_arr(mo.a[i].map);
		getch();
	}
}

void solve_last_line()
{
	int last_map[maxN][maxN];
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
	copy_arr(mapdau, last_map);
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
			print_arr(mapdau);
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
	exit_while_xuoi:;
	
	// giai ben phai
	solve_last_line();
	
	// di chuyen tuan hoan chieu nguoc --> finish
	// left - down - right - up
	findValue(0, mapdau, &cursor.y, &cursor.x);
	while(1)
	{
		while (!(cursor.x == 0))
		{
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, LEFT);
			getch();
		}
			
		dichuyen_huong(mapdau, &cursor.y, &cursor.x, DOWN);
		getch();
		
		while (!(cursor.x == n - 1))
		{
			dichuyen_huong(mapdau, &cursor.y, &cursor.x, RIGHT);
			getch();
			if (mapdau[n - 2][0] == n*(n - 2) + 1 && mapdau[n - 1][n - 1] == 0) goto exit_while_nguoc;
		}
		
		dichuyen_huong(mapdau, &cursor.y, &cursor.x, UP);
		getch();
	}
	exit_while_nguoc:;
}

void solve_map()
{
	if (n == 2 || n == 3)
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
			if (j == n - 1)	// 1-corner status
			{
				// dua so ve dung vi tri
				findValue(mapcuoi[i][j], mapdau, &start.y, &start.x);
				toado end = {i + 1, j};
				solve_number(start, end, -1);
				
				// dua cursor ve dung vi tri - chuyen tu cursor(y, x) den (i + 1, j - 1), const_value = mapcuoi[i][j]
				printf("finish\n");
				start.x = j;
				start.y = i + 1;
				end.x = j - 1;
				end.y = i + 1;
				move_cursor(start, end);
				hoanvi(&mapdau[start.y][start.x], &mapdau[end.y][end.x]);
				print_arr(mapdau);
				
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

int main()
{
	readData();
//	print_arr(mapdau);
	solve_map();
	return 0;
}
