
//#include <SPI.h>
#include <TouchScreen.h>
#include "ILI9488.h"
#include <Adafruit_GFX.h>    // Core graphics library
Adafruit_GFX_Button buttons[4];

// Color definitions
#define  BLACK       0x0000  ///<   0,   0,   0
#define  NAVY        0x000F  ///<   0,   0, 123
#define  DARKGREEN   0x03E0  ///<   0, 125,   0
#define  DARKCYAN    0x03EF  ///<   0, 125, 123
#define  MAROON      0x7800  ///< 123,   0,   0
#define  PURPLE      0x780F  ///< 123,   0, 123
#define  OLIVE       0x7BE0  ///< 123, 125,   0
#define  LIGHTGREY   0xC618  ///< 198, 195, 198
#define  DARKGREY    0x7BEF  ///< 123, 125, 123
#define  GRAY2       0x7BEF  ///< 123, 125, 123
#define  BLUE        0x001F  ///<   0,   0, 255
#define  GREEN       0x07E0  ///<   0, 255,   0
#define  LIGHTGREEN  0xAFE5
#define  CYAN        0x07FF  ///<   0, 255, 255
#define  RED         0xF800  ///< 255,   0,   0
#define  MAGENTA     0xF81F  ///< 255,   0, 255
#define  YELLOW      0xFFE0  ///< 255, 255,   0
#define  WHITE       0xFFFF  ///< 255, 255, 255
#define  ORANGE      0xFD20  ///< 255, 165,   0
#define  GREENYELLOW 0xAFE5  ///< 173, 255,  41
#define  PINK        0xFC18  ///< 255, 130, 198

//////config
#define ROWS                6
#define COLS                14

#define TILE_W              (int)((MAX_X + 1) / COLS)
#define TILE_H              (int)(TILE_W / 2.2)

#define BALL_R              2
#define BALL_SPEED_H        1
#define BALL_SPEED_V        1
#define BALL_COLOR          CYAN
#define BALL_MOVE_WAIT      7 //4

#define PADDLE_W            16
#define PADDLE_H            5
#define PADDLE_TOLERANCE    4
#define PADDLE_COLOR        LIGHTGREEN
#define PADDLE_MOVE_WAIT    7 // 5

#define BOARD_LEFT          (int)(2)
#define BOARD_RIGHT         (int)(MAX_X - 2)
#define BOARD_TOP           23

#define TILES_LEFT          (int)(((MAX_X + 1) - ( COLS * TILE_W ) )/ 2)
#define TILES_TOP           (BOARD_TOP + 2.8 * TILE_H)

#define TILE_IS_DRAWN       1
#define TILE_IS_NOT_DRAWN   0

#define SCOREBOARD_COLOR  LIGHTGREY //WHITE
#define BACKGROUND_COLOR  BLACK
/////////

  int beep;
  unsigned long last;
  
  //    Scoreboard
  #define GAME_LEVEL  1
  #define GAME_LIVES  5
  #define GAME_SCORE  0
  
  int level = GAME_LEVEL;
  int lives = GAME_LIVES;
  unsigned long score = GAME_SCORE;

const int COLORS[ROWS] = { RED, MAGENTA, OLIVE, YELLOW, GREEN, NAVY };
byte tiles[ROWS][COLS];
int tilesLeft;


#define FONT_SPACE 6
#define FONT_X 8
#define MIN_X 0
#define MIN_Y 0
#define MAX_X 319
#define MAX_Y 319

#include "beeping.cpp"
#include "sequencer.cpp"
//#include "breakout.cpp"
//#include "ball.cpp"
//#include "paddle.cpp"
#include "joystick.cpp"
//#include "config.h"
//#include "scoreboard.cpp"
//#include "tiles.cpp"


#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_RS A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RST A4 // Can alternately just connect to Arduino's reset pin

ILI9488 tft(LCD_CS, LCD_RS, LCD_WR, LCD_RD, LCD_RST);

const uint8_t YP   = A2;                               // Вывод Y+ должен быть подключен к аналоговому входу
const uint8_t XM   = A3;                               // Вывод X- должен быть подключен к аналоговому входу
const uint8_t YM   = 8;                                // Вывод Y-
const uint8_t XP   = 9;                                // Вывод X+
#define TS_MINX 65
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

TouchScreen ts = TouchScreen(XP, YP, XM, YM);      // Создаём объект для работы с TouchScreen

uint8_t disp = 1; // display on
uint8_t bb=5;

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("BuratinoBreakout!"); 

  tft.begin();
  tft.setRotation(2);
  DISPpower(disp);

  Joystick::init();

  Beeping::turnOn();
}

void loop() {
  
  drawWallTiles();

  char* bo ="BREAKOUT";
  const int len = strlen (bo) * FONT_SPACE * 6;
  const int left = (MAX_X - len ) / 2;

//  tft.drawStringWithShadow(bo, left, 78, 6, WHITE, BLACK);
  tft.setCursor(left, 78);
  tft.setTextColor(BLUE);  tft.setTextSize(6);
  tft.println(bo);

  boolean go = false;
  create_butt();
  
  delay(700);
  Sequencer::start();
  do
  {

//    tft.drawCenteredString("Press fire to play!", 136, 2, CYAN);
    tft.setCursor(50, 144);
    tft.setTextColor(YELLOW);  tft.setTextSize(2);
    tft.println("Press fire to play!");

    unsigned long last = millis();
    do
    {
      pressbutt();
      go = Joystick::fire() || bb ==1;
      
      if ( millis() - last > 8000 ) break;
      
    } while (!go);

  } while (!go);

  Sequencer::stop();

//  Breakout game;
  gamerun();
}

ISR(TIMER1_COMPA_vect) {

  // sequencer plays tetris theme
  Sequencer::play();
}

void drawWallTiles()
{
  for ( int c = 0; c <  320; c+=40)
  {
    
    tft.fillRect(c+0, 0, 40, 480, RED);

    for ( int i = 20; i < 480; i+= 20)
    {
      tft.drawFastHLine(c+0, i, 40, GRAY2);
      tft.drawFastHLine(c+0, i+1, 40, GRAY2);
    }

    for ( int i = 0; i < 480; i+= 40)
    {
      tft.drawFastVLine(c+20, i, 20, GRAY2 );
      tft.drawFastVLine(c+21, i, 20, GRAY2 );
      tft.drawFastVLine(c+39, i+20, 20, GRAY2 );
      tft.drawFastVLine(c+38, i+20, 20, GRAY2 );
    }
  }
}


void DISPpower(uint8_t disp) {
  pinMode(10, OUTPUT);
  if (disp == 1){
    digitalWrite(10, LOW);
  }
  if (disp == 0){
    digitalWrite(10, HIGH);
  }
//  ring(400, 50);
//  new_millis = millis();
}

/////////////////////// breakout

  Joystick js;
//  Ball ball;
//  Paddle paddle;
//  Scoreboard score;
//  Tiles tiles;

    void gamerun()
    {
//    Scoreboard
      level = GAME_LEVEL;
      lives = GAME_LIVES;
      score = GAME_SCORE;
      
      // blank board
      tft.fillRect(0, 0, 320, MAX_Y, BACKGROUND_COLOR);
      tft.fillRect(0,            0, MAX_X,             BOARD_TOP,            SCOREBOARD_COLOR);
      tft.fillRect(0,            0, BOARD_LEFT - 0,    MAX_Y - PADDLE_H - 2, SCOREBOARD_COLOR);
      tft.fillRect(BOARD_RIGHT,  0, TILE_W / 2,        MAX_Y - PADDLE_H - 2, SCOREBOARD_COLOR);
      tft.fillRect(BOARD_LEFT, BOARD_TOP, BOARD_RIGHT - BOARD_LEFT , MAX_Y - BOARD_TOP - PADDLE_H, BACKGROUND_COLOR);
//      tft.fillRect(0, MAX_Y - PADDLE_H, MAX_X, MAX_Y, BACKGROUND_COLOR);
      tft.fillRect(0, MAX_Y, MAX_X, 2, SCOREBOARD_COLOR);

      scoreinit();

      // initialize tile matrix
      newLevel();
      paddlesetXY((MAX_X -PADDLE_W)/ 2, MAX_Y - PADDLE_H);
      paddledraw();
      
//      create_butt();
      
      do
      {
        
        serve();
        play();

      } while (scorehasLivesLeft());
    
      const char *go ="GAME OVER";
      const int font_size = 5;
      int cursor = (MAX_X - font_size * FONT_SPACE * 9) / 2;

      boolean skip = false;

      for ( int i= 0; i < 9; i++)
      {
        if ( go[i] != ' ')
        {
//          tft.fillRect(cursor, 94, FONT_SPACE * font_size - 2, FONT_X * font_size, RED);
//          tft.drawChar(go[i], cursor - FONT_SPACE / 2, 94 + FONT_SPACE / 2, font_size, WHITE);
          tft.setCursor(cursor - FONT_SPACE / 2, 124 + FONT_SPACE / 2);
          tft.setTextColor(WHITE);  tft.setTextSize(font_size);
          tft.println(go[i]);    
        }
      
        cursor += font_size * FONT_SPACE;

        skip = skip || Joystick::fire();

        if ( skip ) continue;

        for (int j = (10-i) * 100; j > 200; j-=1500 / (i+1))
        {
          Beeping::beep(j, 100);
          delay(100);
        }
      }
    
      while (bb !=1 && !Joystick::fire()){pressbutt();}
    }

    void serve()
    {
      ballsetXY(paddlegetX(), MAX_Y - PADDLE_H - BALL_R * 2 - 2);
      balldraw();
      do
      {  
        pressbutt();
//        const int m = js.getX();

        int m = 0;
        if (bb == 2)
        {
          m=-3;
        }
        if (bb == 3)
        {
          m=3;
        }
        
        if ( m != 0)
        {
          paddledraw(m);
          ballclear();
          ballsetX(paddlegetX());
          balldraw();
          delay(5);
        }
//      } while (!Joystick::fire());
      } while (bb !=1 && !Joystick::fire());
    }

   
    void play()
    {
      for(;;)
      {
        pressbutt();
        
        unsigned long waited = millis() - last;

        if ( waited > BALL_MOVE_WAIT )
        {
          last = millis();
        
          ballclear();
          move();
          balldraw();
        
//          if (ball.collision(paddle))
          if (collision()) // отскок от лопаты работает
          {
            beep = 200;
            Beeping::beep(400,30);
          }
        
          // check collision w/ tiles
//          int scored = ball.collision(tiles);        
          int scored = ballcollision();

          if ( scored > 0)
          {
            beep *=1.1;
            if ( beep > 3000) beep = 3000;
            Beeping::beep(beep,25);
          }
        
          if (scored > 0)
          {
            scoreupdate(scored);
          
            if ( tilesallGone() )
            {
              ballclear();
              newLevel();
              scorenextLevel();
              break;
            }
          }

//          if (ballescaped(paddle))
          if (ballescaped()) // пропустил шар работает
          {
            scoredied();

            for (int i = 54; i > 30; i--)
            {
              Beeping::beep(random(i,i*i), 3);
              delay(6);
            }
          
            ballclear();
          
            break;
          }
        }

//        const int m = js.getX();

        int m = 0;
        if (bb == 2)
        {
          m=-3;
        }
        if (bb == 3)
        {
          m=3;
        }
        
        if (waited > PADDLE_MOVE_WAIT && m != 0)
//        if (m != 0)
        {
          last = millis();
          paddledraw(m);
        }
      }
    }

    void newLevel()
    {
      // ANALOG 7 SHOULD NOT BE CONNECTED TO ANYTHING.
      // PLEASE LEAVE PIN FLOATING
    
      randomSeed(analogRead(7));

      tilesdrawAll();

      ballsetXY(MAX_X / 2, MAX_Y - PADDLE_H - BALL_R * 2 - 2);
      ballsetXiYi(-BALL_SPEED_H, -BALL_SPEED_V);
    
      beep = 200;
    }
    
/////////scoreboard
 
void scoreinit()
    {
      // draw back wall and sidewalls

      tft.setCursor(0, 4);
      tft.setTextColor(BLACK);  tft.setTextSize(2);
      tft.println("     000000000    0");

      tft.setCursor(6, 4);
      tft.setTextColor(BLUE);  tft.setTextSize(2);
      tft.println("   S            L    B");

      tft.setCursor(0, 4);
      tft.setTextColor(RED);  tft.setTextSize(2);
//      tft.println("              0    1   4");
        tft.print("              ");tft.print(score);tft.print("    ");tft.print(level);tft.print("   ");tft.println(lives);
    }
  
    void scoreupdate( int scored ){
      score+= scored;
      int digits = nDigits(score);
//      tft.drawNumber(score, (15-digits)*12, 4, 2, RED, SCOREBOARD_COLOR);
      tft.setCursor((15-digits)*12, 4);
      tft.setTextColor(RED, SCOREBOARD_COLOR);  tft.setTextSize(2);
      tft.println(score);
    }
  
    void scorenextLevel()
    {
      level++;
      int digits = nDigits(level);
//      tft.drawNumber(level,(20-digits)*12, 4, 2, RED, SCOREBOARD_COLOR);
      tft.setCursor((20-digits)*12, 4);
      tft.setTextColor( RED, SCOREBOARD_COLOR);  tft.setTextSize(2);
      tft.println(level);
    }
  
    void scoredied()
    {
      lives--; // вечная жизнь
      if ( lives < 0) return;
      int digits = nDigits(lives);
//      tft.drawNumber(lives,(24-digits)*12, 4, 2, RED, SCOREBOARD_COLOR);
      tft.setCursor((24-digits)*12, 4);
      tft.setTextColor( RED, SCOREBOARD_COLOR);  tft.setTextSize(2);
      tft.println(lives);
    }

    boolean scorehasLivesLeft()
    {
      return lives > 0;
    }
  
    int nDigits(int value)
    {
      int digits = 1;
      long compare = 10;
      while (compare <= value)
      {
        compare *= 10;
        digits++;
      }
      return digits;
    }

/////////////ball

  const int SCORE[7] = {0, 1, 1, 3, 5, 5, 7};

  int left=BOARD_LEFT;
  int right=BOARD_RIGHT-3;
  int top=BOARD_TOP;
  int bottom=MAX_Y;
  
  float x, y, xi, yi;

void ballclear() //очистить шар работает
    {
      tft.fillRect(x, y, BALL_R * 2, BALL_R * 2, BLACK);
    }
  
    void balldraw() //нарисовать шар работает
    {
      tft.fillRect(x, y, BALL_R * 2, BALL_R * 2, BALL_COLOR);
    }

    void ballsetXY(int x_, int y_) //установить шар в центр лопаты работает
    {
      x = x_;
      y = y_;
    }

    void ballsetX(int x_) //установть шар на лопате по X
    {
      x = x_;
    }

    void ballsetXiYi(int xi_, int yi_) //-1.00 -1.00 показывает когда шар в центре лопаты
    {
      xi = xi_;
      yi = yi_;
    }

//    bool escaped(Paddle &paddle)
    bool ballescaped() // шар не отбит работает 
    {
      return y >= paddlegetY();
    }
  
    void move()  // генерирует движение мяча x,y координаты
    {
      if((xi < 0 && x + xi < left)||(xi > 0 && x +xi >= right))
      {
        xi *= -1;
      }

      if(yi < 0 && y+yi-BALL_R*2 < top)
      {
        yi *= -1;
      }

      x+= xi;
      y+= yi;
    }

    int ballgetX()
    {
      return x;
    }

    int ballgetY()
    {
      return y;
    }

    int ballgetXi()
    {
      return xi;
    }

    // check for collision with paddle
    // calculate score and bounce

//    int collision(Tiles &tiles)

    int ballcollision()
    {
//      const int x = this->x+xi;
//      const int y = this->y+yi;
      const int bx = x+xi;
      const int by = y+yi;
    
      int hit;
    
      if ( hit = hitTile(    bx,              by  ))
      {
        return SCORE[hit];
      }
        
      if ( hit = hitTile(    bx + BALL_R * 2, by + BALL_R * 2  ))
      {
        return SCORE[hit];
      }
    
      if ( hit = hitTile(    bx + BALL_R * 2, by  ))
      {
        return SCORE[hit];
      }
    
      hit = hitTile(         bx,              by + BALL_R * 2  );
    
      return SCORE[hit];
    }

    // check for collision with paddle
    // and bounce
//    bool collision(Paddle &paddle)
    bool collision()
    {
      // only interested if ball is moving down
      if ( yi < 0 ) return false;

      const int ny = y+yi+BALL_R*2;

      if ( ny < paddlegetY() ) return false;
    
      // ball center
      const int bc = x + BALL_R;

      const int diff = bc - paddlegetX();

      if ( diff < -PADDLE_W-PADDLE_TOLERANCE || diff > PADDLE_W + PADDLE_TOLERANCE)
      {
        return false;
      }
      else if (diff > 0)
      {
        const int hit = diff / ((PADDLE_W + PADDLE_TOLERANCE)/2);
        xi = (float)hit * .5f + .5f;
        yi = - 2 + xi;
      }
      else if (diff < 0)
      {
        const int hit = diff / ((PADDLE_W + PADDLE_TOLERANCE)/2);
        xi = (float)hit * .5f - .5f;
        yi = - 2 - xi;
      }
      else
      {
        yi *= -1;
      }
    
      return true;
    }

//    int hitTile(Tiles &tiles, int x, int y)
    int hitTile( int x, int y)
    {
      int ty =(y - TILES_TOP)/ TILE_H;
      int tx =(x - TILES_LEFT)/ TILE_W;
    
      const boolean hit = ty >= 0 && ty < ROWS && tx >= 0 && tx < COLS && tilesexists(tx,ty);

      if (!hit) return 0;
    
      clearTile( tx, ty );

      const int score = ROWS - ty;
    
      ty*= TILE_H;
      ty+= TILES_TOP;

      tft.fillRect(TILES_LEFT + tx * TILE_W, ty, TILE_W - 2, TILE_H - 2, BACKGROUND_COLOR);
        
      const int ty2= ty + TILE_H;

      if ( (yi > 0 && y > ty) || (yi < 0 && y > ty2))
      {
        xi *=-1;
      }
      else
      {
        yi *=-1;
      }

      return score;
    }

////// tiles

    boolean tilesallGone()
    {
      return tilesLeft == 0;
    }
  
    void tilesdrawAll()
    {
      for(int i = 0; i < ROWS; i++)
      {
        for(int j = 0; j < COLS; j++)
        {
          tiles[i][j] = TILE_IS_NOT_DRAWN;
        }
      }

      int tilesLeftToDraw = ROWS * COLS;
    
      while ( tilesLeftToDraw > 0)
      {
        int c = random(COLS);
        int r = random(ROWS);
        if (tiles[r][c] == TILE_IS_DRAWN)
        {
          continue;
        }

        tft.fillRect(TILES_LEFT + c * TILE_W, TILES_TOP + r * TILE_H, TILE_W - 2, TILE_H - 2, COLORS[r]);
        tiles[r][c]= TILE_IS_DRAWN;
        tilesLeftToDraw--;

        Beeping::beep( 400 + r* 35 + c * 2, 5 );
        delay(5);
      }
    
      tilesLeft = ROWS * COLS;
    }
    
    void clearTile( int x, int y )
    {
      tiles[y][x] = TILE_IS_NOT_DRAWN;
      tilesLeft--;
    }
    
    boolean tilesexists(int x, int y)
    {
      return tiles[y][x] == TILE_IS_DRAWN;
    }

///////paddle

  int px, py;
  
  // draws paddle on screen when it moves
  // to avoid spending too much time clearing the screen and drawing
  // only the same amount of pixels that the paddle moves are cleared and drawn
  
    void paddledraw(int m)
    {
      if ( m < 0 && px > PADDLE_W )
      {
        // cant move beyond the screen border
        if ( m < -(px - PADDLE_W)) m = -(px - PADDLE_W);

        // remove part of the old paddle on the left
        tft.fillRect( px + PADDLE_W + m, py, -m, PADDLE_H, BACKGROUND_COLOR);
      
        // add a bit more to the left of the existing paddle
        tft.fillRect( px - PADDLE_W + m, py, -m, PADDLE_H, PADDLE_COLOR);
      
        // remember m < 0
        px+=m;
      }
      else if ( m > 0 && x < MAX_X - PADDLE_W )
      {
        // cant move beyond the screen border
        if ( m > MAX_X - px - PADDLE_W ) m = MAX_X - px - PADDLE_W;
      
        // remove a part of the old paddle on the right
        tft.fillRect( px - PADDLE_W, py, m, PADDLE_H, BACKGROUND_COLOR);
      
        // add a bit more  of paddle to the left of the existing paddle
        tft.fillRect( px + PADDLE_W, py, m, PADDLE_H, PADDLE_COLOR );
      
        px+=m;
      }
    }
  
    // draws paddle the first time
    // all paddle must be drawn
  
    void paddledraw()
    {
      tft.fillRect(px- PADDLE_W, py, PADDLE_W * 2, PADDLE_H, PADDLE_COLOR);
    }
  
    void paddlesetXY(int x_, int y_)
    {
      px = x_;
      py = y_;
    }

    int paddlegetX()
    {
      return px;
    }

    int paddlegetY()
    {
      return py;
    }
void create_butt() {
char buttonlabels[4][2]={"P", "F", "<", ">"};
uint16_t buttoncolors[4] = { DARKGREY, PURPLE, DARKGREEN,ORANGE};
    for (uint8_t row=0; row<2; row++) {
      for (uint8_t col=0; col<2; col++) {
        buttons[col + row*2].initButton(&tft, 60+col*(80+120), 
                 370+row*(50+20),    // x, y, w, h, outline, fill, text
                  80, 50,  WHITE, buttoncolors[col+row*2],  WHITE,
                  buttonlabels[col + row*2], 2); 
        buttons[col + row*2].drawButton();
      }
    }
}


void pressbutt()
{
  bb=5;
  bool isPaused = true;
  while(isPaused)
  {
       TSPoint p = ts.getPoint();  
       pinMode(XM, OUTPUT);                                   // Возвращаем режим работы вывода X- в значение «выход» для работы с дисплеем
       pinMode(YP, OUTPUT);                                   // Возвращаем режим работы вывода Y+ в значение «выход» для работы с дисплеем
       if (p.z>100) {
       //      Выводим «сырые» показания TouchScreen:
       // Serial.println((String) "X="+p.x+",\tY="+p.y);
       p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
       p.y = map(p.y, TS_MINY, TS_MAXY, 0, 480);
       //Serial.println((String) "X="+p.x+",\tY="+p.y);
       } else {p.x = p.y = -1;}
       
       // go thru all the buttons, checking if they were pressed
       for (uint8_t b=0; b<4; b++) {
         if (buttons[b].contains(p.x, p.y)) {
           //Serial.print("Pressing: "); Serial.println(b);
           buttons[b].press(true);  // tell the button it is pressed
           bb=b;
         } else {
           buttons[b].press(false);  // tell the button it is NOT pressed
         }
       }
   if(bb !=0) isPaused = false;
  }
}
