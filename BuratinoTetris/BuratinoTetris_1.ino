
//#include "TFTv2_extended.h"
//#include <SPI.h>
#include <TouchScreen.h>
#include "ILI9488.h"
#include <Adafruit_GFX.h>    // Core graphics library
Adafruit_GFX_Button buttons[4];

// Color definitions
#define  BLACK       0x0000  ///<   0,   0,   0
//#define  NAVY        0x000F  ///<   0,   0, 123
#define  DARKGREEN   0x03E0  ///<   0, 125,   0
//#define  DARKCYAN    0x03EF  ///<   0, 125, 123
//#define  MAROON      0x7800  ///< 123,   0,   0
#define  PURPLE      0x780F  ///< 123,   0, 123
//#define  OLIVE       0x7BE0  ///< 123, 125,   0
#define  LIGHTGREY   0xC618  ///< 198, 195, 198
#define  DARKGREY    0x7BEF  ///< 123, 125, 123
#define  BLUE        0x001F  ///<   0,   0, 255
#define  GREEN       0x07E0  ///<   0, 255,   0
#define  CYAN        0x07FF  ///<   0, 255, 255
#define  RED         0xF800  ///< 255,   0,   0
#define  MAGENTA     0xF81F  ///< 255,   0, 255
#define  YELLOW      0xFFE0  ///< 255, 255,   0
#define  WHITE       0xFFFF  ///< 255, 255, 255
#define  ORANGE      0xFD20  ///< 255, 165,   0
//#define  GREENYELLOW 0xAFE5  ///< 173, 255,  41
//#define  PINK        0xFC18  ///< 255, 130, 198

#include "joystick.cpp"
#include "beeping.cpp"
//#include "tetris.cpp"
#include "sequencer.cpp"

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

TouchScreen ts     = TouchScreen(XP, YP, XM, YM);      // Создаём объект для работы с TouchScreen

////////////////
#define LCD_WIDTH             480
#define LCD_HEIGHT            320

#define MIN(X, Y)             (((X) < (Y)) ? (X) : (Y))

#define BOARD_WIDTH           11
#define BOARD_HEIGHT          20

#define BLOCK_SIZE            MIN( (LCD_WIDTH-1) / BOARD_WIDTH, (LCD_HEIGHT-1) / BOARD_HEIGHT )

#define BOARD_LEFT            (LCD_WIDTH - BOARD_WIDTH * BLOCK_SIZE)/4*2
#define BOARD_RIGHT           (BOARD_LEFT + BLOCK_SIZE * BOARD_WIDTH)
#define BOARD_TOP             (LCD_HEIGHT - BOARD_HEIGHT * BLOCK_SIZE) / 2
#define BOARD_BOTTOM          (BOARD_TOP + BOARD_HEIGHT * BLOCK_SIZE)

#define PIT_COLOR             CYAN
#define BG_COLOR              BLACK

#define DROP_WAIT_INIT        800
#define INPUT_WAIT_FIRE       10
#define INPUT_WAIT_ROT        200
#define INPUT_WAIT_MOVE       100

#define INPUT_WAIT_NEW_SHAPE  600

/////////////////

uint8_t disp = 1; // display on
uint8_t bb=5;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("BuratinoTetris!"); 

  Sequencer::init();

  tft.begin();
  tft.setRotation(3);
  DISPpower(disp);

  // play turnon sound	
  Beeping::turnOn();

  // initialize joystick
  Joystick::init();
}

void loop() {

  // title screen
  drawPreGameScreen();

  // wait a bit before playing sounds
  delay(700);

  // start playing tetris theme
  Sequencer::start();

  // wait for click
  while (bb !=1 && !Joystick::fire()){pressbutt();}
  bb=5;

  // stop playing tetris theme
  Sequencer::stop();

  // load game
//  Tetris t;
  trun();

  // game ended
  gameOver();
}

void drawPreGameScreen()
{
// draw background
    int c = LCD_HEIGHT / 28;
    for (int i = LCD_HEIGHT - 1; i >= 0; i -= 2)
    {
      tft.fillRect(0, i, LCD_WIDTH, 2, 0x1f - i / c);
    }
//  tft.fillScreen(WHITE);
  tft.setCursor(90, 40);
  tft.setTextColor(ORANGE);  tft.setTextSize(8);
  tft.println("Tetris");
//  tft.drawCenteredString("Tetris", 40, 8, BLUE);
  tft.setCursor(150, 110);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println("Click F to play");
//  tft.drawCenteredString("Click to play", 110, 2, BLACK);
//  tft.setCursor(20, 220);
//  tft.setTextColor(PURPLE);  tft.setTextSize(2);
//  tft.println("http://vilaca.eu");
//  tft.drawCenteredString("http://vilaca.eu", 220, 2, PURPLE);
  create_butt();
}


void gameOver()
{
  tft.fillRect(102, 84, 256, 52, BLACK);
  tft.setCursor(118, 94);
  tft.setTextColor(WHITE, BLACK);  tft.setTextSize(4);
  tft.println("Game Over");
//  tft.drawString("Game Over", 48, 94, 4, 0x3ffff);
  tft.drawRect(102, 84, 256, 52, RED);

  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(600, 200);
  delay(300);
  Beeping::beep(200, 600);
  delay(1500);
  while(!Joystick::fire() && bb !=1) {pressbutt();}
  bb=5;
//  Joystick::waitForClick();
}


ISR(TIMER1_COMPA_vect) {

  // sequencer plays tetris theme
  Sequencer::play();
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

///////////////////////////////////

// used to clear the position from the screen
typedef struct Backup {
  byte x, y, rot;
};
  
  byte l_shape[4][4][2] {
    {{0, 0}, {0, 1}, {0, 2}, {1, 2}},
    {{0, 1}, {1, 1}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
    {{0, 0}, {0, 1}, {1, 0}, {2, 0}},
  };

  byte j_shape[4][4][2] {
    {{1, 0}, {1, 1}, {0, 2}, {1, 2}},
    {{0, 0}, {1, 0}, {2, 0}, {2, 1}},
    {{0, 0}, {1, 0}, {0, 1}, {0, 2}},
    {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
  };

  byte o_shape[1][4][2] {
    { {0, 0}, {0, 1}, {1, 0}, {1, 1}}
  };

  byte s_shape[2][4][2] {
    {{0, 1}, {1, 0}, {1, 1}, {2, 0}},
    {{0, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte z_shape[2][4][2] {
    {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {0, 2}}
  };

  byte t_shape[4][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {1, 1}},
    {{0, 0}, {0, 1}, {1, 1}, {0, 2}},
    {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
    {{1, 0}, {0, 1}, {1, 1}, {1, 2}}
  };

  byte i_shape[2][4][2] {
    {{0, 0}, {1, 0}, {2, 0}, {3, 0}},
    {{0, 0}, {0, 1}, {0, 2}, {0, 3}}
  };

  // All game shapes and their colors

  byte *all_shapes[7] = {l_shape[0][0], j_shape[0][0], o_shape[0][0], s_shape[0][0], z_shape[0][0], t_shape[0][0], i_shape[0][0]};

  unsigned int colors[7] = {ORANGE, BLUE, YELLOW, GREEN, RED, MAGENTA, CYAN};


  // how many rotated variations each shape has
  
  byte shapes[7] = {4, 4, 1, 2, 2, 4, 2};


  // game progress
  
  int lines, level;

  // current shapes
  
  byte current;
  
  // tetris guidelines have all 7 shapes
  // selected in sequence to avoid
  // long runs without a shape
  
  byte next[7];
  byte next_c;

  unsigned long lastInput, lastDrop;

  byte board[BOARD_HEIGHT][BOARD_WIDTH];

  byte x, y, rot;
  Backup old;

  boolean newShape;

  int dropWait;

  void trun()
  {
    // analog 2 MUST NOT be connected to anything...
    
    randomSeed(analogRead(2));

    // clear board
    
    for ( int i = 0; i < BOARD_WIDTH; i++ )
    {
      for ( int j = 0; j < BOARD_HEIGHT; j++)
      {
        board[j][i] = 0;
      }
    }

    //next shape
    
    randomizer();

    // initialize game logic
    
    lastInput = 0;
    lastDrop = 0;
    dropWait = DROP_WAIT_INIT;
    level = 1;

    // draw background
    
    int c = LCD_HEIGHT / 28;
    for (int i = LCD_HEIGHT - 1; i >= 0; i -= 2)
    {
      tft.fillRect(0, i, LCD_WIDTH, 2, 0x1f - i / c);
    }

    tft.fillRect(BOARD_LEFT, 0, BOARD_RIGHT-BOARD_LEFT-1, BOARD_BOTTOM, BG_COLOR);
    create_butt();
    // draw board left limit
    
    tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_TOP,
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board right limit
    
    tft.drawLine (
      BOARD_RIGHT,
      BOARD_TOP,
      BOARD_RIGHT,
      BOARD_BOTTOM,
      PIT_COLOR);

    // draw board bottom limit
    
    tft.drawLine (
      BOARD_LEFT - 1,
      BOARD_BOTTOM,
      BOARD_RIGHT + 1,
      BOARD_BOTTOM,
      PIT_COLOR);

    for ( int i = BOARD_LEFT + BLOCK_SIZE - 1; i < BOARD_RIGHT; i += BLOCK_SIZE)
    {
      tft.drawLine (
        i,
        BOARD_TOP,
        i,
        BOARD_BOTTOM - 1,
        DARKGREY);
    }

    for ( int i = BOARD_TOP + BLOCK_SIZE - 1; i < BOARD_BOTTOM; i += BLOCK_SIZE)
    {
      tft.drawLine (
        BOARD_LEFT,
        i,
        BOARD_RIGHT - 1,
        i,
        DARKGREY);
    }

    scoreBoard();

    do {
       pressbutt();
      // get clock
      const unsigned long now = millis();

      // display new shape
      if ( newShape )
      {
        Joystick::waitForRelease(INPUT_WAIT_NEW_SHAPE);
        newShape = false;

        // a new shape enters the game
        chooseNewShape();

        // draw next box
       tft.fillRect(350, 3, BLOCK_SIZE * 6, BLOCK_SIZE * 5, BLACK);
       tft.drawRect(349, 2, BLOCK_SIZE * 6 + 1, BLOCK_SIZE * 5 + 1, WHITE);

        byte *shape = all_shapes[next[next_c]];
        for ( int i = 0; i < 4; i++ )
        {
          byte *block = shape + i * 2;
          tft.fillRect(
            350 + BLOCK_SIZE + block[0]*BLOCK_SIZE,
            3 + BLOCK_SIZE + block[1]*BLOCK_SIZE,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            colors[next[next_c]]);
        }

        // check if new shape is placed over other shape(s)
        // on the board
        if ( touches(0, 0, 0 ))
        {
          // draw shape to screen
          draw();
          return;
        }

        // draw shape to screen
        draw();
      }
      else
      {
        // check if enough time has passed since last time the shape
        // was moved down the board
//        if ( now - lastDrop > dropWait || Joystick::getY() > Joystick::NEUTRAL)
          if ( now - lastDrop > dropWait)
        {
          // update clock
          lastDrop = now;
          moveDown();
        }
      }

      if (!newShape)
      {
        userInput(now);
//        bb=5;
      }
//        delay(10);

      delay(10);
    } while (true);

  }

  void chooseNewShape()
  {
    current = next[next_c];

    next_c++;
    
    if ( next_c == 7 )
    {
      randomizer();
    }

    // new shape must be positioned at the middle of
    // the top of the board
    // with zero rotation
    
    rot = 0;
    y = 0;
    x = BOARD_WIDTH / 2;

    old.rot = rot;
    old.y = y;
    old.x = x;
  }

  void userInput(unsigned long now)
  {
    unsigned long waited = now - lastInput;

    int jx = Joystick::getX();    

//    int move = INPUT_WAIT_MOVE / jx;
    int move = INPUT_WAIT_MOVE;
//    if ( jx < Joystick::NEUTRAL && waited > -move)
    if (bb==2 && waited > move)
    {
      if  (x > 0 && !touches(-1, 0, 0))
      {
        x--;
        lastInput = now;
        draw();
      }
    }
//    else if ( jx > Joystick::NEUTRAL && waited > move )
    else if (bb==3 && waited > move)
    {
      if ( x < BOARD_WIDTH && !touches(1, 0, 0))
      {
        x++;
        lastInput = now;
        draw();
      }
    }
    
    if ( bb==1 or Joystick::fire())
//    if ( bb==1 && waited > INPUT_WAIT_FIRE)
    {
      while ( !touches(0, 1, 0 ))
      {
        delay(INPUT_WAIT_FIRE);
        y++;
      }
      lastInput = now;
      draw();
    }

    int my = Joystick::getY();
//    if (( my == -Joystick::HARD && waited > INPUT_WAIT_ROT ) || ( my == -Joystick::HARDER && waited > INPUT_WAIT_ROT / 2 ))
//    {
//      if (Joystick::getY() < 0 && !touches(0, 0, 1))

      if (bb==0 && waited > INPUT_WAIT_ROT)
      {
        rot++;
        rot %= shapes[current];
        lastInput = now;
        draw();
      }
//    }
  }

  void moveDown()
  {
    // prepare to move down
    // check if board is clear bellow
    
    if ( touches(0, 1, 0 ))
    {
      // moving down touches another shape
      newShape = true;

      // this shape wont move again
      // add it to the board
      
      byte *shape = all_shapes[current];
      for ( int i = 0; i < 4; i++ )
      {
        byte *block = (shape + (rot * 4 + i) * 2);
        board[block[1] + y][block[0] + x] = current + 1;
      }

      // check if lines were made
      
      score();
      Beeping::beep(1500, 25);
    }
    else
    {
      // move shape down
      y += 1;
      draw();
    }
  }

  void draw()
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);

//      tft.fillRectangleUseBevel(
      tft.fillRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        colors[current]);

      board[block[1] + y][block[0] + x] = 255;
    }

    // erase old
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (old.rot * 4 + i) * 2);

      if ( board[block[1] + old.y][block[0] + old.x] == 255 )
      continue;

      tft.fillRect(
        BOARD_LEFT + block[0]*BLOCK_SIZE + BLOCK_SIZE * old.x,
        BOARD_TOP + block[1]*BLOCK_SIZE + BLOCK_SIZE * old.y,
        BLOCK_SIZE - 2,
        BLOCK_SIZE - 2 ,
        BG_COLOR);
    }

    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (rot * 4 + i) * 2);
      board[block[1] + y][block[0] + x] = 0;
    }

    old.x = x;
    old.y = y;
    old.rot = rot;
  }

  boolean touches(int xi, int yi, int roti)
  {
    byte *shape = all_shapes[current];
    for ( int i = 0; i < 4; i++ )
    {
      byte *block = (shape + (((rot + roti) % shapes[current]) * 4 + i) * 2);

      int x2 = x + block[0] + xi;
      int y2 = y + block[1] + yi;

      if ( y2 == BOARD_HEIGHT || x2 == BOARD_WIDTH || board[y2][x2] != 0 )
      {
        return true;
      }
    }
    return false;
  }

  void score()
  {
    // we scan a max of 4 lines
    int ll;
    if ( y + 3 >= BOARD_HEIGHT )
    {
      ll = BOARD_HEIGHT - 1;
    }

    // scan board from current position
    for (int l = y; l <= ll; l++)
    {
      // check if there's a complete line on the board
      boolean line = true;

      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        if (board[l][c] == 0)
        {
          line = false;
          break;
        }
      }

      if ( !line )
      {
        // move to next line
        continue;
      }

      Beeping::beep(3000, 50);

      lines++;

      if ( lines % 10 == 0 )
      {
        level++;
        dropWait -= 200;
      }

      scoreBoard();

      // move board down
      for ( int row = l; row > 0; row -- )
      {
        for ( int c = 0; c < BOARD_WIDTH; c++ )
        {
          byte v = board[row - 1][c];

          board[row][c] = v;
//          tft.fillRectangleUseBevel(
          tft.fillRect(
            BOARD_LEFT + BLOCK_SIZE * c,
            BOARD_TOP + BLOCK_SIZE * row,
            BLOCK_SIZE - 2,
            BLOCK_SIZE - 2 ,
            v == 0 ? BLACK : colors[v - 1] ) ;
        }
      }

      // clear top line
      for ( int c = 0; c < BOARD_WIDTH; c++ )
      {
        board[0][c] = 0;
      }

      tft.fillRect(
        BOARD_LEFT,
        0,
        BOARD_RIGHT - BOARD_LEFT,
        BLOCK_SIZE,
        BLACK ) ;
    }

    delay(350);
  }

  void scoreBoard()
  {
    tft.fillRect(11, 8, 128, 50, BLACK);
    tft.setCursor(13, 13);
    tft.setTextColor(YELLOW);  tft.setTextSize(2);
    tft.println("Level");
    tft.setCursor(13, 37);
    tft.setTextColor(0x3f);  tft.setTextSize(2);
    tft.println("Lines");
    tft.setCursor(79, 13);
    tft.setTextColor(YELLOW);  tft.setTextSize(2);
    tft.println(level);
    tft.setCursor(79, 37);
    tft.setTextColor(0x3f);  tft.setTextSize(2);
    tft.println(lines);
    tft.drawRect(10, 7, 130, 52, 0xffff);
  }

  // create a sequence of 7 random shapes
  
  void randomizer()
  {
    // randomize 7 shapes
    
    for ( byte i = 0; i < 7; i ++)
    {
      boolean retry;
      byte shape;
      do
      {
        shape = random(7);
        
        // check if already in sequence
        
        retry= false;
        for ( int j = 0; j < i; j++)
        {
          if ( shape == next[j] )
          {
            retry = true;
            break;
          }
        }

      } while (retry);
      next[i] = shape;
    }
    next_c = 0;
  }  

void create_butt() {
  char buttonlabels[4][2]={"S", "F", "<", ">"};
  uint16_t buttoncolors[4] = { DARKGREY, RED, DARKGREEN,    
                                ORANGE};
//    tft.fillRect(0, 150, 320, 480, BLACK);
    for (uint8_t row=0; row<2; row++) {
      for (uint8_t col=0; col<2; col++) {
        buttons[col + row*2].initButton(&tft, 70+col*(80+260), 
                 175+row*(50+40),    // x, y, w, h, outline, fill, text
                  80, 50,  WHITE, buttoncolors[col+row*2],  WHITE,
                  buttonlabels[col + row*2], 2); 
        buttons[col + row*2].drawButton();
      }
    }
}

void pressbutt()
{
  bb=5;
//  delay(20);
       TSPoint p = ts.getPoint();  
       pinMode(XM, OUTPUT);                                   // Возвращаем режим работы вывода X- в значение «выход» для работы с дисплеем
       pinMode(YP, OUTPUT);                                   // Возвращаем режим работы вывода Y+ в значение «выход» для работы с дисплеем
       if (p.z>100) {
       //      Выводим «сырые» показания TouchScreen:
       // Serial.println((String) "X="+p.x+",\tY="+p.y);
       uint16_t x = p.x;
       uint16_t y = p.y;
       p.y = map(x, TS_MINY, TS_MAXY, 320, 0);
       p.x = map(y, TS_MINX, TS_MAXX, 0, 480);
       //Serial.println((String) "X="+p.x+",\tY="+p.y);
       } else {p.x = p.y = -1;}
       
       // go thru all the buttons, checking if they were pressed
       for (uint8_t b=0; b<4; b++) {
         if (buttons[b].contains(p.x, p.y)) {
           //Serial.print("Pressing: "); Serial.println(b);
           buttons[b].press(true);  // tell the button it is pressed
           bb=b;
//           Beeping::beep(600, 40);
         } else {
           buttons[b].press(false);  // tell the button it is NOT pressed
         }
       }

//       for (uint8_t b=0; b<4; b++) {
//         if (buttons[b].justReleased()) {
           //Serial.print("Released: "); Serial.println(b);
//           buttons[b].drawButton();  // draw normal
//         }
         //           Serial.println(bb);
//         if (buttons[b].justPressed()) {
//           buttons[b].drawButton(true);  // draw invert!
//         }
//       }  
}
