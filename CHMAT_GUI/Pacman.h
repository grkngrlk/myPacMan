


class Pacman
{
private:
	int X, Y, color=0xffff00,size=38;
	int moveTo;//moveTo == 1 means go right, 2 means go up, 3 means go left,4 means go down
	int score = 0;
	int life = 3;
	int mod = 0;
	int startingX, startingY;
public:
	int step = 0 ;

	int getColor();
	int getStep();
	int getX();
	int getY();
	int getMoveTo();
	void setMoveTo(int newVariable);
	int getSize();
	int getLife();
	int getMod();
	void setMod(int newValue);
	int getScore();
	void setScore(int newValue);
	void setLife(int new_life);
	void setX(int value);
	void setY(int value);
	void move();
	int getStartX();
	int getStartY();
	Pacman(int x, int y,int move, int clr);
	void setColor(int color);
	bool didGhostKill(int pacmanX, int pacmanY);
	bool isGhostClose(int ghostX, int ghostY);
	void reverse();

};

