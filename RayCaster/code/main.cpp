//RayCaster in C++ by Rajas Sharma
//Graphics are made using TerminalGraphics by Rajas Sharma

#include <iostream>
#include "TGL.h"
#include <conio.h>
#define cellSide 12
#define playerRadius 2
#define playerSpeed 0.8
#define playerTurningSpeed 0.14232
#define playerXinit 4
#define playerYinit 5
#define rayNumber 400 
#define rayLength 100
#define raySteps 400
#define FOV 1
#define zoom 800
#define fish 0 
#define gradient 80
#define raySample 40

using namespace tglh;
TerminalGraphics tgl;

#define worldX 10
#define worldY 10

bool gamestate = true;

void swap(double &a, double &b){double tmp = a; a=b; b=tmp;}

class Ray{
private:
public:
    int stepCounter=0;
    double x;
    double y;
    double xhold;
    double yhold;
    double vx;double VX;
    double vy;double VY;
    double length;
    int steps;
    double angle;
    Ray(double x, double y,double length,int steps, double angle){
        this->x=x;
        this->y=y;
        this->length=length;
        this->steps=steps;
        this->angle=angle;
        vx = cos(angle)*length/steps;
        vy = sin(angle)*length/steps;
        xhold = x; yhold = y;
    }

    int checkRayCollision(){
        stepCounter=0;
        for(int i = 0;i<steps;i++){
            //if(0<=xhold&&xhold<tgl.wallx&&yhold>=0&&yhold<tgl.wally&&x>=0&&x<tgl.wallx&&y>=0&&y<tgl.wally){
            if(is_intersecting(xhold,yhold,x,y)){return stepCounter;}
            //}
            moveRay();
            stepCounter++;
        }
        //std::cout<<2<<'\n';
        return -1;
    }

    void moveRay(){
        xhold=x;
        yhold=y;
        x+=vx;
        y+=vy;
        //std::cout<<xhold<<' '<<yhold<<'\n';
    }

    bool is_intersecting(double x1,double y1,double x2, double y2){

        int steps = 75;

        int vx = x2 - x1; int vy = y2 - y1;
        double error = 0.01;
        double stepsizeX = (vx + error) / (steps + error); double stepsizeY = (vy + error) / (steps + error);
        for (int i = 0; i < steps + 1; i++) {
            int x = round(x1 + stepsizeX * i);
            int y = round(y1 + stepsizeY * i);
            if (x >= 0 && x < tgl.wallx
                && y >= 0 && y < tgl.wally) { //check if point in bounds
                if(tgl.grid[x][y] == 1){return true;}
            }
        }

        return false;

    }
};

class Player{
private:
public:
    double x;
    double y;
    double speed;
    double vx,vy;
    double turnSpeed;
    double theta=PI/4;
    Player(double x, double y,double speed, double turnSpeed){
        this->x=x*cellSide;this->y=y*cellSide;
        this->speed=speed;
        this->turnSpeed=turnSpeed;
    }
    
    void movePlayer(char _key){
        if(theta>2*PI){theta=0;}
        else if(theta<0){theta=2*PI;}
        if(_key=='w'){y+=vy;x+=vx;}
        if(_key=='a'){theta-=turnSpeed;}
        if(_key=='s'){y-=vy;x-=vx;}
        if(_key=='d'){theta+=turnSpeed;}
        vx=speed*cos(theta);
        vy=speed*sin(theta);
    }

    void drawPlayer(){
        if(y>=0&&x>=0&&x<tgl.wallx&&y<tgl.wally){
            tgl.ellipse(x,y,playerRadius,playerRadius,5,2);
            tgl.grid[x][y]=2;
            tgl.line(x,y,(x+vx*5),(y+vy*5),1,2);
        }
    }
};

Player p(playerXinit,playerYinit,playerSpeed,playerTurningSpeed);
std::vector<Ray> rayArray;

//ALWAYS INDEX WORLD WITH (y,x)
int world[worldY][worldX] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,0,0,1,0,0,0,0,0,1},
    {1,0,0,1,0,0,1,1,0,1},  
    {1,0,0,1,0,0,0,0,0,1},
    {1,1,0,0,0,1,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,0,1},
    {1,1,1,0,0,0,0,1,0,1},
    {1,0,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,1,1,1,1,1},
                                };

void drawMap(){
    for(int i = 0;i<worldY;i++){
        for(int j = 0;j<worldX;j++){
            if(world[i][j]==1){tgl.rect(j*cellSide,i*cellSide,(j+1)*cellSide,(i+1)*cellSide,1);}
        }
    }
}

void Ray__init__(){
    double angle=p.theta-FOV/2.0;
    double d = FOV/(rayNumber+0.01);
    for(int i = 0;i<rayNumber;i++){
        rayArray.push_back(Ray(p.x,p.y,rayLength,raySteps,angle));
        angle+=d;
        //std::cout<<angle<<'\n';
    }
}

//-------------------------------------------------------------------------------------------------------
//main function:
//-------------------------------------------------------------------------------------------------------



 int main(){
    tgl.setWindow(800,450);
    tgl.setTileset(" .S-`'.,-~_:;^+=*<>iv!lI?/|()1t{}[]rjfxnuczmwqpdbkhao#MW&8%B@$02345679ACDEFGHJKLNOPQRSTUVXYZegsy");
    tgl.setBorder();
    tgl.framerate = 1000;
    drawMap();
    Ray__init__();

    while(gamestate){ //main game loop

        if(_kbhit()){
            char _key = _getch();
            p.movePlayer(_key);
            if(_key==' '){std::exit(EXIT_FAILURE);/*gamestate = false;*/}
        } //check if user pressed space and exit if so

        tgl.clear(0);
        drawMap();
        
        for(int i =0;i<rayNumber;i++){
            double val = rayArray[i].checkRayCollision();
            //tgl.line(p.x,p.y,rayArray[i].x,rayArray[i].y,100,3);
            if(val!=-1){
                double d=hypot((double)(p.x-rayArray[i].x),(double)(p.y-rayArray[i].y));
                double middleX = tgl.wallx/2.0;
                double middleY = tgl.wally/2.0;
                double theta = rayArray[i].angle-p.theta;
                                
                if(d>0){  
                    double h = (zoom / d)+d*fish;
                    //NEED BETTER RECT FUNC                  
                   // tgl.fillrect((zoom)*(d*sin(theta)+height)+middleY,zoom*tgl.wallx/(rayNumber+0.01),(zoom)*d*sin(theta)+middleY,(zoom)*d*cos(theta)+middleX,80-(d/rayLength));
                   int d1 = gradient/d;
                   if(d1>80){d1=80;}
                    tgl.line(round(tgl.wallx*sin(theta)+middleX),round(middleY+h),round(tgl.wallx*sin(theta)+middleX),round(middleY-h),1000,(int)(80-d1));
                }
            }
        }

         for(int i =0;i<rayNumber;i+=raySample){
             tgl.line(p.x,p.y,rayArray[i].x,rayArray[i].y,100,3);
        }
        p.drawPlayer();
        
        rayArray.clear();
        Ray__init__();
        
        //drawMap();
        tgl.render();
        tgl.sleep();
        //std::cout<<gamestate<<'\n';
        //return -1;
    }
    return 0;
}
