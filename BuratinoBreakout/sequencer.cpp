

#ifndef SEQUENCERCPP
#define SEQUENCERCPP

#include "Arduino.h"
#include "beeping.cpp"

static const int song1[] = {

  NOTE_C4, 4,
  NOTE_A4, 4,
  NOTE_A4, 4, 
  NOTE_G4, 4,
  NOTE_A4, 4, 
  NOTE_F4, 4, 
  NOTE_C4, 4, 
  NOTE_C4, 4,
  NOTE_C4, 4, 
  NOTE_A4, 4, 
  NOTE_A4, 4, 
  NOTE_AS4, 4,
  NOTE_G4, 4, 
  NOTE_C5, 2, 
  NOTE_C5, 8,
  NOTE_D4, 4,
  NOTE_D4, 4, 
  NOTE_AS4, 4,
  NOTE_AS4, 4,
  NOTE_A4, 4,
  NOTE_G4, 4, 
  NOTE_F4, 4, 
  NOTE_C4, 4, 
  NOTE_A4, 4,
  NOTE_A4, 4, 
  NOTE_G4, 4, 
  NOTE_A4, 4, 
  NOTE_F4, 2
};

static const int song[] = {

//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5, 
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 174, 374.999625);
//    delay(416.66625);
  NOTE_F3, 3.7,
//    tone(tonePin, 195, 124.999875);
//    delay(138.88875);
  NOTE_G3, 1.2,
//    tone(tonePin, 220, 499.9995);
//    delay(555.555);
  NOTE_A3, 5,
//    tone(tonePin, 146, 249.99975);
//    delay(277.7775);
  NOTE_D3, 2.5,
//    tone(tonePin, 146, 249.99975);
//    delay(277.7775);
  NOTE_D3, 2.5,
//    tone(tonePin, 195, 499.9995);
//    delay(555.555);
  NOTE_G3, 5,
//    tone(tonePin, 174, 374.999625);
//    delay(416.66625);
  NOTE_F3, 3.7,
//    tone(tonePin, 155, 124.999875);
//    delay(138.88875);
  NOTE_DS3, 1.2,
//    tone(tonePin, 174, 499.9995);
//    delay(555.555);
  NOTE_F3, 5,
//    tone(tonePin, 116, 249.99975);
//    delay(277.7775);
  NOTE_AS2, 2.5,
//    tone(tonePin, 116, 249.99975);
//    delay(277.7775);
  NOTE_AS2, 2.5,
//    tone(tonePin, 130, 499.9995);
//    delay(555.555);
  NOTE_C3, 5,
//    tone(tonePin, 130, 374.999625);
//    delay(416.66625);
  NOTE_C3, 3.7,
//    tone(tonePin, 146, 124.999875);
//    delay(138.88875);
  NOTE_D3, 1.2,
//    tone(tonePin, 155, 499.9995);
//    delay(555.555);
  NOTE_DS3, 5,
//    tone(tonePin, 155, 374.999625);
//    delay(416.66625);
  NOTE_DS3, 3.7,
//    tone(tonePin, 174, 124.999875);
//    delay(138.88875);
  NOTE_F3, 1.2,
//    tone(tonePin, 195, 499.9995);
//    delay(555.555);
  NOTE_G3, 5,
//    tone(tonePin, 220, 374.999625);
//    delay(416.66625);
  NOTE_A3, 3.7,
//    tone(tonePin, 233, 124.999875);
//    delay(138.88875);
  NOTE_AS3, 1.2,
//    tone(tonePin, 261, 749.99925);
//    delay(833.3325);
  NOTE_C4, 7.5,
//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5,
//    tone(tonePin, 293, 499.9995);
//    delay(555.555);
  NOTE_D4, 5,
//    tone(tonePin, 261, 374.999625);
//    delay(416.66625);
  NOTE_C4, 3.7,
//    tone(tonePin, 233, 124.999875);
//    delay(138.88875);
  NOTE_AS3, 1.2,
//    tone(tonePin, 261, 499.9995);
//    delay(555.555);
  NOTE_C4, 5,
//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5,
//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 220, 374.999625);
//    delay(416.66625);
  NOTE_A3, 3.7,
//    tone(tonePin, 195, 124.999875);
//    delay(138.88875);
  NOTE_G3, 1.2,
//    tone(tonePin, 220, 499.9995);
//    delay(555.555);
  NOTE_A3, 5,
//    tone(tonePin, 146, 374.999625);
//    delay(416.66625);
  NOTE_D3, 3.7,
//    tone(tonePin, 146, 124.999875);
//    delay(138.88875);
  NOTE_D3, 1.2,
//    tone(tonePin, 195, 499.9995);
//    delay(555.555);
  NOTE_G3, 5,
//    tone(tonePin, 174, 374.999625);
//    delay(416.66625);
  NOTE_F3, 3.7,
//    tone(tonePin, 155, 124.999875);
//    delay(138.88875);
  NOTE_DS3, 1.2,
//    tone(tonePin, 174, 499.9995);
//    delay(555.555);
  NOTE_F3, 5,
//    tone(tonePin, 116, 374.999625);
//    delay(416.66625);
  NOTE_AS2, 3.7,
//    tone(tonePin, 116, 124.999875);
//    delay(138.88875);
  NOTE_AS2, 1.2,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 220, 374.999625);
//    delay(416.66625);
  NOTE_A3, 3.7,
//    tone(tonePin, 195, 124.999875);
//    delay(138.88875);
  NOTE_G3, 1.2,
//    tone(tonePin, 174, 999.999);
//    delay(1111.11);
  NOTE_F3, 10,
//    tone(tonePin, 293, 999.999);
//    delay(1111.11);
  NOTE_D4, 10,
//    tone(tonePin, 261, 249.99975);
//    delay(277.7775);
  NOTE_C4, 2.5,
//    tone(tonePin, 233, 249.99975);
//    delay(277.7775);
  NOTE_AS3, 2.5,
//    tone(tonePin, 220, 249.99975);
//    delay(277.7775);
  NOTE_A3, 2.5,
//    tone(tonePin, 233, 249.99975);
//    delay(277.7775);
  NOTE_AS3, 2.5,
//    tone(tonePin, 261, 749.99925);
//    delay(833.3325);
  NOTE_C4, 7.5,
//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5,
//    tone(tonePin, 174, 999.999);
//    delay(1111.11);
  NOTE_F3, 10,
//    tone(tonePin, 233, 999.999);
//    delay(1111.11);
  NOTE_AS3, 10,
//    tone(tonePin, 220, 249.99975);
//    delay(277.7775);
  NOTE_A3, 2.5,
//    tone(tonePin, 195, 249.99975);
//    delay(277.7775);
  NOTE_G3, 2.5,
//    tone(tonePin, 174, 249.99975);
//    delay(277.7775);
  NOTE_F3, 2.5,
//    tone(tonePin, 195, 249.99975);
//    delay(277.7775);
  NOTE_G3, 2.5,
//    tone(tonePin, 220, 749.99925);
//    delay(833.3325);
  NOTE_A3, 7.5,
//    tone(tonePin, 146, 249.99975);
//    delay(277.7775);
  NOTE_D3, 2.5,
//    tone(tonePin, 146, 999.999);
//    delay(1111.11);
  NOTE_D3, 10,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 195, 374.999625);
//    delay(416.66625);
  NOTE_G3, 3.7,
//    tone(tonePin, 220, 124.999875);
//    delay(138.88875);
  NOTE_A3, 1.2,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 195, 374.999625);
//    delay(416.66625);
  NOTE_G3, 3.7,
//    tone(tonePin, 220, 124.999875);
//    delay(138.88875);
  NOTE_A3, 1.2,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 195, 374.999625);
//    delay(416.66625);
  NOTE_G3, 3.7,
//    tone(tonePin, 233, 124.999875);
//    delay(138.88875);
  NOTE_AS3, 1.2,
//    tone(tonePin, 311, 999.999);
//    delay(1111.11);
  NOTE_DS4, 10,
//    tone(tonePin, 311, 999.999);
//    delay(1111.11);
  NOTE_DS4, 10,
//    tone(tonePin, 293, 249.99975);
//    delay(277.7775);
  NOTE_D4, 2.5,
//    tone(tonePin, 261, 249.99975);
//    delay(277.7775);
  NOTE_C4, 2.5,
//    tone(tonePin, 233, 249.99975);
//    delay(277.7775);
  NOTE_AS3, 2.5,
//    tone(tonePin, 261, 249.99975);
//    delay(277.7775);
  NOTE_C4, 2.5,
//    tone(tonePin, 293, 749.99925);
//    delay(833.3325);
  NOTE_D4, 7.5,
//    tone(tonePin, 233, 249.99975);
//    delay(277.7775);
  NOTE_AS3, 2.5,
//    tone(tonePin, 233, 999.999);
//    delay(1111.11);
  NOTE_AS3, 10,
//    tone(tonePin, 261, 999.999);
//    delay(1111.11);
  NOTE_C4, 10,
//    tone(tonePin, 233, 249.99975);
//    delay(277.7775);
  NOTE_AS3, 2.5,
//    tone(tonePin, 220, 249.99975);
//    delay(277.7775);
  NOTE_A3, 2.5,
//    tone(tonePin, 195, 249.99975);
//    delay(277.7775);
  NOTE_G3, 2.5,
//    tone(tonePin, 220, 249.99975);
//    delay(277.7775);
  NOTE_A3, 2.5,
//    tone(tonePin, 233, 749.99925);
//    delay(833.3325);
  NOTE_AS3, 7.5,
//    tone(tonePin, 195, 249.99975);
//    delay(277.7775);
  NOTE_G3, 2.5,
//    tone(tonePin, 195, 999.999);
//    delay(1111.11);
  NOTE_G3, 10,
//    tone(tonePin, 233, 499.9995);
//    delay(555.555);
  NOTE_AS3, 5,
//    tone(tonePin, 220, 374.999625);
//    delay(416.66625);
  NOTE_A3, 3.7,
//    tone(tonePin, 195, 124.999875);
//    delay(138.88875);
  NOTE_G3, 1.2,
//    tone(tonePin, 174, 499.9995);
//    delay(555.555);
  NOTE_F3, 5,
//    tone(tonePin, 116, 374.999625);
//    delay(416.66625);
  NOTE_AS2, 3.7,
//    tone(tonePin, 116, 124.999875);
//    delay(138.88875);
  NOTE_AS2, 1.2,
//    tone(tonePin, 174, 999.999);
//    delay(1111.11);
  NOTE_F3, 10,
//    tone(tonePin, 195, 499.9995);
//    delay(555.555);
  NOTE_G3, 5,
//    tone(tonePin, 220, 499.9995);
//    delay(555.555);
  NOTE_A3, 5,
//    tone(tonePin, 233, 1999.998);
//    delay(2222.22);
  NOTE_AS3, 20
};

static volatile unsigned long slast;
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

      slast = c = w = 0;
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
      if ( m - slast > w)
      {
        slast = m;

        int fr = song[c++];
        int d = song[c++];

        Beeping::beep (fr, d * 120 );
        w = d * 133;

        if ( song[c] == -1 ) {
          c = 0;
          w = 10000;
          Beeping::mute();
        }
      }
    }
};


#endif
