#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include <gfxfont.h>
//#include <SD.h>
#include <EEPROM.h>
#include <Wire.h> 
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// Global variables:
// Game variables
bool success = false;
int selectedGame = 0;
int mainMenuRows = 3;
int mainMenuColumns = 3;
int numGames = 3;

// Timer variables
unsigned long previousTime = 0, currentTime = 0;

// Inputs (buttonOne = red, buttonTwo = green)
bool joystickLeftInput = false;
bool joystickUpInput = false;
bool joystickRightInput = false;
bool joystickDownInput = false;
bool buttonOneInput = false;
bool buttonTwoInput = false;
bool coinSlotButtonInput = false;
bool startButtonInput = false;

// Pins: 
// Pins for joystick 
int joystickLeftPin = 5;
int joystickUpPin = 6;
int joystickRightPin = 7;
int joystickDownPin = 8;

// Pins for two input buttons
int buttonOnePin = 12;
int buttonTwoPin = 3;

// Pins for coin slot button
int coinSlotButtonPin = 4;

// Pin for Start button
int startButtonPin = 9;

// Pins for DFPlayer
int DFPlayer_RX = 10;
int DFPlayer_TX = 11;

// Pins for LCD
int lcd_RST = 0;
int lcd_DC = 23;
int lcd_CS = 24;
int lcd_MOSI = 25;
int lcd_SCK = 26;

// LCD Setup
//Adafruit_ST7735 lcd = Adafruit_ST7735(lcd_CS, lcd_DC, lcd_MOSI, lcd_SCK, lcd_RST);
Adafruit_ST7735 lcd = Adafruit_ST7735(lcd_CS, lcd_DC, lcd_RST);

// DFPlayer Setup
SoftwareSerial SoftwareSerial(DFPlayer_RX, DFPlayer_TX); // RX, TX
DFRobotDFPlayerMini DFPlayer;

// Setup default values for the board
void setup() {
  Serial.begin(9600);
  SoftwareSerial.begin(9600);
  Serial.print("Loading Game...");

  // lcd
  lcd.initR(INITR_BLACKTAB);

  /*delay(1000);
  currentTime = millis();

  while (!DFPlayer.begin(SoftwareSerial)) {
    delay(1000);

    if(millis() - currentTime > 10000){
      Serial.print("DFPlayer Error");
      while(true);
    }
  }

  DFPlayer.volume(25);*/

  // pinMode Setups
  pinMode(joystickLeftPin, INPUT);
  pinMode(joystickUpPin, INPUT);
  pinMode(joystickRightPin, INPUT);
  pinMode(joystickDownPin, INPUT);

  pinMode(buttonOnePin, INPUT);
  pinMode(buttonTwoPin, INPUT);

  pinMode(coinSlotButtonPin, INPUT);
  pinMode(startButtonPin, INPUT);

  pinMode(lcd_DC, OUTPUT);
  pinMode(lcd_CS, OUTPUT);
  pinMode(lcd_MOSI, OUTPUT);
  pinMode(lcd_SCK, OUTPUT);  

  // Use analog input to generate random noise to choose a correct input
  randomSeed(analogRead(0));

  Serial.print("lcd test\n");
  lcd.print("lcd test\n");
  delay(500);
}

// Main Loop
void loop() {
  readInputs();

  

  // Change game selction
  if(joystickLeftInput == true && selectedGame >= 2){
    selectedGame--;            
  }
  else if(joystickUpInput == true && selectedGame > mainMenuColumns){
    selectedGame -= mainMenuColumns;
  }
  else if(joystickRightInput == true && selectedGame < numGames){
    selectedGame++;
  }
  else if(joystickDownInput == true && selectedGame <= numGames-mainMenuColumns){
    selectedGame += mainMenuColumns;
  }

  if(startButtonInput == true){
    switch(selectedGame){
      case 1:
        gameOne();
        break;
      case 2:
        gameTwo();
        break;
      case 3:      
        gameThree();
        break;
    }        
  }
}

// Game One:
void gameOne(){
  
}

// Game Two:
void gameTwo(){

}

// Game Three:
void gameThree(){

}

// Reads all input pins into bool variables
void readInputs() {
  joystickLeftInput = (digitalRead(joystickLeftPin) == HIGH);
  joystickUpInput = (digitalRead(joystickUpPin) == HIGH);
  joystickRightInput = (digitalRead(joystickRightPin) == HIGH);
  joystickDownInput = (digitalRead(joystickDownPin) == HIGH);
  buttonOneInput = (digitalRead(buttonOnePin) == HIGH);
  buttonTwoInput = (digitalRead(buttonTwoPin) == HIGH);
  coinSlotButtonInput = (digitalRead(coinSlotButtonPin) == HIGH);
  startButtonInput = (digitalRead(startButtonPin) == HIGH);
}

/* Tests if inputs are correct based on correctInput
 * buttonOne - 0, buttonTwo - 1, coinSlotButton - 2,  
 * joystickLeft - 3, joystickUp - 4, joystickRight - 5, joystickDown - 6
 */
bool testInputs(int correctInput) {
  bool failureStatus = false;

  switch (correctInput) {
    case 0:
      success = buttonOneInput && !(buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      failureStatus = (buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      break;
    case 1:
      success = buttonTwoInput && !(buttonOneInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      failureStatus = (buttonOneInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      break;
    case 2:
      success = coinSlotButtonInput && !(buttonOneInput || buttonTwoInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      failureStatus = (buttonOneInput || buttonTwoInput || joystickLeftInput || joystickUpInput || joystickRightInput || joystickDownInput);
      break;
    case 3:
      success = joystickLeftInput && !(buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickUpInput || joystickRightInput || joystickDownInput);
      failureStatus = (buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickUpInput || joystickRightInput || joystickDownInput);
      break;
    case 4:
      success = joystickUpInput && !(buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickRightInput || joystickDownInput);
      failureStatus = (buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickRightInput || joystickDownInput);
      break;
    case 5:
      success = joystickRightInput && !(buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickDownInput);
      failureStatus = (buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickDownInput);
      break;
    case 6:
      success = joystickDownInput && !(buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput);
      failureStatus = (buttonOneInput || buttonTwoInput || coinSlotButtonInput || joystickLeftInput || joystickUpInput || joystickRightInput);
      break;
  }

  return failureStatus;
}

// Timer Setup
void timerSetup() {
  cli();  //stop interrupts

  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   //initialize counter value to 0

  // set compare match register (Change this to change interrupt frequency)
  OCR1A = 5124;  // = (16*10^6) / (1*1024) - 1 (must be <65536)

  // turn on CTC mode
  TCCR1B |= (1 << WGM12);

  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();  //allow interrupts
}

ISR(TIMER1_COMPA_vect) {
  Serial.println(millis());
}