#include <iostream>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <ctime>

/*
    This part was copied from
    http://www.cplusplus.com/forum/beginner/4234/
*/
#include <stdio.h>
#include <conio.h>
void gotoxy( int column, int line )
{
    COORD coord;
    coord.X = column;
    coord.Y = line;
    SetConsoleCursorPosition(
        GetStdHandle( STD_OUTPUT_HANDLE ),
        coord
    );
}

int wherex()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD                      result;
    if (!GetConsoleScreenBufferInfo(
         GetStdHandle( STD_OUTPUT_HANDLE ),
         &csbi
         ))
    return -1;
    return result.X;
}

int wherey()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD                      result;
    if (!GetConsoleScreenBufferInfo(
         GetStdHandle( STD_OUTPUT_HANDLE ),
         &csbi
         ))
    return -1;
    return result.Y;
}

#define UP          0
#define RIGHT       1
#define DOWN        2
#define LEFT        3

using namespace std;
class position{
public:
    position(){
        this->x = 0;
        this->y = 0;
    }
    position(int x, int y){
        this->x = x;
        this->y = y;
    }
    position(const position& p){
        this->x = p.x;
        this->y = p.y;
    }
    ~position(){

    }
    int x;
    int y;
    void operator=(const position& p){
        this->x = p.x;
        this->y = p.y;
    }
    bool operator==(const position& p){
        if(this->x == p.x && this->y == p.y)
            return true;
        else
            return false;
    }
};
class game{
private:
    int x;
    int y;
    vector<position> snake;
    position food;
    int direction;
    int steep;
    int time;
    bool go;
public:
    game(int x, int y){
        this->x = x;
        this->y = y;
        this->direction = LEFT;
        position snake1(this->x, this->y/2);
        this->snake.push_back(snake1);
        this->steep = 200;
        this->time = 0;
        food = position(this->x/2, this->y/2);
        this->go = false;
    }
    ~game(){
    }
    void printField( bool snakeVisible ){
        string field = "X: " + to_string(this->snake[0].x) + " | Y:" + to_string(this->snake[0].y) + " | DIRECTION: " + to_string(this->direction) + " | TIME: " + this->spentTime() + " | SCORE: " + to_string(this->score()) + " | FPS: " + to_string( 1000/this->steep ) + "\n";
        for(int i = 0; i < this->y; i++){
            for(int n = 0; n < this->x; n++){
                position p(n,i);
                bool found = false;
                if(p == this->food){
                    field += "X";
                    found = true;
                }
                for(int k = 0; k < this->snake.size(); k++){
                    if(p == snake[k]){
                        if(found)
                            field[field.size() - 1] = 'o';
                        else
                            field += "o";
                        if(!snakeVisible)
                            field[field.size() - 1] = ' ';
                        found = true;
                    }
                }
                if(found == false)
                    field += "+";
            }
            field += "\n";
        }
        gotoxy(0,0);
        cout << field;
    }
    void step(){
        this->checkInput();
        for(int i = this->snake.size(); i > 0; i--){
            this->snake[i] = this->snake[i-1];
        }
        switch(this->direction){
            case UP:
                this->snake[0].y--;
                if(this->snake[0].y < 0)
                    this->snake[0].y = this->y;
                break;
            case RIGHT:
                this->snake[0].x++;
                if(this->snake[0].x > this->x)
                    this->snake[0].x = 0;
                break;
            case LEFT:
                this->snake[0].x--;
                if(this->snake[0].x < 0)
                    this->snake[0].x = this->x;
                break;
            case DOWN:
                this->snake[0].y++;
                if(this->snake[0].y > this->y)
                    this->snake[0].y = 0;
                break;
        }
        if(this->snake[0] == this->food){
            this->add();
            this->food.x = rand()%this->x;
            this->food.y = rand()%this->y;
        }

        for(int i = 1; i < this->snake.size(); i++){
            if(snake[0] == snake[i]){
                Sleep(2000);
                for(int i = 0; i <= 20; i++){
                    this->printField(true);
                    gotoxy(0,0);
                    Sleep(100);
                    this->printField(false);
                    gotoxy(0,0);
                    Sleep(100);
                }
                this->cls();
                cout << "Game Over.\n  Score: " << this->score() << endl;
                Sleep(2000);
                this->go = true;
            }
        }

        Sleep(this->steep);
        this->time += this->steep;
    }
    void add(){
        position p(this->snake[this->snake.size()].x, this->snake[this->snake.size()].y);
        switch(this->direction){
            case UP:
                p.y++;
                break;
            case RIGHT:
                p.x--;
                break;
            case LEFT:
                p.x++;
                break;
            case DOWN:
                p.y--;
                break;
        }
        this->snake.push_back(p);
        if(this->steep > 10){
            this->steep -= 10;
        }
    }
    void checkInput(){
        if( GetAsyncKeyState(VK_UP) < 0  && this->direction != DOWN){
            this->direction = UP;
        }else if( GetAsyncKeyState(VK_LEFT) < 0  && this->direction != RIGHT){
            this->direction = LEFT;
        }else if( GetAsyncKeyState(VK_RIGHT) < 0  && this->direction != LEFT ){
            this->direction = RIGHT;
        }else if( GetAsyncKeyState(VK_DOWN) < 0  && this->direction != UP){
            this->direction = DOWN;
        }
        if( GetAsyncKeyState(VK_SPACE) < 0 && this->steep > 10){
            this->steep -= 10;
        }
    }
    int score(){
        return this->snake.size() - 1;
    }
    string spentTime(){
        string time = "";
        int t = this->time / 1000;
        int s = 0;
        int m = 0;
        int h = 0;
        for(int i = 0; i < t; i++){
            s++;
            if(s == 60){
                s=0;
                m++;
            }
            if(m == 60){
                m=0;
                h++;
            }
        }
        time = to_string(h) + "h " + to_string(m) + "m " + to_string(s) + "s";
        return time;

    }
    bool gameOver(){
        return this->go;
    }
    void cls(){
        system("cls");
    }

};
int main()
{
    game g(60,40);
    while(true){
        g.step();
        g.printField(true);
        if(g.gameOver())
            break;
    }
    return 0;
}
