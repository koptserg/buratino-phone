// =============================================================================
// =============================================================================

#include <Adafruit_GFX.h>
//#include <Adafruit_ST7735.h>
//#include <SPI.h>
#include <TouchScreen.h>
#include "ILI9488.h"

Adafruit_GFX_Button buttons[4];

// initialize Sainsmart 1.8" TFT screen
// (connect pins accordingly or change these values)
//#define TFT_DC            9     // Sainsmart RS/DC
//#define TFT_RST           8     // Sainsmart RES
//#define TFT_CS           10     // Sainsmart CS
// initialize screen with pins
//static Adafruit_ST7735 TFT = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_RS A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RST A4 // Can alternately just connect to Arduino's reset pin

ILI9488 TFT(LCD_CS, LCD_RS, LCD_WR, LCD_RD, LCD_RST);

    const uint8_t YP   = A2;                               // Вывод Y+ должен быть подключен к аналоговому входу
    const uint8_t XM   = A3;                               // Вывод X- должен быть подключен к аналоговому входу
    const uint8_t YM   = 8;                                // Вывод Y-
    const uint8_t XP   = 9;                                // Вывод X+
// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 65
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

TouchScreen ts     = TouchScreen(XP, YP, XM, YM);      // Создаём объект для работы с TouchScreen

// instead of using TFT.width() and TFT.height() set constant values
// (we can change the size of the game easily that way)
#define TFTW            320//128     // screen width
#define TFTH            450//160     // screen height
#define TFTW2            160//64     // half screen width
#define TFTH2            225//80     // half screen height
// game constant
#define SPEED             1
#define GRAVITY         2.8
#define JUMP_FORCE     0.5//2.15
#define SKIP_TICKS     20.0     // 1000 / 50fps
#define MAX_FRAMESKIP     1//5
// bird size
#define BIRDW            16     // bird width
#define BIRDH            16     // bird height
#define BIRDW2           BIRDW/2     // half width
#define BIRDH2           BIRDH/2     // half height
#define BIRDX            50    // x posizion
// pipe size
uint8_t PIPEW      =      24 ;//12     // pipe width
uint16_t GAPHEIGHT  =      60 ;    // pipe gap height
// prize
uint16_t PRZY       =     10;
boolean PRZON       =     1;
// prize1
int      PRZ1X       =     160;
uint16_t PRZ1Y       =     120;
uint8_t  PRZW        =      8;
uint8_t  PRZH        =      8;
//brick
int      BRICKX       =     100;
uint16_t BRICKY       =     0;
uint8_t  BRICKW       =     8;
uint8_t  BRICKH       =     8;
uint8_t  SPEEDBRICK   =     3;
boolean  BRICKON      =     1;

// floor size
uint16_t FLOORH     =      40 ;//20     // floor height (from bottom of the screen)
// grass size
#define GRASSH            BIRDH+2//4     // grass height (inside floor, starts at floor y)

uint16_t GAMEH     =       TFTH-FLOORH;

// background
const unsigned int BCKGRDCOL = TFT.Color565(138,235,244);
// bird
const unsigned int BIRDCOL = TFT.Color565(255,254,174);
// pipe
const unsigned int PIPECOL  = TFT.Color565(99,255,78);
// pipe highlight
const unsigned int PIPEHIGHCOL  = TFT.Color565(250,255,250);
// pipe seam
const unsigned int PIPESEAMCOL  = TFT.Color565(0,0,0);
// floor
const unsigned int FLOORCOL = TFT.Color565(246,240,163);
// grass (col2 is the stripe color)
const unsigned int GRASSCOL  = TFT.Color565(141,225,87);
const unsigned int GRASSCOL2 = TFT.Color565(156,239,88);

// bird sprite
// bird sprite colors (Cx name for values to keep the array readable)
#define  ST7735_BLACK       0x0000  ///<   0,   0,   0

#define C0 BCKGRDCOL
#define C1 TFT.Color565(195,165,75)
#define C2 BIRDCOL
#define ST7735_WHITE       0xFFFF  ///< 255, 255, 255
#define C3 ST7735_WHITE
#define ST7735_RED         0xF800  ///< 255,   0,   0
#define C4 ST7735_RED
#define C5 TFT.Color565(251,216,114)
static unsigned int birdcol[] =
  { 
  C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0,
  
  C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0
  };

// bird structure
static struct BIRD {
  int x, y, old_y;
  unsigned int col;
  float vel_y;
} bird;
// pipe structure
static struct PIPE {
//  char x, gap_y;
  int x;
  int gap_y;
  unsigned int col;
} pipe;
// prize1 structure
static struct PRIZE1 {
//  char x, y;
  int x;
  int y;
  unsigned int col;
} prize1;
// brick structure
static struct BRICK {
//  char x, y;
  int x;
  int y;
  int old_y;
  unsigned int col;
} brick;

// live
static short live = 5;
// score
static short score;
// prize
static short prize;
// temporary x and y var
static short tmpx, tmpy;
// temporary bx and by var
static short tmpbx, tmpby;

// ---------------
// draw pixel
// ---------------
// faster TFT.drawPixel method by inlining calls and using setAddrWindow and pushColor
// using macro to force inlining
//#define TFT.drawPixel(a, b, c) TFT.setAddrWindow(a, b, a, b); TFT.pushColors(c,8,1)

// ---------------
// initial setup
// ---------------
void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("Flappy"); 
  // initialize the push button on pin 2 as an input
//  DDRD &= ~(1<<PD2);
  pinMode(19, INPUT); // A5 определяем как цифровой вход
  // initialize a ST7735S chip, black tab
//  TFT.initR(INITR_BLACKTAB);
  TFT.begin();
  TFT.setRotation(2);
  DISPpower(1);
}

// ---------------
// main loop
// ---------------
void loop() {
  game_start();
  while (live != 0) {
    game_init();  
    game_loop();
  }  
  game_over();  
}

// ---------------
// game loop
// ---------------
void game_loop() {
  // ===============
  // prepare game variables
  // draw floor
  // ===============
  // instead of calculating the distance of the floor from the screen height each time store it in a variable
  //if ( (FLOORH+10) > (TFTH - bird.y)) {FLOORH = TFTH - bird.y - 20;}
  drawfloor();
  // grass x position (for stripe animation)
  int grassx = TFTW;
  // game loop time variables
  double delta, old_time, next_game_tick, current_time;
  next_game_tick = current_time = millis();
  int loops;
  // passed pipe flag to count score
  bool passed_pipe = false;
  // passed prize flag to count score
  bool passed_prize = false;
  bool passed_prize_1 = false;
  // temp var for setAddrWindow
  int px;
  int pbx;
  
  while (1) {
    loops = 0;
    while( millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      // ===============
      // input
      // ===============
//      if ( !(PIND & (1<<PD2)) ) {
      uint8_t bdis = digitalRead(19);
      if ( bdis == 0 or presstouch()) {
        // if the bird is not too close to the top of the screen apply jump force
        if (bird.y > BIRDH2*0.5) bird.vel_y = -JUMP_FORCE;
        // else zero velocity
        else bird.vel_y = 0;
      }
      
      // ===============
      // update
      // ===============
      // calculate delta time
      // ---------------
      old_time = current_time;
      current_time = millis();
      delta = (current_time-old_time)/1000;

      // bird
      // ---------------
      bird.vel_y += GRAVITY * delta;
      bird.y += bird.vel_y;
      
      // pipe
      // ---------------
      pipe.x -= SPEED;
      prize1.x -= SPEED;
      brick.x -= SPEED;
      // if pipe reached edge of the screen reset its position and gap
//      if ((prize1.x > brick.x && prize1.x < -PRZW) or (brick.x > prize1.x && brick.x < -BRICKW)) { 
      if (prize1.x < -PRZW) {
        TFT.fillRect(0, GAMEH, TFTW, FLOORH, BCKGRDCOL);
        pipe.x = TFTW;
        PIPEW = random(20, TFTW2/2);
        pipe.gap_y = random(10, GAMEH-(10+GAPHEIGHT));
        FLOORH = random(10, TFTH - pipe.gap_y - 30);
        if ( (FLOORH+10) > (TFTH - bird.y)) {FLOORH = TFTH - bird.y - 20;}
        GAPHEIGHT = random(BIRDH*3, TFTH-pipe.gap_y-FLOORH-20);
        if ( (FLOORH+10) > (TFTH - pipe.gap_y-GAPHEIGHT)) {FLOORH = TFTH - pipe.gap_y - GAPHEIGHT - 20;}
        PRZY = random(10, GAPHEIGHT-BIRDH-10);
        PRZ1X = random(PIPEW+50, TFTW-20);
        PRZ1Y = random(10, TFTH-FLOORH-20);
        uint8_t przint = random(0, 10);
        if (przint >= 5) PRZON = 1;
        if (przint < 5) PRZON = 0;
        drawfloor();
        passed_prize = false;
        passed_prize_1 = false;
        prize1.x = TFTW+PRZ1X;
        int brickint = random(-1,1);
        if (brickint==0) {BRICKON = 0; BRICKX = 0;}
        if (brickint > 0) {BRICKON = 1; BRICKX = random(PIPEW+BRICKW, PRZ1X-BRICKW);}
        if (brickint < 0) {BRICKON = 1; BRICKX = random(-TFTW+BIRDX+BIRDW+40, 0-BRICKW);}
//        BRICKX = random(-TFTW+BIRDX+BIRDW+20, PRZ1X-20);
//        BRICKX = random(random(-TFTW+BIRDX+BIRDW+BIRDW+20, 0-BRICKW), random(PIPEW+BRICKW+BRICKW, PRZ1X-BRICKW));
        brick.x = TFTW+BRICKX;
//        brick.y = BRICKY;
      }
      if (brick.x < -BRICKH or brick.y > GAMEH-GRASSH) {
//        brick.x = TFTW+BRICKX;
        brick.y = BRICKY;
        SPEEDBRICK = random(2,4);
      }
      // ---------------
      next_game_tick += SKIP_TICKS;
      loops++;
    }

    // ===============
    // draw
    // ===============
    // draw prize
    // ---------------
    if (pipe.x >= 0 && pipe.x < TFTW && !passed_prize && PRZON) {
        TFT.drawFastVLine(pipe.x+(PIPEW/2)-(PRZW/2), pipe.gap_y+PRZY+bird.vel_y, PRZH, ST7735_RED);
        TFT.drawFastVLine(pipe.x+(PIPEW/2)-(PRZW/2)+PRZW, pipe.gap_y+PRZY+bird.vel_y, PRZH, BCKGRDCOL);
    }
    if (pipe.x >= 0 && pipe.x < TFTW && passed_prize && PRZON) {
      if (bird.x < pipe.x+(PIPEW/2)-(PRZW/2)+PRZW && bird.y != pipe.gap_y+PRZY) {
         TFT.drawFastVLine(pipe.x+(PIPEW/2)-(PRZW/2), pipe.gap_y+PRZY, PRZH, BIRDCOL);
         TFT.drawFastVLine(pipe.x+(PIPEW/2)-(PRZW/2)+PRZW, pipe.gap_y+PRZY, PRZH, BCKGRDCOL);
       }
    }
    // erase behind prize
    if (pipe.x <= TFTW && PRZON) TFT.drawFastVLine(pipe.x+(PIPEW/2)-(PRZW/2)+PRZW, pipe.gap_y+PRZY, PRZH, BCKGRDCOL);
    
    // draw prize 1
    if (prize1.x >= 0 && prize1.x < TFTW && !passed_prize_1) {
      TFT.drawFastVLine(prize1.x, PRZ1Y-bird.vel_y, PRZH, ST7735_RED);
      TFT.drawFastVLine(prize1.x+PRZW, PRZ1Y-bird.vel_y, PRZH, BCKGRDCOL);
    }
    if (prize1.x >= 0 && prize1.x < TFTW && passed_prize_1) {
      if (bird.x < prize1.x+PRZW && bird.y != PRZ1Y) {
      TFT.drawFastVLine(prize1.x, PRZ1Y, PRZH, BIRDCOL);
      TFT.drawFastVLine(prize1.x+PRZW, PRZ1Y, PRZH, BCKGRDCOL);
       }
    }
    // erase behind prize1
    if (prize1.x <= TFTW+PRZ1X) TFT.drawFastVLine(prize1.x+PRZW, 0, GAMEH, BCKGRDCOL);

    // draw brick

    if (brick.y >=0 && brick.y < GAMEH && brick.x >= 0 && brick.x < TFTW && BRICKON) {
      brick.y += SPEEDBRICK;
         TFT.drawFastVLine(brick.x, brick.y, BRICKH, ST7735_BLACK);
         TFT.drawFastVLine(brick.x+BRICKW, brick.y, BRICKH, BCKGRDCOL);
    }

    // erase behind brick
    if (brick.x <= TFTW+BRICKX && BRICKON) {TFT.drawFastVLine(brick.x+BRICKW, 0, GAMEH, BCKGRDCOL);}
        
    // ===============
    // draw
    // ===============
    // pipe
    // ---------------
    // we save cycles if we avoid drawing the pipe when outside the screen
    if (pipe.x >= 0 && pipe.x < TFTW) {
      // pipe color
      TFT.drawFastVLine(pipe.x+3, 0, pipe.gap_y, PIPECOL);
      TFT.drawFastVLine(pipe.x+3, pipe.gap_y+GAPHEIGHT+1, GAMEH-(pipe.gap_y+GAPHEIGHT+1), PIPECOL);
      // highlight
      TFT.drawFastVLine(pipe.x, 0, pipe.gap_y, PIPEHIGHCOL);
      TFT.drawFastVLine(pipe.x, pipe.gap_y+GAPHEIGHT+1, GAMEH-(pipe.gap_y+GAPHEIGHT+1), PIPEHIGHCOL);
      // bottom and top border of pipe
      TFT.drawPixel(pipe.x, pipe.gap_y, PIPESEAMCOL);
      TFT.drawPixel(pipe.x, pipe.gap_y+GAPHEIGHT, PIPESEAMCOL);
      // pipe seam
      TFT.drawPixel(pipe.x, pipe.gap_y-6, PIPESEAMCOL);
      TFT.drawPixel(pipe.x, pipe.gap_y+GAPHEIGHT+6, PIPESEAMCOL);
      TFT.drawPixel(pipe.x+3, pipe.gap_y-6, PIPESEAMCOL);
      TFT.drawPixel(pipe.x+3, pipe.gap_y+GAPHEIGHT+6, PIPESEAMCOL);
    }
    // erase behind pipe
    if (pipe.x <= TFTW) TFT.drawFastVLine(pipe.x+PIPEW, 0, GAMEH, BCKGRDCOL);

    // bird
    // ---------------
    tmpx = BIRDW-1;
    do {   
 /*         px = bird.x+tmpx+BIRDW;
          // clear bird at previous position stored in old_y
          // we can't just erase the pixels before and after current position
          // because of the non-linear bird movement (it would leave 'dirty' pixels)
          tmpy = BIRDH - 1;
          
            do {
           TFT.drawPixel(px, bird.old_y + tmpy, BCKGRDCOL);
          } while (tmpy--);
          // draw bird sprite at new position
          tmpy = BIRDH-1;
          do {
            TFT.drawPixel(px, bird.y + tmpy, birdcol[tmpx + (tmpy * BIRDW)]);
          } while (tmpy--);
*/
            px = bird.x+tmpx;
///erase bird rasrt
            TFT.drawFastVLine(px, bird.old_y, BIRDH, BCKGRDCOL);
// draw bird rastr
            if (tmpx<BIRDH) {
//              TFT.drawFastVLine(px, bird.y, BIRDH, BIRDCOL);
              TFT.drawFastVLine(px, bird.y+tmpx/2, BIRDH/2-(tmpx/2), BIRDCOL);
              TFT.drawFastVLine(px, bird.y+BIRDH/2, BIRDH/2-(tmpx/2), BIRDCOL);
            }
            if (tmpx==BIRDH-1) {
              TFT.drawPixel(px, bird.y+BIRDH/2, ST7735_RED);
            }
//

    } while (tmpx--);
    // save position to erase bird on next draw
    bird.old_y = bird.y;

    // grass stripes
    // ---------------
    grassx -= SPEED;
    if (grassx < 0) grassx = TFTW;
    TFT.drawFastVLine( grassx    %TFTW, GAMEH+1, GRASSH-1, GRASSCOL);
    TFT.drawFastVLine((grassx+64)%TFTW, GAMEH+1, GRASSH-1, GRASSCOL2);

    // ===============
    // collision
    // ===============
    // if the bird hit the ground game over
    if (bird.y > GAMEH) {crach(); break;}
    // checking for bird collision with pipe
    if (bird.x+BIRDW >= pipe.x && bird.x <= pipe.x+PIPEW) {
      // bird entered a pipe, check for collision
      if (bird.y < pipe.gap_y || bird.y+BIRDH > pipe.gap_y+GAPHEIGHT) {crach(); break;}
      else {passed_pipe = true;}
    }
    // if bird has passed the pipe increase score
    else if (bird.x > pipe.x+PIPEW-BIRDW && passed_pipe) {
      passed_pipe = false;
      // increase score since we successfully passed a pipe
      score++;
      tone(11, 1000, 20);
      drawscore();
    }
    // checking for bird collision with brick
    if (BRICKON && brick.x <= bird.x+BIRDW && brick.x > bird.x && brick.y+BRICKH >= bird.y && brick.y+BRICKH < bird.y+BIRDH) {
      crach(); break;
    }    
    // checking for bird collision with prize
    if (!passed_prize && PRZON &&
      ((bird.x+BIRDW > pipe.x+(PIPEW/2)&& bird.x+BIRDW < pipe.x+(PIPEW/2)-(PRZW/2)+PRZW) or (bird.x > pipe.x+(PIPEW/2) && bird.x < pipe.x+(PIPEW/2)-(PRZW/2)+PRZW)) 
      &&  
      ((bird.y+BIRDH > pipe.gap_y+PRZY && bird.y+BIRDH < pipe.gap_y+PRZY+PRZH) or (bird.y > pipe.gap_y+PRZY && bird.y < pipe.gap_y+PRZY+PRZH))
      )
      {
      passed_prize = true;
      prize++;
      tone(11, 1000, 20);
      drawscore();
    }
    // checking for bird collision with prize1
    if (!passed_prize_1 &&
      ((bird.x+BIRDW > prize1.x && bird.x+BIRDW < prize1.x+PRZW) or (bird.x > prize1.x && bird.x < prize1.x+PRZW))
      && 
      ((bird.y+BIRDH > PRZ1Y && bird.y+BIRDH < PRZ1Y+PRZH) or (bird.y > PRZ1Y && bird.y < PRZ1Y+PRZH))
      )
      {
      passed_prize_1 = true;
      prize++;
      tone(11, 1000, 20);
      drawscore();
    }
    // checking for bird collision with prize1
//    passed_brick = true;  
  }
  
  // add a small delay to show how the player lost
//  delay(1200);
  live --;
  drawscore();
  while (1) {
    // wait for push button
    uint8_t bdis = digitalRead(19);
    if ( bdis == 0 or presstouch()) break;
  }
}

// ---------------
// game start
// ---------------
void game_start() {
  TFT.fillScreen(ST7735_BLACK);
  TFT.fillRect(10, TFTH2 - 20, TFTW-20, 1, ST7735_WHITE);
  TFT.fillRect(10, TFTH2 + 32, TFTW-20, 1, ST7735_WHITE);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(3);
  // half width - num char * char width in pixels
  TFT.setCursor( TFTW2-(6*9), TFTH2 - 16);
  TFT.println("FLAPPY");
  TFT.setTextSize(3);
  TFT.setCursor( TFTW2-(6*9), TFTH2 + 8);
  TFT.println("-BIRD-");
  TFT.setTextSize(0);
  TFT.setCursor( TFTW2 - (12*3) - 1, TFTH2 - 30);
  TFT.println("BuratinoPhone");
  TFT.setCursor( TFTW2 - (12*3) - 1, TFTH2 + 34);
  TFT.println("press button");
  while (1) {
    // wait for push button
    uint8_t bdis = digitalRead(19);
//    if ( !(PIND & (1<<PD2)) ) break;
    if ( bdis == 0 or presstouch()) break;
  }
  // reset score
  score = 0;
  live = 5;
  prize = 0;
}

// ---------------
// game init
// ---------------
void game_init() {
  // clear screen
  TFT.fillScreen(BCKGRDCOL);
  // 
  TFT.fillRect(0, TFTH+1, TFTW, 30, FLOORCOL);
  drawscore();
  // reset score
//  score = 0;
  // init bird
  bird.x = BIRDX;
//  bird.y = bird.old_y = TFTH2 - BIRDH;
  bird.y = bird.old_y = (TFTH - FLOORH)/2;
  bird.vel_y = -JUMP_FORCE;
  tmpx = tmpy = 0;
  // generate new random seed for the pipe gape
  randomSeed(analogRead(7));
  // init pipe
  pipe.x = TFTW;
  prize1.x = TFTW+PRZ1X;
  prize1.y = PRZ1Y;
  brick.x = TFTW+BRICKX;
  brick.y = BRICKY;
//  pipe.gap_y = random(20, TFTH-60);

  pipe.gap_y = random(10, GAMEH-(10+GAPHEIGHT));
}

void crach() {
      tone(11, 1000, 50);
      delay(100);
      tone(11, 2000, 50);
      delay(100);
      tone(11, 500, 50);
      delay(100);
}

void drawscore() {
  TFT.setTextColor(ST7735_BLACK,FLOORCOL);
  TFT.setCursor( TFTW2-140, TFTH+10);
  TFT.print("PRIZE: ");
  TFT.setTextColor(ST7735_RED,FLOORCOL);
  TFT.setCursor( TFTW2-100, TFTH+10);
  TFT.print(prize);
  
  TFT.setTextColor(ST7735_BLACK,FLOORCOL);
  TFT.setCursor( TFTW2-20, TFTH+10);
  TFT.print("SCORE: ");
  TFT.setTextColor(ST7735_RED,FLOORCOL);
  TFT.setCursor( TFTW2+20, TFTH+10);
  TFT.print(score);

  TFT.setTextColor(ST7735_BLACK,FLOORCOL);
  TFT.setCursor( TFTW2+100, TFTH+10);
  TFT.print("LIVE: ");
  TFT.setTextColor(ST7735_RED,FLOORCOL);
  TFT.setCursor( TFTW2+140, TFTH+10);
  TFT.print(live);
}

// ---------------
// game over
// ---------------
void game_over() {
  TFT.fillScreen(ST7735_BLACK);
  TFT.setTextColor(ST7735_WHITE);
  TFT.setTextSize(2);
  // half width - num char * char width in pixels
  TFT.setCursor( TFTW2 - (9*6), TFTH2 - 4);
  TFT.println("GAME OVER");
  TFT.setTextSize(0);
  TFT.setCursor( TFTW2 - (18*3), TFTH2 - 14);
  TFT.print("prize: ");
  TFT.print(prize);
  TFT.print(" score: ");
  TFT.print(score);
  TFT.setCursor( TFTW2 - (12*3), TFTH2 + 12);
  TFT.println("press button");
  while (1) {
    // wait for push button
//    if ( !(PIND & (1<<PD2)) ) break;
    uint8_t bdis = digitalRead(19);
    if ( bdis == 0 or presstouch()) break;
  }
  live = 5;
}

void DISPpower(uint8_t disp) {
  pinMode(10, OUTPUT);
  if (disp == 1){
    digitalWrite(10, LOW);
  }
  if (disp == 0){
    digitalWrite(10, HIGH);
  }
}

boolean presstouch()
{
       TSPoint p = ts.getPoint();  
       pinMode(XM, OUTPUT);                                   // Возвращаем режим работы вывода X- в значение «выход» для работы с дисплеем
       pinMode(YP, OUTPUT);                                   // Возвращаем режим работы вывода Y+ в значение «выход» для работы с дисплеем
       if (p.z>100) {
       //      Выводим «сырые» показания TouchScreen:
       // Serial.println((String) "X="+p.x+",\tY="+p.y);
       p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
       p.y = map(p.y, TS_MINY, TS_MAXY, 0, 480);
       return true;
       //Serial.println((String) "X="+p.x+",\tY="+p.y);
       } else {p.x = p.y = -1; return false;}
}
void drawfloor(){
   GAMEH = TFTH-FLOORH;
  // draw the floor once, we will not overwrite on this area in-game
  // black line
  TFT.drawFastHLine(0, GAMEH, TFTW, ST7735_BLACK);
  // grass and stripe
  TFT.fillRect(0, GAMEH+1, TFTW2, GRASSH, GRASSCOL);
  TFT.fillRect(TFTW2, GAMEH+1, TFTW2, GRASSH, GRASSCOL2);
  // black line
  TFT.drawFastHLine(0, GAMEH+GRASSH, TFTW, ST7735_BLACK);
  // mud
  TFT.fillRect(0, GAMEH+GRASSH+1, TFTW, FLOORH-GRASSH, FLOORCOL);
}
