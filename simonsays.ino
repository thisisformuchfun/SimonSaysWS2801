/*Simon Says game. Now with sound effects. 
Buttons are to be set on there designated pins without pull down resistors
and connected to ground rather then +5. 

This is a test for master
*/

#include "SPI.h"
#include "Adafruit_WS2801.h"
#include <Tone.h>
#include <PS2X_lib.h>  //for v1.6
#include <SoftwareServo.h>

SoftwareServo servo1;

PS2X ps2x; // create PS2 Controller Class


//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you conect the controller, 
//or call config_gamepad(pins) again after connecting the controller.
int error = 0; 
byte type = 0;
byte vibrate = 0;

Tone tone1;

#define OCTAVE_OFFSET 0

int notes[] = { 0,
NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4,
NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5,
NOTE_C6, NOTE_CS6, NOTE_D6, NOTE_DS6, NOTE_E6, NOTE_F6, NOTE_FS6, NOTE_G6, NOTE_GS6, NOTE_A6, NOTE_AS6, NOTE_B6,
NOTE_C7, NOTE_CS7, NOTE_D7, NOTE_DS7, NOTE_E7, NOTE_F7, NOTE_FS7, NOTE_G7, NOTE_GS7, NOTE_A7, NOTE_AS7, NOTE_B7
};

char *song = "smbdeath:d=4,o=5,b=90:32c6,32c6,32c6,8p,16b,16f6,16p,16f6,16f.6,16e.6,16d6,16c6,16p,16e,16p,16c"; 


Tone speakerpin;
int starttune[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int duration2[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};
int note[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int duration[] = {100, 100, 100, 300, 100, 300};
boolean button[] = {3, 4, 5, 6}; //The four button input pins
boolean ledpin[] = {8, 9, 10, 11};  // LED pins



// Choose which 2 pins you will use for output.
// Can be any valid output pins.
// The colors of the wires may be totally different so
// BE SURE TO CHECK YOUR PIXELS TO SEE WHICH WIRES TO USE!
int dataPin  = 12;    // Yellow wire on Adafruit Pixels
int clockPin = 13;    // Green wire on Adafruit Pixels
// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(5, dataPin, clockPin, WS2801_GRB);

int howfar = 0; //how far have they gotten?!
int turn = 0;  // turn counter
int buttonstate = 0;  // button state checker
int randomArray[100]; //Intentionally long to store up to 100 inputs (doubtful anyone will get this far)
int inputArray[100];  


void setup() 
{
  Serial.begin(57600);

 //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
 error = ps2x.config_gamepad(7,10,9,8, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
 
 if(error == 0){
   Serial.println("Found Controller, configured successful");
   Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
  Serial.println("holding L1 or R1 will print out the analog stick values.");
  Serial.println("Go to www.billporter.info for updates and to report bugs.");
 }
   
  else if(error == 1)
   Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
   Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
   
  else if(error == 3)
   Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
   
   //Serial.print(ps2x.Analog(1), HEX);
   
   type = ps2x.readType(); 
     switch(type) {
       case 0:
        Serial.println("Unknown Controller type");
       break;
       case 1:
        Serial.println("DualShock Controller Found");
       break;
       case 2:
         Serial.println("GuitarHero Controller Found");
       break;
     }
  
  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();
  speakerpin.begin(11); // speaker is on pin 11


  
  for(int x=0; x<4; x++) 
  {
    pinMode(button[x], INPUT);  // button pins are inputs
    digitalWrite(button[x], HIGH);  // enable internal pullup; buttons start in high position; logic reversed
  }
  pinMode(2,OUTPUT);
  servo1.attach(2);
  servo1.write(90);
  randomSeed(analogRead(0)); //Added to generate "more randomness" with the randomArray for the output function
  for (int thisNote = 0; thisNote < 13; thisNote ++) {
     // play the next note:
     speakerpin.play(starttune[thisNote]);
     // hold the note:
     if (thisNote==0 || thisNote==2 || thisNote==4 || thisNote== 6)
     {
       SetButton(1);
     }
     if (thisNote==1 || thisNote==3 || thisNote==5 || thisNote== 7 || thisNote==9 || thisNote==11)
     {
       SetButton(2);
     }
     if (thisNote==8 || thisNote==12)
     {
       SetButton(3);
     }  
     if (thisNote==10)
     {   
       SetButton(4);
     }
     delay(duration2[thisNote]);
     // stop for the next note:
     speakerpin.stop();
     SetAllOff();
     delay(25);
    }
  delay(500);
}

#define isdigit(n) (n >= '0' && n <= '9')
 
void loop() 
{   
  ps2x.read_gamepad(false, false);
  Serial.println("This is the start of loop");
  for (int y=0; y<=99; y++)
  {
    //function for generating the array to be matched by the player
    SetAllOn();
  
    for (int thisNote = 0; thisNote < 6; thisNote ++) {
     // play the next note:
     speakerpin.play(note[thisNote]);
     // hold the note:
     delay(duration[thisNote]);
     // stop for the next note:
     speakerpin.stop();
     delay(10);
     Serial.println("This is what the world wants");
    }
    
    SetAllOff();
    //delay(500);
  
    for (int y=turn; y <= turn; y++)
    { //Limited by the turn variable
  ps2x.read_gamepad(false, false);
  int gobanger=0;
 /* while (ps2x.Button(PSB_RED)!= true) {
      ps2x.read_gamepad();
        if(ps2x.Button(PSB_RED))                   //will be TRUE as long as button is pressed
         Serial.println("Start is being held");
    rainbow(1);
  }*/
  Serial.println("This is the start of limited for loop");
      Serial.println(""); //Some serial output to follow along
      Serial.print("Turn: ");
      Serial.print(y);
      Serial.println("");
     ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
      randomArray[y] = random(1, 5); //Assigning a random number (1-4) to the randomArray[y], y being the turn count
      for (int x=0; x <= turn; x++)
      {
          Serial.println("This is the start of limited x-variable for loop");
        Serial.print(randomArray[x]);
      
        for(int y=0; y<4; y++)
        {
      
          if (randomArray[x] == 1 && ledpin[y] == 8) 
          {  //if statements to display the stored values in the array
     ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
            SetButton(1);
            speakerpin.play(NOTE_G3, 100);
            delay(400);
            SetButton(0);
            delay(100);
          }

          if (randomArray[x] == 2 && ledpin[y] == 9) 
          {
                 ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
            SetButton(2);
            speakerpin.play(NOTE_A3, 100);
            delay(400);
            SetButton(0);
            delay(100);
          }
  
          if (randomArray[x] == 3 && ledpin[y] == 10) 
          {     ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
            SetButton(3);
            speakerpin.play(NOTE_B3, 100);
            delay(400);
            SetButton(0);
            delay(100);
          }

          if (randomArray[x] == 4 && ledpin[y] == 11) 
          {     ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
            SetButton(4);
            speakerpin.play(NOTE_C4, 100);
            delay(400);
            SetButton(0);
            delay(100);
          }
        }
      }
    }
    input();
  }
}
 
 void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
 
 
void SetAllOn() {
       ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
      uint32_t c=Color(255, 0, 0);
      strip.setPixelColor(0, c);
      strip.show();
      c=Color(0, 255, 0);
      strip.setPixelColor(1, c);
      strip.show();
      c=Color(0, 0, 255);
      strip.setPixelColor(2, c);
      strip.show();
      c=Color(255, 200, 0);
      strip.setPixelColor(3, c);
      strip.show();
      c=Color(255, 255, 255);
      strip.setPixelColor(4, c);
      strip.show();
}
void SetAllRED() {
       ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
      uint32_t c=Color(255, 0, 0);
      strip.setPixelColor(0, c);
      strip.show();
      strip.setPixelColor(1, c);
      strip.show();
      strip.setPixelColor(2, c);
      strip.show();
      strip.setPixelColor(3, c);
      strip.show();
      strip.setPixelColor(4, c);
      strip.show();
}
void SetAllOff() {
       ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
      uint32_t c=Color(0, 0, 0);
      strip.setPixelColor(0, c);
      strip.show();
      strip.setPixelColor(1, c);
      strip.show();
      strip.setPixelColor(2, c);
      strip.show();
      strip.setPixelColor(3, c);
      strip.show();
      strip.setPixelColor(4, c);
      strip.show();
}
void SetButton(int b) {
  uint32_t c;
       ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
  switch (b) {
    case 1:    
      c=Color(255, 0, 0);
      strip.setPixelColor(0, c);
      strip.show();
      strip.setPixelColor(1, 0);
      strip.show();
      strip.setPixelColor(2, 0);
      strip.show();
      strip.setPixelColor(3, 0);
      strip.show();
      c=Color(255, 255, 255);
      strip.setPixelColor(4, c);
      strip.show();
      break;
    case 2:    
      c=Color(0, 255, 0);
      strip.setPixelColor(0, 0);
      strip.show();
      strip.setPixelColor(1, c);
      strip.show();
      strip.setPixelColor(2, 0);
      strip.show();
      strip.setPixelColor(3, 0);
      strip.show();
      c=Color(255, 255, 255);
      strip.setPixelColor(4, c);
      strip.show();
      break;
    case 3:    
      c=Color(0, 0, 255);
      strip.setPixelColor(0, 0);
      strip.show();
      strip.setPixelColor(1, 0);
      strip.show();
      strip.setPixelColor(2, c);
      strip.show();
      strip.setPixelColor(3, 0);
      strip.show();
      c=Color(255, 255, 255);
      strip.setPixelColor(4, c);
      strip.show();
      break;
    case 4:    
      c=Color(255, 200, 0);
      strip.setPixelColor(0, 0);
      strip.show();
      strip.setPixelColor(1, 0);
      strip.show();
      strip.setPixelColor(2, 0);
      strip.show();
      strip.setPixelColor(3, c);
      strip.show();
      c=Color(255, 255, 255);
      strip.setPixelColor(4, c);
      strip.show();
      break;
    default:
      // turn all the LEDs off:
      SetAllOff();
    }
}




void play_rtttl(char *p)
{
  // Absolutely no error checking in here

  byte default_dur = 4;
  byte default_oct = 6;
  int bpm = 63;
  int num;
  long wholenote;
  long duration;
  byte note;
  byte scale;

  // format: d=N,o=N,b=NNN:
  // find the start (skip name, etc)

  while(*p != ':') p++;    // ignore name
  p++;                     // skip ':'

  // get default duration
  if(*p == 'd')
  {
    p++; p++;              // skip "d="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    if(num > 0) default_dur = num;
    p++;                   // skip comma
  }

  Serial.print("ddur: "); Serial.println(default_dur, 10);

  // get default octave
  if(*p == 'o')
  {
    p++; p++;              // skip "o="
    num = *p++ - '0';
    if(num >= 3 && num <=7) default_oct = num;
    p++;                   // skip comma
  }

  Serial.print("doct: "); Serial.println(default_oct, 10);

  // get BPM
  if(*p == 'b')
  {
    p++; p++;              // skip "b="
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    bpm = num;
    p++;                   // skip colon
  }

  Serial.print("bpm: "); Serial.println(bpm, 10);

  // BPM usually expresses the number of quarter notes per minute
  wholenote = (60 * 1000L / bpm) * 4;  // this is the time for whole note (in milliseconds)

  Serial.print("wn: "); Serial.println(wholenote, 10);


  // now begin note loop
  while(*p)
  {
    // first, get note duration, if available
    num = 0;
    while(isdigit(*p))
    {
      num = (num * 10) + (*p++ - '0');
    }
    
    if(num) duration = wholenote / num;
    else duration = wholenote / default_dur;  // we will need to check if we are a dotted note after

    // now get the note
    note = 0;

    switch(*p)
    {
      case 'c':
        note = 1;
        break;
      case 'd':
        note = 3;
        break;
      case 'e':
        note = 5;
        break;
      case 'f':
        note = 6;
        break;
      case 'g':
        note = 8;
        break;
      case 'a':
        note = 10;
        break;
      case 'b':
        note = 12;
        break;
      case 'p':
      default:
        note = 0;
    }
    p++;

    // now, get optional '#' sharp
    if(*p == '#')
    {
      note++;
      p++;
    }

    // now, get optional '.' dotted note
    if(*p == '.')
    {
      duration += duration/2;
      p++;
    }
  
    // now, get scale
    if(isdigit(*p))
    {
      scale = *p - '0';
      p++;
    }
    else
    {
      scale = default_oct;
    }

    scale += OCTAVE_OFFSET;

    if(*p == ',')
      p++;       // skip comma for next note (or we may be at the end)

    // now play the note

    if(note)
    {
      Serial.print("Playing: ");
      Serial.print(scale, 10); Serial.print(' ');
      Serial.print(note, 10); Serial.print(" (");
      Serial.print(notes[(scale - 4) * 12 + note], 10);
      Serial.print(") ");
      Serial.println(duration, 10);
      tone1.play(notes[(scale - 4) * 12 + note]);
      delay(duration);
      tone1.stop();
    }
    else
    {
      Serial.print("Pausing: ");
      Serial.println(duration, 10);
      delay(duration);
    }
  }
}





void input() { //Function for allowing user input and checking input against the generated array
     ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
  for (int x=0; x <= turn;)
  { //Statement controlled by turn count

    for(int y=0; y<4; y++)
    {
      
      buttonstate = digitalRead(button[y]);
    
      if (buttonstate == LOW && button[y] == 4)
      { //Checking for button push
           ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
        SetButton(1);
        speakerpin.play(NOTE_G3, 100);
        delay(200);
        SetButton(0);
        digitalWrite(ledpin[0], LOW);
        inputArray[x] = 1;
        delay(250);
        Serial.print(" ");
        Serial.print(1);
        if (inputArray[x] != randomArray[x]) { //Checks value input by user and checks it against
          fail();                              //the value in the same spot on the generated array
        }                                      //The fail function is called if it does not match
        x++;
      }
       if (buttonstate == LOW && button[y] == 5)
      {
             ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
        SetButton(2);
        speakerpin.play(NOTE_A3, 100);
        delay(200);
        SetButton(0);
        inputArray[x] = 2;
        delay(250);
        Serial.print(" ");
        Serial.print(2);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == 6)
      {
             ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
        SetButton(3);
        speakerpin.play(NOTE_B3, 100);
        delay(200);
        SetButton(0);
        inputArray[x] = 3;
        delay(250);
        Serial.print(" ");
        Serial.print(3);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == 3)
      {
             ps2x.read_gamepad();
     if(ps2x.Button(PSB_RED))             //will be TRUE if button was JUST pressed
     Serial.println("Green Fret Pressed");
        SetButton(4);
        speakerpin.play(NOTE_C4, 100);
        delay(200);
        SetButton(0);
        inputArray[x] = 4;
        delay(250);
        Serial.print(" ");
        Serial.print(4);
        if (inputArray[x] != randomArray[x]) 
        {
          fail();
        }
        x++;
      }
    }
  }
  delay(500);
  howfar++;
  turn++; //Increments the turn count, also the last action before starting the output function over again
}

void fail() { //Function used if the player fails to match the sequence
 play_rtttl(song); ///SMB dead mario sound effect!
 servo1.write(180);
 SoftwareServo::refresh();
  for (int y=0; y<=5; y++)
  { //Flashes lights for failure
    
    SetAllRED();
    speakerpin.play(NOTE_B2, 300);
    delay(200);
    SetAllOff();
    speakerpin.play(NOTE_C3, 300);
    delay(200);
  }
  SetAllRED();
  delay(500);
  turn = -1; //Resets turn value so the game starts over without need for a reset button
    if (howfar >= 4)
  {
    for (int y=0; y<=3; y++)
    { //Flashes lights for failure
    
    SetAllRED();
    speakerpin.play(NOTE_G5, 300);
    delay(200);
    SetAllOff();
    speakerpin.play(NOTE_C4, 300);
    delay(200);
    }
  }
  if (howfar >= 6)
  {
    for (int y=0; y<=3; y++)
    { //Flashes lights for failure
    
    SetAllRED();
    speakerpin.play(NOTE_G6, 300);
    delay(200);
    SetAllOff();
    speakerpin.play(NOTE_C5, 300);
    delay(200);
    }
   turn = -2;//resets to the coin loop! 
  }
    if (howfar >= 8)
  {
    for (int y=0; y<=3; y++)
    { //Flashes lights for failure
    
    SetAllRED();
    speakerpin.play(NOTE_G7, 300);
    delay(200);
    SetAllOff();
    speakerpin.play(NOTE_C6, 300);
    delay(200);
    }
   turn = -2;//resets to the coin loop! 
  }
      if (howfar >= 10)
  {
    for (int y=0; y<=3; y++)
    { //Flashes lights for failure
    
    SetAllRED();
    speakerpin.play(NOTE_A7, 300);
    delay(200);
    SetAllOff();
    speakerpin.play(NOTE_C7, 300);
    delay(200);
    }
   turn = -2;//resets to the coin loop! 
  }
  SetAllOff();
  servo1.write(90);
  SoftwareServo::refresh();
  howfar=0;
}


