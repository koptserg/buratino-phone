
#ifndef SEQUENCERCPP
#define SEQUENCERCPP

#include "Arduino.h"
#include "beeping.cpp"

static const int song[] = {

  NOTE_E5, 4,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 2,
  NOTE_E5, 1,
  NOTE_D5, 1,
  NOTE_C5, 2,
  NOTE_B4, 2,

  NOTE_A4, 4,
  NOTE_A4, 2,
  NOTE_C5, 2,

  NOTE_E5, 4,
  NOTE_D5, 2,
  NOTE_C5, 2,

  NOTE_B4, 2,
  NOTE_E4, 2,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 4,
  NOTE_E5, 4,

  NOTE_C5, 4,
  NOTE_A4, 4,

  NOTE_A4, 2,
  NOTE_A3, 2,
  NOTE_B3, 2,
  NOTE_C4, 2,

  NOTE_D4, 2,
  NOTE_D5, 4,
  NOTE_F5, 2,

  NOTE_A5, 2,
  NOTE_A5, 1,
  NOTE_A5, 1,
  NOTE_G5, 2,
  NOTE_F5, 2,

  NOTE_E5, 6,
  NOTE_C5, 2,

  NOTE_E5, 2,
  NOTE_F5, 1,
  NOTE_E5, 1,
  NOTE_D5, 2,
  NOTE_C5, 2,

  NOTE_B4, 4,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 4,
  NOTE_E5, 4,

  NOTE_C5, 4,
  NOTE_A4, 4,

  NOTE_A4, 8,

  // repeat

  NOTE_E5, 4,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 2,
  NOTE_E5, 1,
  NOTE_D5, 1,
  NOTE_C5, 2,
  NOTE_B4, 2,

  NOTE_A4, 4,
  NOTE_A4, 2,
  NOTE_C5, 2,

  NOTE_E5, 4,
  NOTE_D5, 2,
  NOTE_C5, 2,

  NOTE_B4, 2,
  NOTE_E4, 2,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 4,
  NOTE_E5, 4,

  NOTE_C5, 4,
  NOTE_A4, 4,

  NOTE_A4, 2,
  NOTE_A3, 2,
  NOTE_B3, 2,
  NOTE_C4, 2,

  NOTE_D4, 2,
  NOTE_D5, 4,
  NOTE_F5, 2,

  NOTE_A5, 2,
  NOTE_A5, 1,
  NOTE_A5, 1,
  NOTE_G5, 2,
  NOTE_F5, 2,

  NOTE_E5, 6,
  NOTE_C5, 2,

  NOTE_E5, 2,
  NOTE_F5, 1,
  NOTE_E5, 1,
  NOTE_D5, 2,
  NOTE_C5, 2,

  NOTE_B4, 4,
  NOTE_B4, 2,
  NOTE_C5, 2,

  NOTE_D5, 4,
  NOTE_E5, 4,

  NOTE_C5, 4,
  NOTE_A4, 4,

  NOTE_A4, 8,
  -1

};

static volatile unsigned long last;
static volatile int c;
static volatile int w;
static volatile boolean songOn;


class Sequencer {
  public:

    static void init() {
      
      // enable timer, used to play music sequencer async
      TCCR1A = 0; // No options in control register A
      TCCR1B = (1 << CS10); // Set prescaler to divide by 8
      TIMSK1 = (1 << OCIE1A); // Call ISR when TCNT2 = OCRA2
      OCR1A = 0;// Set frequency of generated wave
      sei(); // Enable interrupts to generate waveform!

      last = c = w = 0;
      songOn = false;
    }

    static void start() {
      init();
      songOn = true;
    }

    static void stop() {
      songOn = false;
      Beeping::mute();
    }

    static void play()
    {
      if (!songOn)
      {
        return;
      }

      unsigned long m = millis();
      if ( m - last > w)
      {
        last = m;

        int fr = song[c++];
        int d = song[c++];

        Beeping::beep (fr, d * 100 );
        w = d * 105;

        if ( song[c] == -1 ) {
          c = 0;
          w = 10000;
          Beeping::mute();
        }
      }
    }
};


#endif
