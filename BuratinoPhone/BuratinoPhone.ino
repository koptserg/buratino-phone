#include "Adafruit_GFX.h"
#include "ILI9488.h"
#include <TouchScreen.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define ADAFRUIT_FONA_DEBUG 0

SoftwareSerial fonaSS = SoftwareSerial(13, 12);
Adafruit_FONA fona = Adafruit_FONA(4);

// For the Adafruit TFT shield, these are the default.
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_RS A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RST A4 // Can alternately just connect to Arduino's reset pin

// Color definitions
#define  BLACK       0x0000  ///<   0,   0,   0
//#define  NAVY        0x000F  ///<   0,   0, 123
#define  DARKGREEN   0x03E0  ///<   0, 125,   0
//#define  DARKCYAN    0x03EF  ///<   0, 125, 123
//#define  MAROON      0x7800  ///< 123,   0,   0
//#define  PURPLE      0x780F  ///< 123,   0, 123
//#define  OLIVE       0x7BE0  ///< 123, 125,   0
//#define  LIGHTGREY   0xC618  ///< 198, 195, 198
#define  DARKGREY    0x7BEF  ///< 123, 125, 123
#define  BLUE        0x001F  ///<   0,   0, 255
//#define  GREEN       0x07E0  ///<   0, 255,   0
//#define  CYAN        0x07FF  ///<   0, 255, 255
#define  RED         0xF800  ///< 255,   0,   0
//#define  MAGENTA     0xF81F  ///< 255,   0, 255
#define  YELLOW      0xFFE0  ///< 255, 255,   0
#define  WHITE       0xFFFF  ///< 255, 255, 255
#define  ORANGE      0xFD20  ///< 255, 165,   0
//#define  GREENYELLOW 0xAFE5  ///< 173, 255,  41
//#define  PINK        0xFC18  ///< 255, 130, 198

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9488 tft(LCD_CS, LCD_RS, LCD_WR, LCD_RD, LCD_RST);

// The STMPE610 uses hardware SPI on the shield, and #8
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

// text box where numbers go
#define TEXT_X 20
#define TEXT_Y 30
#define TEXT_W 300
#define TEXT_H 50
#define TEXT_TSIZE 4
#define TEXT_TCOLOR  WHITE
// the data (phone #) we store in the textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;

// We have a status line for like, is FONA working
//#define STATUS_X 10
//#define STATUS_Y 85
//line date and time
//#define TIME_X 80
//#define TIME_Y 10

/* create 15 buttons, in classic candybar phone style */
char buttonlabels[15][6]={"S", "M", "C", "1", "2abc", "3def", "4ghi", "5jkl", "6mno", "7pqrs", "8tuv", "9wxyz", "*", "0+ ", "#" };

Adafruit_GFX_Button buttons[15];

uint8_t CallStatus = 0;
uint8_t oldCallStatus = 5;
uint16_t callstatus_i = 1000;
uint8_t bmax = 15;
uint8_t bbook = 0;
boolean incallnum = false; // входящий номер
uint8_t oldbutt = 15;
uint8_t ibut=0;
uint8_t sbook=0; 
//char replybuf[1] = "";
char replybuf[60]= "  ... BuratinoPhone ...";
uint8_t oldnsms = 20;
uint8_t disp = 1; // display on
uint8_t olddisp = 0;
unsigned long new_millis = millis();
uint8_t bdis_max = 0;
uint8_t dn = 1;
char bt[2]= ".";

void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("Real_o_Phone!"); 
  
  // clear the screen
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen( BLACK);
  status(replybuf); // приветствие
  // Check FONA is there
//  FONApowerOn();
  DISPpower(disp);
  fonaSS.begin(19200); // if you're using software serial
  // проверяем есть ли связь с GPRS устройством
  // See if the FONA is responding
  if (! fona.begin(fonaSS)) {           // can also try fona.begin(Serial1)  
    status(F("Does not work"));
    while (1);
  }
  status(F("OK!"));
  
    // create 'text field'
  tft.drawRect(TEXT_X-10, TEXT_Y, TEXT_W, TEXT_H,  WHITE);
  
  // Check we connect to the network
  while (fona.getNetworkStatus() != 1) {
    status(F("Looking for service..."));
    delay(100);
  }
  status(F("Connected to network!"));
 
  // set to external mic & headphone
  fona.setAudio(FONA_EXTAUDIO);
  fona.setVolume(100); // 0-100, defaul 40
  fona.callerIdNotification(1); // включить АОН
//    fonaSS.println(F("AT+CLIP=1"));
// задать AT командами из GPRSAT Commands сохраняемые режимы
//  AT+CMGF=1     //текстовый режим
//  AT+CSCS="GSM" // режим GSM
//  AT+CLTS=1 // Get Local Timestamp
//  AT&W          // сохранение текущего профиля и нужно перезагрузить модем
// привязка BT-гарнитуры
// AT+BTPOWER=1  // включит BT
// AT+BTPAIRCFG=0 // сопряжение: 0 с подтверждением, 2 автоматическое 
// AT+BTUNPAIR=0 // удалить все сопряженные
// AT+BTSCAN=1   // сканирование доступных устройств
// AT+BTPAIR=0,1 // установить сопряжение с первым устройством из списка найденных
// AT+BTSTATUS?    // проверка есть ли в списке сопряженных
// AT+BTACPT=1    // подтверждение запроса на подключение;
  fonaSS.println(F("AT+CNMI=2,1,0,0,0")); //запись входящих SMS на СИМ-карту
  fonaSS.println(F("AT+CUSD=1")); // режим USSD - выполнить запрос, вернуть ответ
  fonaSS.println(F("AT+BTPOWER=1")); // включить BT
  delay(1500);
  pinMode(19, INPUT); // A5 определяем как цифровой вход
  pinMode(20, INPUT); // A6 определяем как цифровой вход
}

void loop(void) {
   uint8_t ri = analogRead(20);
      if (ri==0){
         if (CallStatus != 3) {
           delay(500);
           eventsms();
           CallStatus = fona.getCallStatus();
//           oldCallStatus = 5;
         }
      }
   uint8_t bdis = digitalRead(19);
    if (bdis ==0 && olddisp == 0) {
      olddisp = 1;
      if (disp == 0) {
       disp = 1;
      } else {
       disp = 0;  
      }
      DISPpower(disp);
    }
    if (bdis !=0 && olddisp == 1) {  
      olddisp = 0;
    }
    if ((millis() - new_millis > 30000) && disp == 1) {
      disp = 0;
      DISPpower(disp);
    }
  if (callstatus_i == 0) {
    event(); 
    CallStatus = fona.getCallStatus();
    if (disp==1 or incallnum == false){
       getGetBatt();
       Time();
    }
    if (CallStatus == 0) {
      if (bbook==0) {
        status(F("Ready"));
      }
      if (bbook==1 && sbook==0){
        status(F("Ready/Book"));
      }
      if (bbook==2 && sbook==0){
        status(F("Ready/SMS Send"));
      }
      if (bbook==3 && sbook==0){
        status(F("Ready/SMS Read"));
      }
      if (bbook==1 && sbook==1 && textfield_i !=0){
        status(F("Ready/Book/Save"));
      }
      if (bbook==2 && sbook==1 && textfield_i !=0){
        status(F("Ready/SMS Send/Send"));
      }
      if (bbook==1 && sbook==1 && textfield_i ==0){
        status(F("Ready/Book/Del"));
      }
      if (bbook==3 && sbook==1 && textfield_i ==0){
        status(F("Ready/SMS Read/Del"));
      }
      if (oldCallStatus != CallStatus){
          bmax = 15;
          create_butt(5);
          oldCallStatus = CallStatus;
      }

    }
    if (CallStatus == 2) {status(F("Unkown"));} 
//    Serial.println(incallnum);
//    Serial.println(CallStatus); 
    if (CallStatus == 3) {
      status(F("Ring"));
      if (!incallnum){
         fona.incomingCallNumber(textfield) ;
         char cring[] = "Ringing: ";
         status(strcat(cring, textfield));
         textfield_i = 12;
         clear_textfild();
         update_textfild();
         incallnum = true;
      } 
      if (oldCallStatus != CallStatus){
          bmax = 3;
          create_butt(1);
          oldCallStatus = CallStatus;
      }
          disp = 1;
          DISPpower(disp);
          ring(400,50); ring(600,100);
    } else { incallnum = false;}    
    if (CallStatus == 4) {
      status(F("Talk"));
      if (oldCallStatus != CallStatus){
          bmax = 3;
          create_butt(1);
          oldCallStatus = CallStatus;
        }
      }
    
    callstatus_i = 2000;
  }
  callstatus_i = callstatus_i-1;

  TSPoint p = ts.getPoint();  
  pinMode(XM, OUTPUT);                                   // Возвращаем режим работы вывода X- в значение «выход» для работы с дисплеем
  pinMode(YP, OUTPUT);                                   // Возвращаем режим работы вывода Y+ в значение «выход» для работы с дисплеем
  
  // Scale from ~0->4000 to tft.width using the calibration #'s
  if (p.z>100 && disp == 1) {
    new_millis = millis();
//      Выводим «сырые» показания TouchScreen:
// Serial.println((String) "X="+p.x+",\tY="+p.y);    
//    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
//    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
//    uint16_t x = p.x;
//    uint16_t y = p.y;
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 480);
  } else {p.x = p.y = -1;}
    
  // go thru all the buttons, checking if they were pressed
  for (uint8_t b=0; b<bmax; b++) {
    if (buttons[b].contains(p.x, p.y)) {
      //Serial.print("Pressing: "); Serial.println(b);
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed

  for (uint8_t b=0; b<bmax; b++) {
    if (buttons[b].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }
    
    if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert!
              
        // if a numberpad button, append the relevant # to the textfield
        if (b >= 3 && b <= 15) {
            if (bbook == 0 && textfield_i < TEXT_LEN) {
              ring(b*100, 50);  
              if (oldbutt!=b){
                oldbutt = b;
                textfield[textfield_i] = buttonlabels[b][0];
                textfield_i++;
	              textfield[textfield_i] = 0; // zero terminate
                ibut=1;
              } 
              else {
                if (ibut <= (strlen(buttonlabels[b])-1)) {
                  textfield_i--;
                  textfield[textfield_i] = buttonlabels[b][ibut];
                  textfield_i++;
                  textfield[textfield_i] = 0; // zero terminate
                  ibut++;
                  if (ibut >(strlen(buttonlabels[b])-1)) {ibut=0;}
                }
              }
            }
            char ph_number[13]="";
            char ph_name[13]="";
            if (bbook == 1 or bbook == 2){ 
                fona.getSIMSender(b-2, ph_number, 12);
                fona.getSIMSendernName(b-2, ph_name, 12);
            }   
            if (bbook == 1 && sbook == 0 && ph_number[0] != '\0'){ // выбор из книги
              ring(b*100, 50);  
              char csel[21] = "";
              sprintf(csel,"Book %d: ", b-2);
              status(strcat(csel, ph_name));
              strcpy(textfield, ph_number);
              textfield_i = strlen(textfield);
              textfield[textfield_i] = 0;
              clear_textfild();
              update_textfild();
              oldCallStatus = 5;
              bbook = 0; 
              oldbutt = 15;
            }
            char sms_name[13]="";
            char sms_number[13]="";
            if (bbook == 3) {
               fona.getSMSSender(b-2, sms_number, 13);
               fona.readSMS(b-2, sms_name, 44, 44);
            }
            if (bbook == 3 && sbook == 0 && sms_name[0] != '\0'){ // выбор из списка смс
              ring(b*100, 50); 
              char cselsms[51] = "";
              sprintf(cselsms,"SMS %d:",b-2);
              strcat(cselsms, sms_name);
              status(cselsms);
              strncpy(textfield, sms_number, 12);
              textfield_i = strlen(textfield);
              textfield[textfield_i] = 0;
              clear_textfild();
              update_textfild();
//              oldCallStatus = 5;
//              bbook = 0; 
//              oldbutt = 15;
            }
            if (bbook == 3 && sbook == 1 && textfield_i == 0){ // удаление SMS
              ring(b*100, 50);  
              char smsdel[13] = "";
              sprintf(smsdel,"Delete SMS %d",b-2);
              status(smsdel);
              fona.deleteSMS(b-2);
              bbook = 0;
              sbook = 0;
              oldCallStatus = 5;              
            }
            if (bbook == 1 && sbook == 1 && textfield_i == 0){ // удаление из книги
              ring(b*100, 50);  
              char cd[2] = "";
              itoa(b-2, cd, 10);
              char msgdel[15] = "Delete Phone ";
//              char msgdel[15] = "";
//              sprintf(msgdel,"Delete Phone %d", b-2);
//              status(msgdel);
              status(strcat(msgdel, cd));
              fonaSS.print(F("AT+CPBW="));
              fonaSS.println(b-2);
              bbook = 0;
              sbook = 0;
              oldCallStatus = 5;              
            }
            if (bbook == 1 && sbook == 1 && textfield_i != 0){ //запись в книгу имени и номера
              // AT+CPBW=4,"+71234567890",145,"Test"
              fonaSS.print(F("AT+CPBW="));
              fonaSS.print(b-2);
              fonaSS.print(F(",\""));
              if (textfield[0] == '+' or textfield[0] == '#') {
                fonaSS.print(textfield);
              } else {
                fonaSS.print(ph_number);
              }
              fonaSS.print(F("\",145,\""));
              if (ph_name[0] == '\0') fonaSS.print(b-2);
              if (ph_name[0] != '\0') fonaSS.print(textfield);            
              fonaSS.println(F("\""));
              ring(b*100, 50);  
              char sbb[2] = "";
              itoa(b-2, sbb, 10);
              char msgsave[12] = "Save Book ";
              strcat(msgsave, sbb);
              status(msgsave);
              bbook = 0;
              sbook = 0;
              oldCallStatus = 5;              
            }
            if (bbook == 2 && sbook == 0 && textfield_i != 0 && ph_number[0] != '\0'){ //отправить SMS
              fona.sendSMS(ph_number, textfield);
              delay(2000);
              ring(b*100, 50);  
              char smssave[23] = "Send SMS: ";
              strcat(smssave, ph_number);
              status(smssave);
              bbook = 0;
              sbook = 0;
              oldCallStatus = 5;              
            }
        }

        // clr button! delete char
        if (b == 1) {
          if (CallStatus == 0){
              ring(b*100, 50);  
              bmax = 11;
              if (bbook <= 2){
                bbook++;
              } else {
                bbook=1;
              }
              if ( bbook == 1) {
                status(F("Ready/Book"));
              }
              if ( bbook == 2) {
                status(F("Ready/SMS Send"));
                if (textfield_i == 0){
                  bbook = 3;
                }
              }
              if ( bbook == 3) {
                status(F("Ready/SMS Read"));
              }
              create_book();
              sbook = 0;
          } 
        }
        // its always OK to just hang up
        if (b == 2) {
          ring(b*100, 50);  
          oldbutt = 15;
          sbook = 0;
          if (CallStatus == 0 && textfield_i != 0 && bbook == 0){
            textfield[textfield_i] = 0;
            if (textfield_i > 0) {
              textfield_i--;
              textfield[textfield_i] = ' ';
            }
          } 
          else {
          status(F("End"));
          fona.hangUp();
          oldCallStatus = 5; 
          bbook = 0;
          }
        }
        // update the current text field
        update_textfild();
        // we dont really check that the text field makes sense
        // just try to call
        if (b == 0) { 
//          bbook = 0;
          oldbutt = 15; 
          if (CallStatus == 0 && (bbook == 1 or bbook == 3 )){ 
            ring(50, 50); 
            status(F("Save"));
            sbook=1;         
          } 
          if (CallStatus == 0 && (textfield[0] == '+' or textfield[0] == '#' or textfield[0] == '*') && bbook == 0){
            ring(50, 50);
            status(F("Call"));
            char call[20] = "Call: ";
            status(strcat(call, textfield));
            bmax = 3;
            create_butt(1);
            fona.callPhone(textfield);
            oldCallStatus = 5;
          } 
          if (CallStatus == 3){
            ring(50, 50); 
            status(F("Answ"));
            fona.pickUp();            
          }
        }
//                    Serial.print(CallStatus);
        delay(100);
  }    

  }  

}
  
void Time() {
  //+CCLK: "18/12/09,14:38:38+20"
  char buff[20] = "";
  while(!fona.getTime(buff, 19));
  char timeB[9]="";
  char dateB[9]="";
  if (buff[0] != '\0'){
    uint8_t inbr = 1;
    for (uint8_t i=0; i<=7; i++) {
        dateB[i] = buff[inbr];
        inbr++ ;                 
    }
    dateB[9]= '\0';
    inbr=10;
    for (uint8_t i=0; i<=7; i++) {
        timeB[i] = buff[inbr];
        inbr++ ;                 
    }
    timeB[9]= '\0';
  }
  tft.fillRect(105, 10, 320, 16,  BLACK);
  tft.setCursor(105, 10);
  tft.setTextColor(DARKGREY);
  tft.setTextSize(2);
  tft.print(dateB); tft.print(" "); tft.print(timeB);
}

void getGetBatt() {
  //+CBC: 0,99,4205
  char batt[12] = "";
  while(!fona.getBatt(batt, 11));
  char batt_pr[4]="";
    uint8_t li = 0;
    while ( batt[li] != ',') { 
      li++ ;
    }
    li=li+1;
    uint8_t i = 0;
    while ( batt[li] != ',') {
      batt_pr[i] = batt[li];
      li++ ;
      i++;
    }
    batt_pr[i] = '\0';
  uint8_t rssi = fona.getRSSI();
  tft.fillRect(10, 10, 95, 16,  BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(DARKGREY);
  tft.setTextSize(2);
  tft.print(batt_pr); tft.print("% "); tft.print(rssi); tft.print(bt);
}

void update_textfild(void){
        // update the current text field
        tft.setCursor(TEXT_X-2, TEXT_Y+10);
        tft.setTextColor(TEXT_TCOLOR,  BLACK);
        tft.setTextSize(TEXT_TSIZE);
        tft.print(textfield);
}

void clear_textfild(void){
        tft.fillRect(TEXT_X-9, TEXT_Y+1, TEXT_W-2, TEXT_H-2,  BLACK);
}

void ring(uint16_t tones, uint16_t durat) {
        tone( 11, tones, durat * 2 );
        delay( durat * 2 );
        noTone( 11 );
}

void DISPpower(uint8_t disp) {
  pinMode(10, OUTPUT);
  if (disp == 1){
    digitalWrite(10, LOW);
  }
  if (disp == 0){
    digitalWrite(10, HIGH);
  }
  ring(400, 50);
  new_millis = millis();
}
 
void FONApowerOn() {
  delay(1000);
    pinMode(11, OUTPUT);
//    if (!digitalRead(10)) {
        digitalWrite(11, HIGH);
        delay(1000);
//    }
    digitalWrite(11, LOW);
    delay(1000);
}

void create_butt(uint8_t row_button) {

uint16_t buttoncolors[15] = { DARKGREEN,  DARKGREY,  RED, 
                              BLUE,  BLUE,  BLUE, 
                              BLUE,  BLUE,  BLUE, 
                              BLUE,  BLUE,  BLUE, 
                              ORANGE,  BLUE,  ORANGE};
    tft.fillRect(0, 150, 320, 480, BLACK);
    for (uint8_t row=0; row<row_button; row++) {
      for (uint8_t col=0; col<3; col++) {
        buttons[col + row*3].initButton(&tft, 60+col*(80+20), 
                 175+row*(50+17),    // x, y, w, h, outline, fill, text
                  80, 50,  WHITE, buttoncolors[col+row*3],  WHITE,
                  buttonlabels[col + row*3], 2); 
        buttons[col + row*3].drawButton();
      }
    }
}


void create_book() {
char phonename[8][7];

    tft.fillRect(0, 200, 320, 480, BLACK);
    uint8_t ib=1;
    for (uint8_t row=3; row<=bmax-1; row++) {
        uint8_t col=0;
        phonename[ib][0] = '\0';
        if (bbook==1 or bbook==2 ){
          fona.getSIMSendernName(ib, phonename[ib], 12);
        }
        if (bbook==3 && sbook == 0){
          fona.readSMS(ib, phonename[ib],7, 7);
        }
        buttons[col + row].initButton(&tft, 160, 
                 118+row*(30+4),    // x, y, w, h, outline, fill, text
                  250, 28,  WHITE, BLUE,  WHITE,
                  phonename[ib], 2); 
        buttons[col + row].drawButton();
        ib=ib+1;
    }
}

void reply(char *varreply, uint8_t n, uint8_t k) {
//    Serial.println(n); Serial.println(k);
    uint8_t inbr = 0;
    if (k >= n) {
      for (uint8_t i=n; i<=k; i++) {
        varreply[inbr] = replybuf[i];
        inbr++ ;                 
      }
    }
    varreply[inbr]= '\0';
}

void eventsms() {
  readline(500);
  //RING+CLIP: "<incoming phone number>",145,"",0,"",0
//  if (strncmp(replybuf, "RING+CLIP: ", 11) == 0 && incallnum == 0){
//    uint8_t li = 0;
//    while ( replybuf[li] != '\"') {   
//      li++ ;
//    }
//    li=li+1;
//    uint8_t ln=li;
//    while ( replybuf[li] != '\"') {   
//      li++ ;
//    }
//    uint8_t lk=li-1;
//    char nsms[13]="";
//    reply(nsms, ln, lk);
//    char newsms[] = "Ringing: ";
//    strcat(newsms, nsms);
//    status(newsms);
//    strncpy(textfield, nsms, 12);
//    textfield_i = strlen(textfield);
//    textfield[textfield_i] = 0;
//    update_textfild();
//    incallnum = 1;
//  } 
  //+CMTI: "SM",4
  if (strncmp(replybuf, "+CMTI: \"SM\",", 11) == 0){
    uint8_t len = strlen(replybuf);
    uint8_t li = len;
    while ( replybuf[li] != ',') {   
      li-- ;
    }
    char nsms[2]="";
    reply(nsms, li+1, li+1);
    disp = 1;
    DISPpower(disp);
    uint8_t tsms = strtoul(nsms, NULL, 10);
    clear_textfild();
    char sms_name[15]="";
    char sms_number[13]="";
    delay(300);
    while(!fona.readSMS(tsms, sms_name, 15, 15));
    while(!fona.getSMSSender(tsms, sms_number, 13));
    char newsms[] = "";
    sprintf(newsms,"New SMS %d:", tsms);
    strcat(newsms, sms_name);
    status(newsms);
    ring(400, 50);
    strncpy(textfield, sms_number, 12);
    textfield_i = strlen(textfield);
    textfield[textfield_i] = 0;
    update_textfild();
    uint8_t db = tsms+1;
    if (tsms == 5) db = 1;
    while(!fona.deleteSMS(db));
  }
}

void event() {
  char ussd[13] = "";
  readline(100);
  //+CUSD: 0,"Balance:439r",64
  if (strncmp(replybuf, "+CUSD: ", 7) == 0){
//    uint8_t len = strlen(replybuf);
    uint8_t li = 0;
    while ( replybuf[li] != '\"') {   
      li++ ;
    }
    li=li+1;
    uint8_t ln=li;
    while ( replybuf[li] != '\"') {   
      li++ ;
    }
    uint8_t lk=li-1;
    reply(ussd, ln, lk);
    char newussd[] = "USSD: ";
    strcat(newussd, ussd);
    newussd[25] = '\0';
    status(newussd);
    ring(400, 50);
    disp = 1;
    DISPpower(disp);
  }
  //+BTCONNECTING: 1c:52:16:b3:f5:a8,"HFG"
    if (strncmp(replybuf, "+BTCONNECTING: ", 15) == 0){
      fonaSS.println(F("AT+BTACPT=1"));
      readline(200);
      //OK+BTCONNECT: 1,"Mi Bluetooth Hea",1c:52:16:b3:f5:a8,"HFG"
      if (strncmp(replybuf, "OK+BTCONNECT: ", 14) == 0){
        uint8_t li = 0;
        while ( replybuf[li] != '\"') {   
          li++ ;
        }
        li=li+1;
        uint8_t ln=li;
        while ( replybuf[li] != '\"') {   
          li++ ;
        }
        uint8_t lk=li-1;
        reply(ussd, ln, lk);
        char newussd[] = "BT CONNECT: ";
        strcat(newussd, ussd);
        newussd[25] = '\0';
        status(newussd);
      }
      ring(400, 50);
      disp = 1;
      DISPpower(disp);
      bt[0]= '*';
      bt[1]= '\0';
  }
  //+BTDISCONN: "Mi Bluetooth Hea",1c:52:16:b3:f5:a8,"HFG"
  if (strncmp(replybuf, "+BTDISCONN: ", 12) == 0){
      uint8_t li = 0;
      while ( replybuf[li] != '\"') {   
        li++ ;
      }
      li=li+1;
      uint8_t ln=li;
      while ( replybuf[li] != '\"') {   
        li++ ;
      }
      uint8_t lk=li-1;
      reply(ussd, ln, lk);
      char newussd[] = "BT DISCONNECT: ";
      strcat(newussd, ussd);
      newussd[25] = '\0';
      status(newussd);
      ring(400, 50);
      disp = 1;
      DISPpower(disp);
      bt[0]= '.';
      bt[1]= '\0';
  }
}

void readline(uint16_t timeout) {
  uint16_t replyidx = 0;
  while (timeout--) {
    if (replyidx >= 255) { //обязательно буфер модема читаем до конца 256 символов
      break;
    }
    while(fonaSS.available()) {
      char c =  fonaSS.read();
      if (c == '\n') continue;
      if (c == '\r') continue;
//      if (c == 0xA) {
//        if (replyidx == 0)   // the first 0x0A is ignored
//          continue;
//      }
      replybuf[replyidx] = c;
      replyidx++;
    }
    if (timeout == 0) {
      break;
    }
    delay(1);
  }
    replybuf[replyidx] = 0;
    //Serial.println(timeout); //Serial.println(replyidx); //      
//    Serial.println(replybuf);
}

// Print something in the mini status bar with either flashstring
void status(const __FlashStringHelper *msg) {
  tft.fillRect(10, 125, 320, 16,  BLACK);
  tft.setCursor(10, 125);
  tft.setTextColor( WHITE);
  tft.setTextSize(2);
  tft.print(msg);
}
// or charstring
void status(char *msg) {
//  char msg1[26] = "";
//  char msg2[26] = "";
  tft.fillRect(0, 85, 320, 38,  BLACK);
//  uint8_t len = strlen(msg);
//  uint8_t li = 0;
//    while ( li < 25 && li < len ) {
//      msg1[li] = msg[li];
//      li++ ;
//    }
//    msg1[li] = '\0';
//    uint8_t li2 = 0;
//    while ( li2 < 25 && li < len ) {
//      msg2[li2] = msg[li];
//      li++ ;
//      li2++ ;
//    }
//    msg2[li2] = '\0';
  tft.setCursor(10, 85);
  tft.setTextColor(DARKGREY);
  tft.setTextSize(2);
  tft.print(msg);
//  tft.setCursor(10, 105);
//  tft.setTextColor(DARKGREY);
//  tft.setTextSize(2);
//  tft.print(msg2);
}
