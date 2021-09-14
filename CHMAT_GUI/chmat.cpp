#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "chmat.h"
#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a number */


int FillBMPInfo(BITMAPINFO* info, int x, int y, int bits)
{
	info->bmiHeader.biBitCount = bits;
	info->bmiHeader.biClrImportant = 0;
	info->bmiHeader.biClrUsed = 0;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biHeight = y;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biSize = sizeof(BITMAPINFO);
	info->bmiHeader.biSizeImage = x * y * bits / 8;
	info->bmiHeader.biWidth = x;
	info->bmiHeader.biXPelsPerMeter = 0;
	info->bmiHeader.biYPelsPerMeter = 0;
	return(0);
}



bool CHMAT::isMovable(int x, int y, int moveTo) {//checks the direction if it is moveable return true 

	if (moveTo == 1) {
		if (get(x + 39, y) == 0 && get(x + 39, y + 38) == 0 && get(x + 39, y + 20) == 0)
			return true;
	}
	else if (moveTo == 2) {
		if (get(x, y + 39) == 0 && get(x + 38, y + 39) == 0 && get(x + 20, y + 39) == 0)
			return true;
	}
	else if (moveTo == 3) {
		if (get(x - 1, y) == 0 && get(x - 1, y + 38) == 0 && get(x - 1, y + 20) == 0)
			return true;
	}
	else if (moveTo == 4) {
		if (get(x, y - 1) == 0 && get(x + 38, y - 1) == 0 && get(x + 20, y - 1) == 0)
			return true;
	}
	else {
		return false;
	}
	return false;
}


void CHMAT::ghostClear(CHMAT& checker, int ghostX, int ghostY, int ghostSize, int mode) {//clear the ghost from the board

	if (mode != 2) {
		Inverted_Right_Triangle_L(ghostX + 4, ghostY + 7, ghostSize - 32, 0);
		Inverted_Triangle(ghostX + 8, ghostY + 12, ghostSize - 16, 0);
		Inverted_Right_Triangle_R(ghostX + 28, ghostY + 7, ghostSize - 32, 0);
	}
	Rectangle(ghostX + 4, ghostY + 8, ghostSize - 8, ghostSize - 19, 0, true);
	Ellipse(ghostX + 4, ghostY + 7, ghostSize - 8, ghostSize - 8, 0, true);



	for (int i = 37; i < 40; i++)
	{
		if (checker.get(ghostX + i + 2, ghostY + 19) == 2) {
			drawPackmanFood(ghostX + i + 2, ghostY + 19, enmObjectType::smallFood, checker);
		}
		if (checker.get(ghostX + 19, ghostY + i + 2) == 2) {
			drawPackmanFood(ghostX + 19, ghostY + i + 2, enmObjectType::smallFood, checker);
		}
		if (checker.get(ghostX + i + 2, ghostY + 19) == 3) {
			drawPackmanFood(ghostX + i + 2, ghostY + 19, enmObjectType::largeFood, checker);
		}
		if (checker.get(ghostX + 19, ghostY + i + 2) == 3) {
			drawPackmanFood(ghostX + 19, ghostY + i + 2, enmObjectType::largeFood, checker);
		}
	}
	for (int i = -1; i < 2; i++)
	{
		if (checker.get(ghostX + i, ghostY + 19) == 2) {
			drawPackmanFood(ghostX + i, ghostY + 19, enmObjectType::smallFood, checker);
		}
		if (checker.get(ghostX + 19, ghostY + i) == 2) {
			drawPackmanFood(ghostX + 19, ghostY + i, enmObjectType::smallFood, checker);
		}
		if (checker.get(ghostX + i, ghostY + 19) == 3) {
			drawPackmanFood(ghostX + i, ghostY + 19, enmObjectType::largeFood, checker);
		}
		if (checker.get(ghostX + 19, ghostY + i) == 3) {
			drawPackmanFood(ghostX + 19, ghostY + i, enmObjectType::largeFood, checker);
		}
	}
}

CHMAT::CHMAT(int x, int y, int typ)
{
	if (typ == CHMAT_UNSIGNED_CHAR)
		data = calloc(x * y, sizeof(char));
	else if (typ == CHMAT_INT)
	{
		BITMAPINFO binfo;
		FillBMPInfo(&binfo, x, y, 32);
		HBitmap = CreateDIBSection(NULL, &binfo, DIB_RGB_COLORS, (void**)&data, NULL, NULL);
	}
	if (data != NULL)
	{
		X = x;
		Y = y;
		type = typ;
	}
}

CHMAT::~CHMAT()
{
	if (type == CHMAT_INT)
		DeleteObject(HBitmap);
	else free(data);
}

void CHMAT::clearPacman(int x, int y, int size) {
	Ellipse(x + 4, y + 4, size - 8, size - 8, 0, true);
}

template<typename T>
int CHMAT::set(int x, int y, T val)
{
	if (x<1 || x>X) return (-1);
	if (y<1 || y>Y) return (-1);

	if (type == CHMAT_UNSIGNED_CHAR)
		((unsigned char*)data)[(y - 1) * X + (x - 1)] = val;
	else if (type == CHMAT_INT)
		((int*)data)[(y - 1) * X + (x - 1)] = val;
	else if (type == CHMAT_UNSIGNED_INT)
		((unsigned int*)data)[(y - 1) * X + (x - 1)] = val;
	else if (type == CHMAT_DOUBLE)
		((double*)data)[(y - 1) * X + (x - 1)] = (double)val;
	return 0;
}


void CHMAT::myDraw(int* x, int* y, int numX, int numY, int color) {

	Line(*x, *y, (*x + numX), (*y + numY), color);
	*x += numX;
	*y += numY;
}



int CHMAT::pacmanEatFood(int x, int y, int moveTo) {
	int score = 0, holder = 0;

	switch (moveTo)
	{
	case 1:
		holder = get(x + 33, y + 19);
		if (holder == 2) {
			set(x + 33, y + 19, 0);
			score = 10;
		}
		else if (holder == 3) {
			set(x + 33, y + 19, 0);
			score = 50;
		}
		break;
	case 2:
		holder = get(x + 19, y + 33);
		if (holder == 2) {
			set(x + 19, y + 33, 0);
			score = 10;
		}
		else if (holder == 3) {
			set(x + 19, y + 33, 0);
			score = 50;
		}
		break;
	case 3:
		holder = get(x + 7, y + 19);
		if (holder == 2) {
			set(x + 7, y + 19, 0);
			score = 10;
		}
		else if (holder == 3) {
			set(x + 7, y + 19, 0);
			score = 50;
		}
		break;
	case 4:
		holder = get(x + 19, y + 7);
		if (holder == 2) {
			set(x + 19, y + 7, 0);
			score = 10;
		}
		else if (holder == 3) {
			set(x + 19, y + 7, 0);
			score = 50;
		}
		break;
	default:
		break;
	}


	return score;
}

void CHMAT::drawEdge(int color) {//draws the edge of the board

	int x = 10, y = 10;
	myDraw(&x, &y, 540, 0, color);
	myDraw(&x, &y, 0, 100, color);
	myDraw(&x, &y, -40, 0, color);
	myDraw(&x, &y, 0, 20, color);
	myDraw(&x, &y, 40, 0, color);
	myDraw(&x, &y, 0, 100, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, -0, 80, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, 40, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, 80, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, 180, color);
	myDraw(&x, &y, -260, 0, color);
	myDraw(&x, &y, 0, -80, color);
	myDraw(&x, &y, -20, 0, color);
	myDraw(&x, &y, 0, 80, color);
	myDraw(&x, &y, -260, 0, color);
	myDraw(&x, &y, 0, -180, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, -80, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, -40, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, -80, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, -100, color);
	myDraw(&x, &y, 40, 0, color);
	myDraw(&x, &y, 0, -20, color);
	myDraw(&x, &y, -40, 0, color);
	myDraw(&x, &y, 0, -100, color);

	x = 5, y = 5;
	myDraw(&x, &y, 550, 0, color);
	myDraw(&x, &y, 0, 230, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, 70, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, 50, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, 70, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, 190, color);
	myDraw(&x, &y, -550, 0, color);
	myDraw(&x, &y, 0, -190, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, -70, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, -50, color);
	myDraw(&x, &y, 100, 0, color);
	myDraw(&x, &y, 0, -70, color);
	myDraw(&x, &y, -100, 0, color);
	myDraw(&x, &y, 0, -230, color);


}


void CHMAT::drawBoard(int color) {//draws the map

	int x, y;
	drawEdge(color);

	//Ellipse(355, 195, 30, 30, 0xffff00, true);   

	Rectangle(270, 50, 20, 80, color, false);
	Rectangle(210, 110, 140, 20, color, false);


	Rectangle(270, 170, 20, 80, color, false);
	Rectangle(210, 230, 140, 20, color, false);

	Rectangle(270, 410, 20, 80, color, false);
	Rectangle(210, 470, 140, 20, color, false);

	Rectangle(150, 170, 80, 20, color, false);
	Rectangle(330, 170, 80, 20, color, false);

	Rectangle(50, 470, 60, 20, color, false);
	Rectangle(50, 530, 60, 40, color, false);
	Rectangle(150, 530, 80, 40, color, false);
	Rectangle(150, 230, 20, 80, color, false);

	Rectangle(150, 350, 20, 140, color, false);
	Rectangle(150, 410, 80, 20, color, false);

	Rectangle(450, 470, 60, 20, color, false);
	Rectangle(450, 530, 60, 40, color, false);
	Rectangle(330, 530, 80, 40, color, false);
	Rectangle(390, 230, 20, 80, color, false);

	Rectangle(390, 350, 20, 140, color, false);
	Rectangle(330, 410, 80, 20, color, false);

	Rectangle(50, 50, 180, 20, color, false);
	Rectangle(150, 50, 20, 80, color, false);

	Rectangle(330, 50, 180, 20, color, false);
	Rectangle(390, 50, 20, 80, color, false);

	Rectangle(90, 110, 20, 80, color, false);
	Rectangle(50, 170, 60, 20, color, false);

	Rectangle(450, 110, 20, 80, color, false);
	Rectangle(450, 170, 60, 20, color, false);

	x = 210;
	y = 290;

	myDraw(&x, &y, 140, 0, color);
	myDraw(&x, &y, 0, 80, color);
	myDraw(&x, &y, -50, 0, color);
	myDraw(&x, &y, 0, -10, color);
	myDraw(&x, &y, 40, 0, color);
	myDraw(&x, &y, 0, -60, color);
	myDraw(&x, &y, -120, 0, color);
	myDraw(&x, &y, 0, 60, color);
	myDraw(&x, &y, 40, 0, color);
	myDraw(&x, &y, 0, 10, color);
	myDraw(&x, &y, -50, 0, color);
	myDraw(&x, &y, 0, -80, color);

	Rectangle(260, 365, 40, 5, 0xFADBD8, true);
}


void CHMAT::goingBackPoints() {// when a ghost is eaten it uses these value to go back to the base



	set(30, 20, 2 + 10);
	set(30, 80, 1 + 10);
	set(70, 80, 1 + 10);
	set(130, 80, 2 + 10);
	set(130, 140, 2 + 10);
	set(130, 200, 2 + 10);
	set(130, 320, 1 + 10);
	set(190, 320, 2 + 10);
	set(190, 380, 1 + 10);
	set(250, 380, 1 + 10);

	set(70, 140, 3 + 10);
	set(30, 140, 2 + 10);
	set(30, 140, 2 + 10);
	set(30, 200, 1 + 10);

	set(250, 20, 2 + 10);
	set(250, 80, 3 + 10);
	set(190, 80, 2 + 10);
	set(190, 140, 3 + 10);
	set(250, 140, 2 + 10);
	set(250, 200, 3 + 10);
	set(190, 200, 2 + 10);
	set(190, 260, 2 + 10);

	set(310, 20, 2 + 10);
	set(310, 80, 1 + 10);
	set(370, 80, 2 + 10);
	set(370, 140, 1 + 10);
	set(310, 140, 2 + 10);
	set(310, 200, 1 + 10);
	set(370, 200, 2 + 10);
	set(370, 260, 2 + 10);
	set(370, 320, 2 + 10);
	set(370, 380, 3 + 10);
	set(310, 380, 3 + 10);


	set(430, 140, 2 + 10);
	set(430, 80, 2 + 10);
	set(490, 80, 3 + 10);
	set(530, 80, 3 + 10);
	set(530, 20, 2 + 10);

	set(430, 200, 3 + 10);
	set(530, 200, 3 + 10);
	set(530, 140, 2 + 10);
	set(490, 140, 1 + 10);

	set(430, 320, 3 + 10);

	set(30, 580, 4 + 10);
	set(30, 500, 1 + 10);
	set(130, 500, 1 + 10);
	set(190, 500, 4 + 10);
	set(190, 440, 1 + 10);
	set(250, 440, 4 + 10);

	set(30, 440, 1 + 10);
	set(130, 440, 2 + 10);
	set(130, 580, 4 + 10);
	set(250, 580, 4 + 10);
	set(250, 500, 3 + 10);



	set(310, 580, 4 + 10);
	set(310, 500, 1 + 10);
	set(370, 500, 4 + 10);
	set(370, 440, 3 + 10);
	set(310, 440, 4 + 10);


	set(430, 580, 4 + 10);
	set(430, 500, 3 + 10);
	set(530, 580, 4 + 10);
	set(530, 500, 3 + 10);
	set(530, 440, 2 + 10);
	set(430, 440, 2 + 10);
	set(430, 380, 2 + 10);



}

void CHMAT::print_mat()
{
	int x, y;
	unsigned char* ucd = (unsigned char*)data;
	int* id = (int*)data;
	for (y = 0; y < Y; y++)
	{
		for (x = 0; x < X; x++)
		{
			if (type == CHMAT_UNSIGNED_CHAR)
				printf("%d ", ucd[y * X + x]);
			else if (type == CHMAT_INT)
				printf("%d ", id[y * X + x]);
		}
		printf("\n");
	}
	printf("\n");
}

void CHMAT::operator = (int a)
{
	int x, y;
	unsigned char* ucd = (unsigned char*)data;
	int* id = (int*)data;
	for (y = 0; y < Y; y++)
	{
		for (x = 0; x < X; x++)
		{
			if (type == CHMAT_UNSIGNED_CHAR)
				ucd[y * X + x] = a;
			else if (type == CHMAT_INT)
				id[y * X + x] = a;
		}
	}
}

//Template????
void CHMAT::operator += (int a)
{
	int x, y;
	unsigned char* ucd = (unsigned char*)data;
	int* id = (int*)data;
	for (y = 0; y < Y; y++)
	{
		for (x = 0; x < X; x++)
		{
			if (type == CHMAT_UNSIGNED_CHAR)
				ucd[y * X + x] += a;
			else if (type == CHMAT_INT)
				id[y * X + x] += a;
		}
	}
}

int& CHMAT::operator () (int x, int y)
{
	static int r = 0;
	if (x > X || y > Y); //???????????????????
	int* t = (int*)data;
	return t[X * (y - 1) + (x - 1)];
}

int& CHMAT::get(int x, int y)
{
	static int r = 0;
	if (x > X || y > Y);
	int* t = (int*)data;
	return t[X * (y - 1) + (x - 1)];
}

struct cord
{
	int x = 0, y = 0;
};





cord cuttingPoint(cord A, cord B, cord C, cord D) {


	// Line AB represented as a1x + b1y = c1 
	double a1 = B.y - A.y;
	double b1 = A.x - B.x;
	double c1 = a1 * (A.x) + b1 * (A.y);

	// Line CD represented as a2x + b2y = c2 
	double a2 = D.y - C.y;
	double b2 = C.x - D.x;
	double c2 = a2 * (C.x) + b2 * (C.y);
	cord result;

	double determinant = a1 * b2 - a2 * b1;

	if (determinant == 0)
	{
		// The lines are parallel. This is simplified 
		// by returning a pair of FLT_MAX 
		printf("lalal");
		return result;
	}
	else
	{
		double x = (b2 * c1 - b1 * c2) / determinant;
		double y = (a1 * c2 - a2 * c1) / determinant;
		result.x = x;
		result.y = y;
		return result;
	}
}

bool CHMAT::Ellipse(int x, int y, int width, int height, int col, bool isFill)
{
	cord A, B, D, O, holder, h1, h2;
	bool result = false;

	A.x = x;
	A.y = y + height / 2;

	B.x = x + width / 2;
	B.y = y + height;

	D.x = x + width / 2;
	D.y = y;

	O.x = x + width / 2;
	O.y = y + height / 2;


	int counter1 = 0, counter2 = 0, pre_point_x = A.x, pre_point_y = A.y;
	int xlen = width / 2, ylen = height / 2;
	int counter = ylen;

	if (xlen > ylen) counter = xlen;



	for (int i = 0; xlen >= counter1 || ylen >= counter2; i++)
	{

		h1.x = x + counter1;
		h1.y = O.y;

		h2.x = x;
		h2.y = O.y + counter2;

		float pers1 = (counter1 * 100 / xlen);
		float pers2 = (counter2 * 100 / ylen);


		float difference = pers1 - pers2;
		if (difference < 0) difference = difference * (-1);

		if (pers1 == pers2) {
			counter1++;
			counter2++;
		}
		else if (pers1 > pers2) {
			counter2++;
			continue;
		}
		else {
			counter1++;
			continue;
		}
		holder = cuttingPoint(D, h1, B, h2);

		if (set(holder.x, holder.y, col) == 0) result = true;
		Line(holder.x, holder.y, pre_point_x, pre_point_y, col);

		if (set(holder.x + 2 * (O.x - holder.x), holder.y, col) == 0) result = true;
		Line(holder.x + 2 * (O.x - holder.x), holder.y, pre_point_x + 2 * (O.x - pre_point_x), pre_point_y, col);


		if (set(holder.x, holder.y - 2 * (holder.y - O.y), col) == 0) result = true;
		Line(holder.x, holder.y - 2 * (holder.y - O.y), pre_point_x, pre_point_y - 2 * (pre_point_y - O.y), col);

		if (set(holder.x + 2 * (O.x - holder.x), holder.y - 2 * (holder.y - O.y), col) == 0) result = true;
		Line(holder.x + 2 * (O.x - holder.x), holder.y - 2 * (holder.y - O.y), pre_point_x + 2 * (O.x - pre_point_x), pre_point_y - 2 * (pre_point_y - O.y), col);


		if (isFill) {
			if (pre_point_y != holder.y)Line_for_ellipse(pre_point_x, pre_point_y, holder.x, holder.y, col, true, O.x, O.y, width, height);

			for (int j = 0; j < O.x - holder.x; j++)
			{
				set(holder.x + j, holder.y, col);

				set(O.x + j, holder.y, col);

				set(holder.x + j, holder.y - 2 * (holder.y - O.y), col);
				set(O.x + j, holder.y - 2 * (holder.y - O.y), col);

			}
		}
		pre_point_x = holder.x;
		pre_point_y = holder.y;

	}

	// this part is for testing the ellipse inside of the given rectangle
	/*for (int i = 0; i < width; i++) set(x +i, y, 0xff0f);
	for (int i = 0; i < height; i++) set(x , y+i, 0xff0f);
	for (int i = height; i > 0; i--) set(x+width, y+i, 0xff0f);
	for (int i = width; i > 0; i--) set(x + i, y + height, 0xff0f);*/
	return result;


}


bool CHMAT::Rectangle(int x1, int y1, int width, int height, int color, bool is_filled)
{
	if (x1 > X || y1 > Y)
	{
		return false;
	}

	int i, j, k, l;

	if (is_filled == true)
	{
		for (j = y1; j <= y1 + height; j++)
		{
			for (i = x1; i <= x1 + width; i++)
			{
				set(i, j, color);
			}
		}
	}
	else if (is_filled == false)
	{
		for (i = x1; i <= x1 + width; i++)
		{
			set(i, y1, color);
		}

		for (j = y1; j <= y1 + height; j++)
		{
			set(x1, j, color);
		}

		for (k = x1 + width; k >= x1; k--)
		{
			set(k, y1 + height, color);
		}

		for (l = y1 + height; l >= y1; l--)
		{
			set(x1 + width, l, color);
		}
	}
	return true;
}


void CHMAT::Line(int x1, int y1, int x2, int y2, int color) {
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

	dx = x2 - x1;      /* the horizontal distance of the line */
	dy = y2 - y1;      /* the vertical distance of the line */
	dxabs = abs(dx);
	dyabs = abs(dy);
	sdx = sgn(dx);
	sdy = sgn(dy);
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = x1;
	py = y1;

	if (dxabs >= dyabs) /* the line is more horizontal than vertical */
	{
		for (i = 0; i < dxabs; i++)
		{
			y += dyabs;
			if (y >= dxabs)
			{
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			set(px, py, color);
		}
	}
	else /* the line is more vertical than horizontal */
	{
		for (i = 0; i < dyabs; i++)
		{
			x += dxabs;
			if (x >= dyabs)
			{
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			set(px, py, color);
		}
	}
	set(x1, y1, color);
	set(x2, y2, color);

}

void CHMAT::Line_for_ellipse(int x1, int y1, int x2, int y2, int color, bool isFill, int o_x, int o_y, int width, int height) {
	int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

	dx = x2 - x1;      /* the horizontal distance of the line */
	dy = y2 - y1;      /* the vertical distance of the line */
	dxabs = abs(dx);
	dyabs = abs(dy);
	sdx = sgn(dx);
	sdy = sgn(dy);
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = x1;
	py = y1;

	if (dxabs >= dyabs) /* the line is more horizontal than vertical */
	{
		for (i = 0; i < dxabs; i++)
		{
			y += dyabs;
			if (y >= dxabs)
			{
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			if (isFill) {
				for (int i = 0; i < o_x - x1; i++)
				{
					set(px + i, py, color);
					set(o_x + o_x - (px + i), py, color);
					set(px + i, (o_y - (py - o_y)), color);
					set(o_x + o_x - (px + i), (o_y - (py - o_y)), color);

				}
			}
			else set(px, py, color);
		}
	}
	else /* the line is more vertical than horizontal */
	{
		for (i = 0; i < dyabs; i++)
		{
			x += dxabs;
			if (x >= dyabs)
			{
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			if (isFill) {
				for (int i = 0; i < o_x - x1; i++)
				{
					set(px + i, py, color);



					set(o_x + o_x - (px + i), py, color);
					set(px + i, (o_y - (py - o_y)), color);
					set(o_x + o_x - (px + i), (o_y - (py - o_y)), color);
				}
			}
			else set(px, py, color);
		}
	}
}


int CHMAT::ghostMove(int x, int y, int moveTo) {// return a movable random direction
	int holder, counter = 0;
	if (isMovable(x, y, moveTo)) {
		return moveTo;
	}

	if (isMovable(x, y, 1)) {
		counter++;
	}if (isMovable(x, y, 2)) {
		counter++;
	}if (isMovable(x, y, 3)) {
		counter++;
	}if (isMovable(x, y, 4)) {
		counter++;
	}
	if (counter == 1) {
		do {
			holder = (rand() % 4) + 1;
		} while (!isMovable(x, y, holder));

	}
	else {
		do {
			holder = (rand() % 4) + 1;
		} while (!isMovable(x, y, holder) || (!(holder != moveTo + 2 && holder != moveTo - 2)));

	}
	return holder;
}

int CHMAT::ghostMove(int pacmanX, int pacmanY, int ghostX, int ghostY) {// it retuns a direction from ghost to pacman so it can follow
	int holderX = ghostX - pacmanX, holderY = ghostY - pacmanY, holder1, holder2;
	int holderX2 = holderX, holderY2 = holderY;

	if (holderX < 0) {
		holderX2 *= -1;
		holder2 = 1;
	}
	else {
		holder2 = 3;
	}


	if (holderY < 0) {
		holderY2 *= -1;
		holder1 = 2;
	}
	else {
		holder1 = 4;
	}

	if (holderX2 < holderY2) {
		if (isMovable(ghostX, ghostY, holder1)) {
			return holder1;
		}
	}
	if (isMovable(ghostX, ghostY, holder2))
		return holder2;


	return 0;
}

void CHMAT::printPacman(int x, int y, int size, int color, int moveTo, int* step, bool stepUp) {

	Ellipse(x + 4, y + 4, size - 8, size - 8, color, true);

	bool holder = false;
	if (*step == 0 || *step == 1 || *step == 2) holder = true;
	if (moveTo == 1 && *step == 3)

		for (int i = 0; i < 16; i++)
			Line(x + 19 + i, y + 19 + i, x + 19 + i, y + 19 - i, 0);
	else if (moveTo == 2 && holder)
		for (int i = 0; i < 16; i++)
		{
			Line(x + 19 + i, y + 19 + i, x + 19 - i, y + 19 + i, 0);

		}
	else if (moveTo == 3 && holder)
		for (int i = 0; i < 16; i++)
		{
			Line(x + 19 - i, y + 19 + i, x + 19 - i, y + 19 - i, 0);

		}
	else if (moveTo == 4 && holder)
		for (int i = 0; i < 16; i++)
		{
			Line(x + 19 + i, y + 19 - i, x + 19 - i, y + 19 - i, 0);

		}
	if (stepUp)
		*step = (*step + 1) % 4;
}

void CHMAT::printGhost(int X, int Y, int size, int moveTo, int color, int mod)
{

	if (mod == 0) { // normal game mode
		if (moveTo == 1)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 11, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 26, Y + 19, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 2)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 21, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 21, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 3)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 7, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 22, Y + 19, size - 32, size - 32, 0x0000ff, true);

		}
		else if (moveTo == 4)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 17, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 17, size - 32, size - 32, 0x0000ff, true);

		}
	}
	else if (mod == 1) {
		if (moveTo == 1)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, 0x0000ff, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, 0x0000ff, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, 0x0000ff);
			Inverted_Triangle(X + 8, Y + 12, size - 16, 0x0000ff);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, 0x0000ff);

			Ellipse(X + 9, Y + 19, size - 31, size - 31, 0xffffff, true);
			Ellipse(X + 24, Y + 19, size - 31, size - 31, 0xffffff, true);

			Line(X + 7, Y + 9, X + 10, Y + 12, 0xffffff);
			Line(X + 10, Y + 12, X + 13, Y + 9, 0xffffff);
			Line(X + 13, Y + 9, X + 16, Y + 12, 0xffffff);
			Line(X + 16, Y + 12, X + 19, Y + 9, 0xffffff);
			Line(X + 19, Y + 9, X + 22, Y + 12, 0xffffff);
			Line(X + 22, Y + 12, X + 25, Y + 9, 0xffffff);
			Line(X + 25, Y + 9, X + 28, Y + 12, 0xffffff);
			Line(X + 28, Y + 12, X + 31, Y + 9, 0xffffff);
			Line(X + 7, Y + 10, X + 10, Y + 13, 0xffffff);
			Line(X + 10, Y + 13, X + 13, Y + 10, 0xffffff);
			Line(X + 13, Y + 10, X + 16, Y + 13, 0xffffff);
			Line(X + 16, Y + 13, X + 19, Y + 10, 0xffffff);
			Line(X + 19, Y + 10, X + 22, Y + 13, 0xffffff);
			Line(X + 22, Y + 13, X + 25, Y + 10, 0xffffff);
			Line(X + 25, Y + 10, X + 28, Y + 13, 0xffffff);
			Line(X + 28, Y + 13, X + 31, Y + 10, 0xffffff);

		}
		else if (moveTo == 2)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, 0x0000ff, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, 0x0000ff, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, 0x0000ff);
			Inverted_Triangle(X + 8, Y + 12, size - 16, 0x0000ff);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, 0x0000ff);

			Ellipse(X + 9, Y + 19, size - 31, size - 31, 0xffffff, true);
			Ellipse(X + 24, Y + 19, size - 31, size - 31, 0xffffff, true);

			Line(X + 7, Y + 9, X + 10, Y + 12, 0xffffff);
			Line(X + 10, Y + 12, X + 13, Y + 9, 0xffffff);
			Line(X + 13, Y + 9, X + 16, Y + 12, 0xffffff);
			Line(X + 16, Y + 12, X + 19, Y + 9, 0xffffff);
			Line(X + 19, Y + 9, X + 22, Y + 12, 0xffffff);
			Line(X + 22, Y + 12, X + 25, Y + 9, 0xffffff);
			Line(X + 25, Y + 9, X + 28, Y + 12, 0xffffff);
			Line(X + 28, Y + 12, X + 31, Y + 9, 0xffffff);
			Line(X + 7, Y + 10, X + 10, Y + 13, 0xffffff);
			Line(X + 10, Y + 13, X + 13, Y + 10, 0xffffff);
			Line(X + 13, Y + 10, X + 16, Y + 13, 0xffffff);
			Line(X + 16, Y + 13, X + 19, Y + 10, 0xffffff);
			Line(X + 19, Y + 10, X + 22, Y + 13, 0xffffff);
			Line(X + 22, Y + 13, X + 25, Y + 10, 0xffffff);
			Line(X + 25, Y + 10, X + 28, Y + 13, 0xffffff);
			Line(X + 28, Y + 13, X + 31, Y + 10, 0xffffff);
		}
		else if (moveTo == 3)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, 0x0000ff, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, 0x0000ff, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, 0x0000ff);
			Inverted_Triangle(X + 8, Y + 12, size - 16, 0x0000ff);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, 0x0000ff);

			Ellipse(X + 9, Y + 19, size - 31, size - 31, 0xffffff, true);
			Ellipse(X + 24, Y + 19, size - 31, size - 31, 0xffffff, true);

			Line(X + 7, Y + 9, X + 10, Y + 12, 0xffffff);
			Line(X + 10, Y + 12, X + 13, Y + 9, 0xffffff);
			Line(X + 13, Y + 9, X + 16, Y + 12, 0xffffff);
			Line(X + 16, Y + 12, X + 19, Y + 9, 0xffffff);
			Line(X + 19, Y + 9, X + 22, Y + 12, 0xffffff);
			Line(X + 22, Y + 12, X + 25, Y + 9, 0xffffff);
			Line(X + 25, Y + 9, X + 28, Y + 12, 0xffffff);
			Line(X + 28, Y + 12, X + 31, Y + 9, 0xffffff);
			Line(X + 7, Y + 10, X + 10, Y + 13, 0xffffff);
			Line(X + 10, Y + 13, X + 13, Y + 10, 0xffffff);
			Line(X + 13, Y + 10, X + 16, Y + 13, 0xffffff);
			Line(X + 16, Y + 13, X + 19, Y + 10, 0xffffff);
			Line(X + 19, Y + 10, X + 22, Y + 13, 0xffffff);
			Line(X + 22, Y + 13, X + 25, Y + 10, 0xffffff);
			Line(X + 25, Y + 10, X + 28, Y + 13, 0xffffff);
			Line(X + 28, Y + 13, X + 31, Y + 10, 0xffffff);
		}
		else if (moveTo == 4)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, 0x0000ff, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, 0x0000ff, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, 0x0000ff);
			Inverted_Triangle(X + 8, Y + 12, size - 16, 0x0000ff);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, 0x0000ff);

			Ellipse(X + 9, Y + 19, size - 31, size - 31, 0xffffff, true);
			Ellipse(X + 24, Y + 19, size - 31, size - 31, 0xffffff, true);

			Line(X + 7, Y + 9, X + 10, Y + 12, 0xffffff);
			Line(X + 10, Y + 12, X + 13, Y + 9, 0xffffff);
			Line(X + 13, Y + 9, X + 16, Y + 12, 0xffffff);
			Line(X + 16, Y + 12, X + 19, Y + 9, 0xffffff);
			Line(X + 19, Y + 9, X + 22, Y + 12, 0xffffff);
			Line(X + 22, Y + 12, X + 25, Y + 9, 0xffffff);
			Line(X + 25, Y + 9, X + 28, Y + 12, 0xffffff);
			Line(X + 28, Y + 12, X + 31, Y + 9, 0xffffff);
			Line(X + 7, Y + 10, X + 10, Y + 13, 0xffffff);
			Line(X + 10, Y + 13, X + 13, Y + 10, 0xffffff);
			Line(X + 13, Y + 10, X + 16, Y + 13, 0xffffff);
			Line(X + 16, Y + 13, X + 19, Y + 10, 0xffffff);
			Line(X + 19, Y + 10, X + 22, Y + 13, 0xffffff);
			Line(X + 22, Y + 13, X + 25, Y + 10, 0xffffff);
			Line(X + 25, Y + 10, X + 28, Y + 13, 0xffffff);
			Line(X + 28, Y + 13, X + 31, Y + 10, 0xffffff);
		}
	}
	else if (mod == 2) {// go base with eyes mod
		if (moveTo == 1)
		{
			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 11, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 26, Y + 19, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 2)
		{
			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 21, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 21, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 3)
		{
			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 7, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 22, Y + 19, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 4)
		{
			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 17, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 17, size - 32, size - 32, 0x0000ff, true);
		}
	}
	else if (mod == 3) {
		if (moveTo == 1)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 11, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 26, Y + 19, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 2)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 21, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 21, size - 32, size - 32, 0x0000ff, true);
		}
		else if (moveTo == 3)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 7, Y + 19, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 22, Y + 19, size - 32, size - 32, 0x0000ff, true);

		}
		else if (moveTo == 4)
		{
			Rectangle(X + 4, Y + 8, size - 8, size - 19, color, true);
			Ellipse(X + 4, Y + 7, size - 8, size - 8, color, true);
			Inverted_Right_Triangle_L(X + 4, Y + 7, size - 32, color);
			Inverted_Triangle(X + 8, Y + 12, size - 16, color);
			Inverted_Right_Triangle_R(X + 28, Y + 7, size - 32, color);

			Ellipse(X + 7, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 9, Y + 17, size - 32, size - 32, 0x0000ff, true);

			Ellipse(X + 22, Y + 17, size - 27, size - 27, 0xffffff, true);
			Ellipse(X + 24, Y + 17, size - 32, size - 32, 0x0000ff, true);

		}
	}


}
void CHMAT::drawPackmanFood(int locationX, int locationY, int foodType, CHMAT& checker) {
	int counter = 0;
	if (foodType == enmObjectType::smallFood) {
		Rectangle(locationX - 1, locationY - 1, 3, 3, enmObjectColor::smallFoodColor, true);
		checker(locationX, locationY) = 2;
	}
	else if (foodType == enmObjectType::largeFood) {
		Ellipse(locationX - 4, locationY - 4, 9, 9, enmObjectColor::largeFoodColor, true);
		checker(locationX, locationY) = 3;
	}
}

template int CHMAT::set<int>(int, int, int);
template int CHMAT::set<double>(int, int, double);


void CHMAT::Inverted_Right_Triangle_L(int x, int y, int width, int color)
{
	int i, j, cnt = 0;

	for (j = y; j >= 1; j--)
	{
		for (i = x; i <= x + width - cnt; i++)
		{
			set(i, j, color);
		}
		cnt++;
	}
}

void CHMAT::Inverted_Triangle(int x, int y, int width, int color)
{
	int i, j, cnt = -1;

	for (j = y; j >= 1; j--)
	{
		cnt++;

		for (i = x; i <= x + width - cnt; i++)
		{
			set(i, j, color);
		}

		x++, cnt++;
	}
}

void CHMAT::Inverted_Right_Triangle_R(int x, int y, int width, int color)
{
	int i, j, cnt = 0;

	for (j = y; j >= 1; j--)
	{
		for (i = x; i <= x + width - cnt; i++)
		{
			set(i, j, color);
		}
		x++, cnt++;
	}
}

void CHMAT::drawFoods(CHMAT& checker) {

	drawPackmanFood(30, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(150, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(170, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(270, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(290, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(390, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(410, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 30, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 30, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 50, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 50, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 50, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 50, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 70, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 70, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 70, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 70, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 90, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 90, enmObjectType::smallFood, checker);

	drawPackmanFood(70, 110, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 110, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 110, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 110, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 110, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 110, enmObjectType::smallFood, checker);

	drawPackmanFood(70, 130, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 130, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 130, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 130, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 130, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 130, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 150, enmObjectType::largeFood, checker);
	drawPackmanFood(50, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(150, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(170, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(390, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(410, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 150, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 150, enmObjectType::largeFood, checker);

	drawPackmanFood(30, 170, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 170, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 170, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 170, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 170, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 170, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 190, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 190, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 190, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 190, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 190, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 190, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(150, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(170, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(390, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(410, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 210, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 210, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 230, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 230, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 250, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 250, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 270, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 270, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 290, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 290, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 310, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 310, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 330, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 330, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 350, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 350, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 370, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 370, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 390, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 390, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 410, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 410, enmObjectType::smallFood, checker);

	drawPackmanFood(130, 430, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 430, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 450, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 450, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 470, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 470, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 470, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 470, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 490, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 490, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 490, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 490, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(150, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(170, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(270, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(290, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(390, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(410, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 510, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 510, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 530, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 530, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 530, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 530, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 530, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 530, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 550, enmObjectType::largeFood, checker);
	drawPackmanFood(130, 550, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 550, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 550, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 550, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 550, enmObjectType::largeFood, checker);

	drawPackmanFood(30, 570, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 570, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 570, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 570, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 570, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 570, enmObjectType::smallFood, checker);

	drawPackmanFood(30, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(50, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(70, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(90, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(110, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(130, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(150, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(170, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(190, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(210, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(230, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(250, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(310, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(330, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(350, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(370, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(390, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(410, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(430, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(450, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(470, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(490, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(510, 590, enmObjectType::smallFood, checker);
	drawPackmanFood(530, 590, enmObjectType::smallFood, checker);
}

void CHMAT::setTurningPoint() {
	//1st
	set(30, 30 - 2, 13);
	set(250, 30 - 2, 13);
	set(310, 30 - 2, 13);
	set(530, 30 - 2, 13);
	set(30, 30 - 2, 13);

	//2st
	set(30, 90 - 2, 13);
	set(70, 90 - 2, 13);
	set(130, 90 - 2, 13);
	set(190, 90 - 2, 13);
	set(250, 90 - 2, 13);
	set(310, 90 - 2, 13);
	set(370, 90 - 2, 13);
	set(430, 90 - 2, 13);
	set(490, 90 - 2, 13);
	set(530, 90 - 2, 13);

	//3st
	set(30, 150 - 2, 13);
	set(70, 150 - 2, 13);
	set(130, 150 - 2, 13);
	set(190, 150 - 2, 13);
	set(250, 150 - 2, 13);
	set(310, 150 - 2, 13);
	set(370, 150 - 2, 13);
	set(430, 150 - 2, 13);
	set(490, 150 - 2, 13);
	set(530, 150 - 2, 13);

	//4st
	set(30, 210 - 2, 13);
	set(130, 210 - 2, 13);
	set(190, 210 - 2, 13);
	set(250, 210 - 2, 13);
	set(310, 210 - 2, 13);
	set(370, 210 - 2, 13);
	set(430, 210 - 2, 13);
	set(530, 210 - 2, 13);

	//5st
	set(190, 270 - 2, 13);
	set(370, 270 - 2, 13);

	//6st
	set(130, 330 - 2, 13);
	set(190, 330 - 2, 13);
	set(370, 330 - 2, 13);
	set(430, 330 - 2, 13);

	//7
	set(190, 390 - 2, 13);
	set(250, 390 - 2, 13);
	set(310, 390 - 2, 13);
	set(370, 390 - 2, 13);

	//8st
	set(30, 450 - 2, 13);
	set(130, 450 - 2, 13);
	set(190, 450 - 2, 13);
	set(250, 450 - 2, 13);
	set(310, 450 - 2, 13);
	set(370, 450 - 2, 13);
	set(430, 450 - 2, 13);
	set(530, 450 - 2, 13);

	//9st
	set(30, 510 - 2, 13);
	set(130, 510 - 2, 13);
	set(190, 510 - 2, 13);
	set(250, 510 - 2, 13);
	set(310, 510 - 2, 13);
	set(370, 510 - 2, 13);
	set(430, 510 - 2, 13);
	set(530, 510 - 2, 13);

	//10th
	set(30, 590 - 2, 13);
	set(130, 590 - 2, 13);
	set(250, 590 - 2, 13);
	set(310, 590 - 2, 13);
	set(430, 590 - 2, 13);
	set(530, 590 - 2, 13);




}