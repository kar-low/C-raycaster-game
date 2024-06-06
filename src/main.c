#include "raylib.h"
#include "textures.h"
#include <math.h>
#include <stdio.h>
#include "texWalls.data"
#include "texFloors.data"

#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

int mapH=16, mapW=8;

int map[] = {
  1,1,1,1,1,1,1,1,
  1,1,0,1,0,0,0,1,
  1,1,0,1,0,0,0,1,
  1,1,0,0,0,0,0,1,
  1,1,1,1,0,0,0,1,
  1,1,1,2,0,0,0,2,
  1,1,1,1,0,0,0,1,
  1,1,1,1,1,1,0,1,
  1,1,1,1,1,1,0,1,
  1,1,0,1,0,0,0,1,
  1,1,0,1,0,0,0,1,
  1,1,0,0,0,1,0,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,0,0,0,1,
  1,1,1,1,1,1,1,1,
};

int mapfloor[] = {
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,1,1,1,0,
  0,0,0,0,1,0,2,0,
  0,0,0,0,1,0,1,0,
  0,0,0,0,1,0,1,0,
  0,0,0,0,1,1,1,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
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

int *MapTex(int a){
  return texW[a-1];
}

int *FloorTex(int a){
  return texF[a];
}


int screenH, screenW;
float indexNecega, scaleUIindex; 
bool inDoorRange;
int doorMapLocation;
bool enableUI;



bool collisionCheck(float _pX, float _pY){
  return map[(int)_pX/64+(int)_pY/64*8]==0;
}
Color white = {255,255,255,255};
Color black = {0,0,0,255};
Color selectedColor;

float pX, pY, pa=0, pdx, pdy;
int movespeed=2;

void OpenDoor(int mapLoc){
  map[mapLoc]=0;
}

void inputs(){
  if (IsKeyDown(KEY_LEFT)) {pa-=0.05; if(pa<0) {pa+=2*PI;} pdx=cos(pa)*movespeed; pdy=sin(pa)*movespeed;}
  if (IsKeyDown(KEY_RIGHT)){pa+=0.05; if(pa>2*PI) {pa-=2*PI;} pdx=cos(pa)*movespeed; pdy=sin(pa)*movespeed;}
  if (IsKeyDown(KEY_UP)) if(collisionCheck(pX+pdx,pY+pdy)){pX += pdx; pY += pdy;}
  if (IsKeyDown(KEY_DOWN)) if(collisionCheck(pX-pdx,pY-pdy)){pX -= pdx; pY -= pdy;}
  if (IsKeyDown(KEY_E) && inDoorRange) {OpenDoor(doorMapLocation);}

}

void DrawUI(){
  if(enableUI){
  DrawRectangle(260*scaleUIindex,580*scaleUIindex,200.0f*scaleUIindex,40.0f*scaleUIindex,(Color){255,255,255,255});
  DrawText("OPEN DOOR", 275*scaleUIindex, 585*scaleUIindex, 30*scaleUIindex, (Color){0,0,0,255});  
  }
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
int mapRaycast; //return mapID of block hit with centerRay;
int mapRaycastIndex;
int depth[180];
void DrawRays(){
  int colorID;
  int r,mx,my,mp,dof; float rx,ry,xo,yo;
  double ra;
  int dofMax=16;
  float fogRange=500;
  
  int mapRaycastIndexH, mapRaycastIndexV;


  ra=pa-DR*30; if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;}
 Color color = {0,0,120,255};
//horizontal lines
  for(r=0; r<180; r++){
    int mv=0,mh=0;
    float disT;
    dof=0;
    float disH=1000000, hx=pX, hy=pY;
    float aTan=-1/tan(ra);
    if(ra>PI){ry=(((int)pY>>6)<<6)-0.0001; rx=(pY-ry)*aTan+pX; yo=-64; xo=-yo*aTan;}
    if(ra<PI){ry=(((int)pY>>6)<<6)+64; rx=(pY-ry)*aTan+pX; yo=64; xo=-yo*aTan;}
    if(ra==0 || ra==PI){rx=pX;ry=pY; dof=dofMax;}
    while(dof<dofMax){
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapW+mx;
      if(mp>0 && mp<mapH*mapW && map[mp]>0) {hx=rx; hy=ry; disH=dist(pX,pY,hx,hy);dof=dofMax;mh=map[mp];mapRaycastIndexH=mp;}
      else {rx+=xo;ry+=yo; dof+=1;}
    }
   
//vertical lines
    dof=0;
    float disV=1000000, vx=pX, vy=pY;
    float nTan=-tan(ra);
    if(ra>P2 && ra<P3){rx=(((int)pX>>6)<<6)-0.0001; ry=(pX-rx)*nTan+pY; xo=-64; yo=-xo*nTan;}
    if(ra<P2 || ra>P3){rx=(((int)pX>>6)<<6)+64; ry=(pX-rx)*nTan+pY; xo=64; yo=-xo*nTan;}
    if(ra==0 || ra==PI){rx=pX;ry=pY; dof=dofMax;}
    while(dof<dofMax){
      mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*mapW+mx;
      if(mp>0 && mp<mapH*mapW && map[mp]>0) {vx=rx; vy=ry; disV=dist(pX,pY,vx,vy);dof=dofMax; mv=map[mp];mapRaycastIndexV=mp;}
      else {rx+=xo;ry+=yo; dof+=1;}
    }
        //(Color){0,0,fmax(130-disT/2,0),255};
    if(disV<disH){rx=vx;ry=vy;disT=disV;color=MapColor(mv);colorID=mv; mapRaycast=mv;mapRaycastIndex=mapRaycastIndexV;}
    if(disH<disV){rx=hx;ry=hy;disT=disH;color=MapColor(mh);colorID=mh; mapRaycast=mh;mapRaycastIndex=mapRaycastIndexH;}
    


    float ca=pa-ra; if(ca<0) {ca+=2*PI;} if(ca>2*PI) {ca-=2*PI;} disT*=cos(ca);
    int lineH = ((screenH/10)*screenH)/disT; 
    

    
//WALLS
    float ty_step=32.0f/(float)lineH;
    float ty_off=0; 
    if(lineH>screenH) {ty_off=(lineH-screenH)/2.0; lineH=screenH;}
    
    int lineO=(screenH/2)-(lineH>>1);
    float ty=ty_off*ty_step;
    float tx=0;
    if(disV<disH) {tx=(int)(ry/2.0)%32; if(ra>P2 && ra<P3){tx=31-tx;}}
    else {tx=(int)(rx/2.0)%32; if(ra>PI){tx=31-tx;}}
    if(colorID>0){
    for(int h=0;h<lineH;h++){
      int rVal = MapTex(1)[((int)(ty)*32 + (int)(tx))*3];
      int gVal = MapTex(1)[((int)(ty)*32 + (int)(tx))*3+1];
      int bVal = MapTex(1)[((int)(ty)*32 + (int)(tx))*3+2];
      color = (Color){rVal,gVal,bVal,255};

      color.r=disT<fogRange ? (1-disT/fogRange)*color.r : 0;
      color.g=disT<fogRange ? (1-disT/fogRange)*color.g : 0;
      color.b=disT<fogRange ? (1-disT/fogRange)*color.b : 0;
      
      if(disV<disH) color=(Color){color.r-(color.r>>2),color.g-(color.g>>2),color.b-(color.b>>2),255};

      DrawLineEx((Vector2){r*indexNecega*(9.0f/16.0f),h+lineO},(Vector2){(r+1)*indexNecega*(9.0f/16.0f),h+lineO}, 1, color);
      if(colorID==2){
        rVal = MapTex(colorID)[((int)(ty)*32 + (int)(tx))*3];
        gVal = MapTex(colorID)[((int)(ty)*32 + (int)(tx))*3+1];
        bVal = MapTex(colorID)[((int)(ty)*32 + (int)(tx))*3+2];
        if(rVal!=255 && gVal!=0 && bVal!=255){
          color = (Color){rVal,gVal,bVal,255};
          color.r=disT<fogRange ? (1-disT/fogRange)*color.r : 0;
          color.g=disT<fogRange ? (1-disT/fogRange)*color.g : 0;
          color.b=disT<fogRange ? (1-disT/fogRange)*color.b : 0;
          if(disV<disH) color=(Color){color.r-(color.r>>2),color.g-(color.g>>2),color.b-(color.b>>2),255};
          DrawLineEx((Vector2){r*indexNecega*(9.0f/16.0f),h+lineO},(Vector2){(r+1)*indexNecega*(9.0f/16.0f),h+lineO}, 1, color);
        }
      }
      ty+=ty_step;
    }
    }


    //FLOOR 
    float tileSize=0.5;

    for(int h=lineO+lineH;h<screenH;h++){

      float dy=h-(screenH/2.0);
      tx=pX*tileSize + cos(ra)*((screenH*tileSize)*(indexNecega/7.5f))*32/dy/cos(ca);
      ty=pY*tileSize + sin(ra)*((screenH*tileSize)*(indexNecega/7.5f))*32/dy/cos(ca);
      
      int mp=mapfloor[(int)(ty/32.0f)*mapW+(int)(tx/32.0f)];
      int rVal = FloorTex(mp)[(((int)(ty)&31)*32 + ((int)(tx)&31))*3];
      int gVal = FloorTex(mp)[(((int)(ty)&31)*32 + ((int)(tx)&31))*3+1];
      int bVal = FloorTex(mp)[(((int)(ty)&31)*32 + ((int)(tx)&31))*3+2];
      color = (Color){rVal,gVal,bVal,255};
      color.r=fmax((((float)h/screenH)-0.4),0)*color.r;
      color.g=fmax((((float)h/screenH)-0.4),0)*color.g;
      color.b=fmax((((float)h/screenH)-0.4),0)*color.b;
      DrawLineEx((Vector2){r*indexNecega*(9.0f/16.0f),h},(Vector2){(r+1)*indexNecega*(9.0f/16.0f),h}, 1, color); 
    }


    //ROOF
    for(int h=0;h<lineO;h++){

      float dy=h-(screenH/2.0);
      tx=pX*tileSize - cos(ra)*((screenH*tileSize)*indexNecega/7.5f)*32/dy/cos(ca);
      ty=pY*tileSize - sin(ra)*((screenH*tileSize)*indexNecega/7.5f)*32/dy/cos(ca);
  
      int c = Textures[((int)(ty)&31)*32 + ((int)(tx)&31)]*255;
      color = (Color){c,c,c,255};
      DrawLineEx((Vector2){r*indexNecega*(9.0f/16.0f),h},(Vector2){(r+1)*indexNecega*(9.0f/16.0f),h}, 1, color); 
    }


      ra+=(DR/3.0); if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;}
    if(r==90) {CenterRayDist=disT;
      //printf("%i %f\n", colorID, CenterRayDist);
    if(CenterRayDist<50 && mapRaycast==2){
      enableUI=true;
      doorMapLocation=mapRaycastIndex; 
      inDoorRange=true;
      
    }
    else{
      enableUI=false;
      inDoorRange=false;
    }}
    depth[r]=disT;
   // DrawLine(pX,pY,rx,ry,(Color){255,0,0,255});

  }
}


typedef struct{
  int type;
  int state;
  int map;
  int x,y,z;
} sprite; sprite sp[4];

void drawSprites(){
  float sx=sp[0].x-pX;
  float sy=sp[0].y-pY;
  float sz=sp[0].z;
  float cs=-cos(pa), sn=sin(pa);
  float a=sy*cs+sx*sn;
  float b=sx*cs-sy*sn;

  sx=a;
  sy=b;
  int x,y;
  sx=(sx*942/sy)+(800.0f/2.0f);
  sy=(sz*942/sy)+(954.0f/2.0f);
  int scale=-(32.0f*80.0f/b);
  if(b<depth[90])
  DrawRectangle(sx,sy,1*scale,1*scale,(Color){255,0,0,255});
  printf("sx:%f sy:%i\n",sx-scale/2,depth[90]);
}



int main(){
  screenW=853;
  screenH=800;
  InitWindow(1920,1080, "test");
  indexNecega=7.5f*((float)screenH/640.0f);
  scaleUIindex=indexNecega/7.5f;
  SetTargetFPS(60);
  pX=400;
  pY=400;
  printf("%f\n",scaleUIindex);
  sp[0].x=2.5f*64; sp[0].y=1.5f*64; sp[0].z=1;
  pdx=2;
  pdy=0;
   while(WindowShouldClose()==false){
    BeginDrawing();
    ClearBackground((Color){5,5,5,255});
    inputs();

    //DrawMap();

    DrawRays();
    //DrawPlayer();
    //drawSprites();
    DrawUI();
    EndDrawing();

  }
 
  CloseWindow();
  return 0;
}
