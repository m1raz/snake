#include <iostream>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <ctime>
#include <stdio.h>
#include <conio.h>
#include <fstream>

#define UP          0
#define RIGHT       1
#define DOWN        2
#define LEFT        3

#define NEW_GAME    101
#define H_SCORE     102
#define EXIT        103

using namespace std;
namespace console{
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
    void cls(){
        system("cls");
    }
}

struct menu_item{
    int id;
    string item_txt;
    menu_item(int id, string item_txt){
        this->id = id;
        this->item_txt = item_txt;
    }
};

class menu{
private:
    vector<menu_item> item;
    int preselected;
    int selected;
public:
    menu(){
        menu_item mi(NEW_GAME, "New Game");
        this->item.push_back(mi);
        mi = menu_item(H_SCORE, "High Score");
        this->item.push_back(mi);
        mi = menu_item(EXIT, "Exit");
        this->item.push_back(mi);
        this->preselected = 0;
        this->selected = -1;
    }
    ~menu(){
        //deconstructor
    }
    void checkInput(){
        this->selected = -1;
        if( GetAsyncKeyState(VK_UP) < 0){
            this->preselected--;
            if(this->preselected < 0){
                this->preselected = this->item.size() - 1;
            }
        }else if( GetAsyncKeyState(VK_DOWN) < 0){
            this->preselected++;
            if(this->preselected > this->item.size() - 1){
                this->preselected = 0;
            }
        }
        if(GetAsyncKeyState(VK_SPACE) < 0){
            this->selected = this->item[this->preselected].id;
        }
        Sleep(150);
    }
    bool isSelected(){
        if(this->selected >= 0)
            return true;
    }
    int selectedItem(){
        return this->selected;
    }
    void printMenu(){
        string m = "Menu\n====\n";
        for(int i = 0; i < this->item.size(); i++){
            if(i == this->preselected)
                m += "> ";
            else
                m += "  ";
            m += this->item[i].item_txt + '\n';
        }
        console::gotoxy(0,0);
        cout << m;
    }
};
class score{
private:
    int value;
    string owner;
public:
    score(){
        this->value = 0;
        this->owner = "snake";
    }
    score(int value, string owner){
        this->value = value;
        this->owner = owner;
    }
    void increase(int add){
        this->value += add;
    }
    void decrease(int take){
        this->value -= take;
    }
    void setName(){
        string n;
        console::cls();
        console::gotoxy(0,0);
        cout << "Enter your name: ";
        cin >> n;
        this->owner = n;
        console::cls();
    }
    int saveScore(){
        string s = this->owner + ',' + to_string(this->value) + '\n';
        string line;
        ifstream myfile ("score");
        if (myfile.is_open()){
            while ( getline (myfile,line) ){
                s += line + '\n';
            }
            myfile.close();
        }else{
            cout << "Unable to open scorefile";
            return 0;
        }
        ofstream scoreo;
        scoreo.open ("score");
        scoreo << s;
        scoreo.close();
    }
    string show(){
        return to_string(this->value);
    }
    void showHighScore(){
        vector<score> s;
        string line;
        ifstream myfile ("score");
        if (myfile.is_open()){
            while ( getline (myfile,line) ){
                string name = "";
                string scr = "";
                int pos = 0;
                for(int i = 0; i < line.size(); i++){
                    switch(pos){
                        case 0:
                            if(line[i] != ',')
                                name += line[i];
                            else
                                pos = 1;
                        break;
                        case 1:
                            scr += line[i];
                    }
                }
                s.push_back(score(stoi(scr), name));
            }
            myfile.close();
            for(int i = 0; i < s.size(); i++){
                for(int n = 0; n < s.size(); n++){
                    if(s[n].value < s[n+1].value){
                        score temp(s[n].value, s[n].owner);
                        s[n] = s[n+1];
                        s[n+1] = temp;
                    }
                }
            }
            string hs = "High Score\n==========\n";
            int blo = 0;
            for(int i = 0; i <= s.size() - 1; i++){
                if(s[i].owner.size() > blo)
                    blo = s[i].owner.size();
            }
            int blv = 0;
            for(int i = 0; i <= s.size() - 1; i++){
                if(s[i].show().size() > blv)
                    blv = s[i].show().size();
            }
            for(int i = 0; i <= s.size() - 1; i++){
                hs += s[i].owner;
                for(int n = 0; n < blo - s[i].owner.size(); n++){
                    hs += '.';
                }
                hs += ".............";
                for(int n = 0; n < blv - s[i].show().size(); n++){
                    hs += '.';
                }
                hs += s[i].show() + '\n';
            }
            console::cls();
            cout << hs;
            while(true){
                if(GetAsyncKeyState(VK_SPACE) < 0){
                    Sleep(150);
                    break;
                }
            }
            console::cls();
        }else{
            cout << "Unable to open scorefile";
        }
    }
    void operator=(const score& s){
        this->value = s.value;
        this->owner = s.owner;
    }
};

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
    score score;
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
        string field = "TIME:" + this->spentTime() + " SCORE:" + this->score.show() + "\n";
        for(int i = 0; i < this->y; i++){
            for(int n = 0; n < this->x; n++){
                position p(n,i);
                bool found = false;
                if(p == this->food){
                    field += ".";
                    found = true;
                }
                for(int k = 0; k < this->snake.size(); k++){
                    if(p == snake[k]){
                        if(found)
                            field[field.size() - 1] = '.';
                        else
                            field += ".";
                        bool isHead = false;
                        if(this->snake[0].x == n && this->snake[0].y == i){
                            field[field.size() - 1] = 'o';
                        }
                        if(!snakeVisible)
                            field[field.size() - 1] = 'x';
                        found = true;
                    }
                }
                if(found == false)
                    field += " ";
            }
            field += "\n";
        }
        console::gotoxy(0,0);
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
                if(this->snake[0].x >= this->x)
                    this->snake[0].x = 1;
                break;
            case LEFT:
                this->snake[0].x--;
                if(this->snake[0].x < 0)
                    this->snake[0].x = this->x;
                break;
            case DOWN:
                this->snake[0].y++;
                if(this->snake[0].y >= this->y)
                    this->snake[0].y = 1;
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
                    console::gotoxy(0,0);
                    Sleep(100);
                    this->printField(false);
                    console::gotoxy(0,0);
                    Sleep(100);
                }
                console::cls();
                cout << "Game Over.\n  Score: " << this->score.show() << endl;
                Sleep(2000);
                this->score.setName();
                this->score.saveScore();
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
        if(this->steep > 100){
            this->steep -= 10;
        }
        this->score.increase(1);
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

};
int main()
{
    menu m;
    while(true){
        m.checkInput();
        if(m.isSelected()){
            switch(m.selectedItem()){
                case NEW_GAME:{
                    game g(30,20);
                    while(true){
                        g.step();
                        g.printField(true);
                        if(g.gameOver())
                            break;
                    }
                    break;
                }
                case H_SCORE:{
                    score s;
                    s.showHighScore();
                    break;
                }
                case EXIT:{
                    console::cls();
                    cout << "Bye!";
                    Sleep(1000);
                    return 0;
                    break;
                }
            }
        }
        m.printMenu();

    }
    return 0;
}
