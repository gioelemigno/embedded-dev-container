// from: https://github.com/VolosR/M5Dial/tree/main

#include <Arduino.h>

#include "M5Dial.h"
M5Canvas img(&M5Dial.Display);

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

#include "Noto.h"
#include "smallFont.h"
#include "middleFont.h"
#include "bigFont.h"
#include "secFont.h"

#define color1 TFT_WHITE
#define color2 0x8410
#define color3 0x5ACB
#define color4 0x15B3
#define color5 0x00A3

unsigned short grays[12];

String cc[12]={"45","40","35","30","25","20","15","10","5","0","55","50"};
String days[]={"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};

float x[360];
float y[360];
float px[360];
float py[360];
float lx[360];
float ly[360];

int r=116;
int sx=120;
int sy=120;

int start[12];
int startP[60];

int rAngle=359;
int angle=0;

// hours, minutes, seconds
String h;
String m;
String s;

// date
String date = "12";
String d1 = "1";
String d2 = "2";

// month
String month = "34";
String m1 = "3";
String m2 = "4";

String weekday; 
String year; 


inline void setup_arrays(){
   int b=0;
   int b2=0;
   double rad=0.01745;

   // sx and sy are the center coordinates
   for(int i=0;i<360;i++){
      x[i]=((r-20)*cos(rad*i))+sx;
      y[i]=((r-20)*sin(rad*i))+sy;
      px[i]=(r*cos(rad*i))+sx;
      py[i]=(r*sin(rad*i))+sy;

      lx[i]=((r-6)*cos(rad*i))+sx;
      ly[i]=((r-6)*sin(rad*i))+sy;
      
      if(i%30==0){
         start[b]=i;
         b++;
      }

      if(i%6==0){
         startP[b2]=i;
         b2++;
      }
   }
}

inline void compute_grays_scale(){
   int co=210;
   for(int i=0;i<12;i++){ 
      grays[i]=tft.color565(co, co, co);
      co=co-20;
   }
}

void setup() {

   auto cfg = M5.config();
   M5Dial.begin(cfg, true, true);

   M5Dial.Rtc.setDateTime({ { 2023, 10, 25 }, { 15, 56, 56 } } );
   sprite.createSprite(240,240);

   sprite.setSwapBytes(true);    
   sprite.setSwapBytes(true);
   sprite.setTextDatum(4);

   setup_arrays();
   compute_grays_scale();
}


inline void write_seconds(){
   // write seconds
   sprite.loadFont(secFont);
   sprite.setTextColor(grays[1],TFT_BLACK);
   sprite.drawString(s,sx,sy-42);
   sprite.unloadFont();
}

inline void seconds_animation(){
   // set text font for yellow text and external circle
   sprite.loadFont(Noto);

   // set text color for external circle
   sprite.setTextColor(grays[3],TFT_BLACK);

   // draw tacks external circle
   for(int i=0;i<60;i++)
      if(startP[i]+angle<360)
         sprite.fillSmoothCircle(px[startP[i]+angle],py[startP[i]+angle],1,grays[4],TFT_BLACK);
      else
         sprite.fillSmoothCircle(px[(startP[i]+angle)-360],py[(startP[i]+angle)-360],1,grays[4],TFT_BLACK);

   // draw numbers external circle
   for(int i=0;i<12;i++)
      if(start[i]+angle<360){
         sprite.drawString(cc[i],x[start[i]+angle],y[start[i]+angle]);
         sprite.drawWedgeLine(px[start[i]+angle],py[start[i]+angle],lx[start[i]+angle],ly[start[i]+angle],2,2,grays[3],TFT_BLACK);
      }
      else{
         sprite.drawString(cc[i],x[(start[i]+angle)-360],y[(start[i]+angle)-360]);
         sprite.drawWedgeLine(px[(start[i]+angle)-360],py[(start[i]+angle)-360],lx[(start[i]+angle)-360],ly[(start[i]+angle)-360],2,2,grays[3],TFT_BLACK);
      }

   // // draw tacks external circle
   // int pos = 0;

   // for(int i=0; i<60; i++) {
   //    // Calculate the wrapped position once
   //    pos = (startP[i] + angle) % 360;
      
   //    // Draw the dot
   //    sprite.fillSmoothCircle(px[pos], py[pos], 1, grays[4], TFT_BLACK);
   // }

   // // draw numbers external circle
   // for(int i=0;i<12;i++){
   //    pos = (start[i] + angle) % 360; 

   //    sprite.drawString(cc[i], x[pos], y[pos]);
   //    sprite.drawWedgeLine(px[pos], py[pos], lx[pos], ly[pos], 2, 2, grays[3], TFT_BLACK);
   // }

   // write second tack
   sprite.drawWedgeLine(sx-1,sy-82,sx-1,sy-70,1,5,0xA380,TFT_BLACK);

   // draw moving red dot
   sprite.fillSmoothCircle(px[rAngle],py[rAngle],4,TFT_RED,TFT_BLACK);

   sprite.unloadFont();
}

inline void write_weekday(){
   // write weekday
   sprite.loadFont(smallFont);
   sprite.setTextColor(0x35D7,TFT_BLACK);
   sprite.drawString(weekday,80,72);
   sprite.drawString("DAY",160,72);
   sprite.unloadFont();
}

inline void write_date(){
   // write date
   sprite.fillRect(64,82,16,28,grays[8]);
   sprite.fillRect(84,82,16,28,grays[8]);
   sprite.fillRect(144,82,16,28,grays[8]);
   sprite.fillRect(164,82,16,28,grays[8]);
   sprite.loadFont(middleFont);
   sprite.setTextColor(grays[2],grays[8]);

   m1 = month[0];
   m2 = month[1];
   sprite.drawString(m1,71,99,2);
   sprite.drawString(m2,91,99,2);
   
   d1 = date[0];
   d2 = date[1];
   sprite.drawString(d1,150,99,2);
   sprite.drawString(d2,170,99,2);
   
   sprite.unloadFont();
}

inline void write_hours_minutes(){
   // write time
   sprite.loadFont(bigFont);
   sprite.setTextColor(grays[0],TFT_BLACK);
   sprite.drawString(h+":"+m,sx,sy+32);
   sprite.unloadFont();
}

inline void write_year(){
   // write text in yellow
   sprite.loadFont(Noto);
   sprite.setTextColor(0xA380,TFT_BLACK);
   sprite.drawString(year,120,190);
   sprite.drawString("***",120,114);
   sprite.unloadFont();
}

inline void zero_padding(int8_t num, String& dest){
   if(num < 10) 
      dest = "0" + String(num); 
   else 
      dest = String(num);
}

static constexpr const char* const wd[7] = { "SUN", "MON", "TUE", "WED", "THR", "FRI", "SAT" };

void loop() {
   auto dt = M5Dial.Rtc.getDateTime();

   rAngle=rAngle-3;
   angle=dt.time.seconds*6; 

   // M5Dial.Display.printf("%04d/%02d/%02d(%s)", dt.date.year, dt.date.month, dt.date.date, wd[dt.date.weekDay]);
   // M5Dial.Display.printf("%02d:%02d:%02d", dt.time.hours, dt.time.minutes, dt.time.seconds);

   month = dt.date.month;
   date = dt.date.date;
   weekday = wd[dt.date.weekDay];
   year = dt.date.year;

   zero_padding(dt.time.seconds, s);
   zero_padding(dt.time.hours, h);
   zero_padding(dt.time.minutes, m);


   if(angle>=360)
      angle=0;

   if(rAngle<=0){
      rAngle=359;  
   }


   // clean screen
   sprite.fillSprite(TFT_BLACK);

   write_seconds();
   seconds_animation();
   
   write_weekday();
   write_date();

   write_hours_minutes();

   write_year();

   // render
   M5Dial.Display.pushImage(0,0,240,240,(uint16_t*)sprite.getPointer());
}
