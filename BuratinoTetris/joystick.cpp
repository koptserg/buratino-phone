
#ifndef JOYSTICKCPP
#define JOYSTICKCPP

#include <Arduino.h>

// analog pins

#define XPIN        0
#define YPIN        1

// digital pin

#define FIREPIN     19

// joystick center for both axis

#define CENTER      512

class Joystick
{
  public:

    // joystick position constants 
    
    static const int NEUTRAL = 0;
    static const int SOFT = 1;
    static const int HARD = 2;
    static const int HARDER = 3;

    static void init ()
    {
      pinMode ( FIREPIN, INPUT_PULLUP );
    }

    static int getX()
    {
      return getPosition(XPIN) * -1;
    }

    static int getY()
    {
      return getPosition(YPIN) * -1;
    }

    static boolean fire()
    {
      return digitalRead(FIREPIN) == LOW;
    }

    static void waitForRelease()
    {
      while (fire());
    }

    static void waitForRelease(int howLong)
    {
      int c = 0;
      do
      {
        delay (10);
        c += 10;
      }
      while ((fire() || getY() != 0 || getX() != 0) && c < howLong);
    }

    static void waitForClick()
    {
      while (!fire());
    }

  private:

    static int getPosition (int pin)
    {
//      const int n = analogRead(pin) - CENTER;
      const int n = 512 - CENTER;
      return n / 128;
    }
    
};

#endif
