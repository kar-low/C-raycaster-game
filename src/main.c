#include "raylib.h"
#include <math.h>
#include <stdio.h>


#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533


int map[] = {
  1,1,1,1,1,1,1,1,
  1,2,0,1,0,0,0,1,
  1,1,0,1,0,0,0,2,
  1,1,0,0,0,0,0,2,
  1,3,3,1,0,0,0,2,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,1,1,1,1,

};

Color MapColor(int a){
  switch(a){
    case 1:
      return (Color){0,0,120,255}; 
    break;
    case 2:
      return (Color){0,80,0,255};
    break;
    case 3:
      return (Color){100,65,23,255};
    break;

  }
}

int mapH=8, mapW=8;
int screenH, screenW;
bool collisionCheck(float _pX, float _pY){
  return map[(int)_pX/64+(int)_pY/64*8]==0;
}
Color white = {255,255,255,255};
Color black = {0,0,0,255};
Color selectedColor;

float pX, pY, pa=0, pdx, pdy;
int movespeed=2;
void inputs(){
  if (IsKeyDown(KEY_LEFT)) {pa-=0.05; if(pa<0) {pa+=2*PI;} pdx=cos(pa)*movespeed; pdy=sin(pa)*movespeed;}
  if (IsKeyDown(KEY_RIGHT)){pa+=0.05; if(pa>2*PI) {pa-=2*PI;} pdx=cos(pa)*movespeed; pdy=sin(pa)*movespeed;}
  if (IsKeyDown(KEY_UP)) if(collisionCheck(pX+pdx,pY+pdy)){pX += pdx; pY += pdy;}
  if (IsKeyDown(KEY_DOWN)) if(collisionCheck(pX-pdx,pY-pdy)){pX -= pdx; pY -= pdy;}
}



void DrawMap(){
  for(int i=0; i<8; i++){
    for(int j=0;j<8;j++){
      if(map[j+8*i]==0)
        selectedColor=black;
      else 
        selectedColor=white;
      DrawRectangle(j*64+1, i*64+1, 63, 63, selectedColor);
    }
  }
}

void DrawPlayer(){
  DrawRectangle(pX-8,pY-8, 16, 16, (Color){255,0,255,255});
  DrawLine(pX, pY, pX+pdx*1500, pY+pdy*1500, (Color){255,0,255,255});
}

float dist(float ax, float ay, float bx, float by){
  return sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
}

float CenterRayDist;
void DrawRays(){
  int r,mx,my,mp,dof; float rx,ry,ra,xo,yo;
  
  ra=pa-DR*30; if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;}
 Color color = {0,0,120,255};
//horizontal lines
  for(r=0; r<120; r++){
    int mv=0,mh=0;
    float disT;
    dof=0;
    float disH=1000000, hx=pX, hy=pY;
    float aTan=-1/tan(ra);
    if(ra>PI){ry=(((int)pY>>6)<<6)-0.0001; rx=(pY-ry)*aTan+pX; yo=-64; xo=-yo*aTan;}
    if(ra<PI){ry=(((int)pY>>6)<<6)+64; rx=(pY-ry)*aTan+pX; yo=64; xo=-yo*aTan;}
    if(ra==0 || ra==PI){rx=pX;ry=pY; dof=8;}
    while(dof<8){
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapW+mx;
      if(mp>0 && mp<mapH*mapW && map[mp]>0) {hx=rx; hy=ry; disH=dist(pX,pY,hx,hy);dof=8;mh=map[mp];}
      else {rx+=xo;ry+=yo; dof+=1;}
    }
   
//vertical lines
    dof=0;
    float disV=1000000, vx=pX, vy=pY;
    float nTan=-tan(ra);
    if(ra>P2 && ra<P3){rx=(((int)pX>>6)<<6)-0.0001; ry=(pX-rx)*nTan+pY; xo=-64; yo=-xo*nTan;}
    if(ra<P2 || ra>P3){rx=(((int)pX>>6)<<6)+64; ry=(pX-rx)*nTan+pY; xo=64; yo=-xo*nTan;}
    if(ra==0 || ra==PI){rx=pX;ry=pY; dof=8;}
    while(dof<8){
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapW+mx;
      if(mp>0 && mp<mapH*mapW && map[mp]>0) {vx=rx; vy=ry; disV=dist(pX,pY,vx,vy);dof=8; mv=map[mp];}
      else {rx+=xo;ry+=yo; dof+=1;}
    }
        //(Color){0,0,fmax(130-disT/2,0),255};
    if(disV<disH){rx=vx;ry=vy;disT=disV;color=MapColor(mv);}
    if(disH<disV){rx=hx;ry=hy;disT=disH;color=MapColor(mh); color.r=fmax(color.r-15, 0); color.g=fmax(color.g-15, 0);color.b=fmax(color.b-15, 0);}


    float ca=pa-ra; if(ca<0) {ca+=2*PI;} if(ca>2*PI) {ca-=2*PI;} disT*=cos(ca);
    float lineH = (64*screenH)/disT; if(lineH>screenH) lineH=screenH;
    float lineO=(screenH/2)-lineH/2;

    color.r=fmax(color.r-disT/5, 0);
    color.g=fmax(color.g-disT/5, 0);
    color.b=fmax(color.b-disT/5, 0);

    DrawLineEx((Vector2){r*30.0f*(9.0f/16.0f),lineO},(Vector2){r*30.0f*(9.0f/16.0f),lineH+lineO}, 30.0f*(9.0f/16.0f), color);
    ra+=(DR/2); if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;}
   if(r==60) CenterRayDist=disT;
   // DrawLine(pX,pY,rx,ry,(Color){255,0,0,255});

  }
}

void DrawUI(){
  DrawRectangle(440,50,200,80,(Color){255,255,255,255});
}
int main(){
  screenW=1920;
  screenH=1080;
  InitWindow(1920,1080, "test");
  
  SetTargetFPS(60);
  pX=400;
  pY=400;
  pdx=2;
  pdy=0;
   while(WindowShouldClose()==false){
    BeginDrawing();
    ClearBackground((Color){5,5,5,255});
    inputs();
    DrawRectangle(0,screenH/2,2000,screenH, (Color){10,9,2,255});
//  DrawMap();

    DrawRays();
    // DrawPlayer();

 //     DrawUI();
    EndDrawing();

  }
 
  CloseWindow();
  return 0;
}
