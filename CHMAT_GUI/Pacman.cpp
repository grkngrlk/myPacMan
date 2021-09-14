#include "Pacman.h"


void Pacman::setColor(int clr) {
	color = clr;
}
void Pacman::move() { //moves pacman

	
	switch (moveTo)
	{
	case 1:
		X += 1;
		break;
	case 2:
		Y += 1;
		break;
	case 3:
		X -= 1;
		break;
	case 4:
		Y -= 1;
		break;
	default:
		break;

	}
}

Pacman::Pacman(int x, int y,int move,int clr) {//Constructor with coordiantes
	X = x;
	startingX = x;
	Y = y;
	startingY = y;
	moveTo = move;
	color = clr;

}

bool Pacman::didGhostKill(int pacmanX, int pacmanY) {//deletes ghost when it been eaten
	int holder1 = pacmanX - X;
	int holder2 = Y - pacmanY;
	if (400 < (holder1 * holder1 + holder2 * holder2)) {
		return false;
	}
	return true;
}



bool Pacman::isGhostClose(int x, int y) {//c
	int holder1= X - x, holder2 = Y -y;
	if(7000> holder1*holder1 + holder2*holder2)
		return true;
	return false;
}


void Pacman::reverse() {
	switch (moveTo)
	{
	case 1:
		moveTo = 3;
		break;
	case 2:
		moveTo = 4;
		break;
	case 3:
		moveTo = 1;
		break;
	case 4:
		moveTo = 2;
		break;
	default:
		break;
	}
}


int Pacman::getColor() {
	return color;
}
int Pacman::getStep() {
	return step;
}
int Pacman::getX() {
	return X;
}
int Pacman::getY() {
	return Y;
}
int Pacman::getMoveTo() {
	return moveTo;
}
int Pacman::getScore(){
	return score;
}
void Pacman::setScore(int newValue) {
	if (newValue < 0)
		return;
	score = newValue;
}
void Pacman::setLife(int newValue) {
	if (newValue < 0)
		return;
	life = newValue;
}
void Pacman::setMoveTo(int newValue) {
	if (newValue < 1 || newValue > 4)
		return;
	moveTo = newValue;
}
void Pacman::setX(int newValue) {
	if (newValue < 0)
		return;
	X = newValue;
}
void Pacman::setY(int newValue) {
	if (newValue < 0)
		return;
	Y = newValue;
}
int Pacman::getStartX() {
	return startingX;
}
int Pacman::getStartY() {
	return startingY;
}
int Pacman::getSize() {
	return size;
}
int Pacman::getLife() {
	return life;
}
int Pacman::getMod() {
	return mod;
}

void Pacman::setMod(int newValue) {
	if (newValue < 0 || newValue > 5)
		return;
	mod = newValue;
}