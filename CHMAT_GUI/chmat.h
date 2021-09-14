#define CHMAT_UNSIGNED_CHAR     1
#define CHMAT_UNSIGNED_INT      2
#define CHMAT_INT               3
#define CHMAT_FLOAT             4
#define CHMAT_DOUBLE            5

class CHMAT
{
public:

    // Object Types 
    enum enmObjectType { wallObjectType = 1, smallFood = 2, largeFood = 3 };
    enum enmObjectColor {smallFoodColor = 0xFADBD8, mediumFoodColor = 0xFADBD8, largeFoodColor = 0xFADBD8};
    enum enmKeys { RightArrow = 39, LeftArrow = 37, UpArrow = 38, DownArrow = 40 };
    //members
    int X = 0, Y = 0, type = 0;
    void* data = 0;
    HBITMAP HBitmap; //bitmap handle
    //---constructer & destructor ------------- --------
    CHMAT(int x, int y, int typ);
    ~CHMAT();
    //---methods ---------------------
    template<typename T> int set(int x, int y, T val);
    void print_mat();
    //---operators -----------------
    void operator = (int a);
    void operator += (int a) ;
    int& operator () (int x, int y);
    int& get(int x, int y);

    bool isMovable(int x,int y,int moveTo);//ismet
    bool Ellipse(int x, int y, int width, int height, int color, bool isFill);//ismet
    void Line_for_ellipse(int x1, int y1, int x2, int y2, int color, bool isFill, int o_x, int o_y, int width, int height);//ismet
    void drawBoard(int color);//ismet
    void drawEdge(int color);//İsmet
    void myDraw(int *x, int *y, int numX, int numY,int color);//ismet
    void ghostClear(CHMAT &checker,int ghostX, int ghostY,int ghostSize,int mode);//ismet
    void setTurningPoint();//ismet
    int ghostMove(int x, int y, int moveTo);// ismet
    int ghostMove(int pacmanX , int pacmanY, int ghostX, int ghostY);//ismet
    void printPacman(int x, int y, int size, int color, int moveTo,int *step,bool stepUp);//ismet
    int pacmanEatFood(int x,int y,int moveTo);//ismet
    void goingBackPoints();//ismet
    void clearPacman(int x, int y, int size);


    void printGhost(int X, int Y, int size, int moveTo, int color,int mod);//ilhan
    bool Rectangle(int x1, int y1, int width, int height, int color, bool is_filled);//ilhan
    void Inverted_Right_Triangle_L(int x, int y, int width, int color);//ilhan
    void Inverted_Triangle(int x, int y, int width, int color);//ilhan
    void Inverted_Right_Triangle_R(int x, int y, int width, int color);//ilhan

    void Line(int x1, int y1, int x2, int y2, int color);//abdulaziz
    void drawPackmanFood(int locationX, int locationY, int foodType,CHMAT &checker);//abdulAziz
    void drawFoods(CHMAT &checker);//abdulaziz    

};

