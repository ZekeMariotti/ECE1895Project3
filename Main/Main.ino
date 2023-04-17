#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

#include <gfxfont.h>
//#include <SD.h>
#include <EEPROM.h>
#include <Wire.h> 
#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"

#include <string.h>

// Color definitions
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define ORANGE   0xF460
#define GREEN    0x07E0
#define CYAN     0x07FF
#define PURPLE   0xA020F0
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// Global variables:
// Game variables
bool success = false;
bool startScreen = false;
int selectedGame = 1;
int mainMenuRows = 3;
int mainMenuColumns = 3;
int numGames = 3;

// Game One: Tetris
int blockSize = 5;
int gamePiece[64] = {0};
int gameMatrix[10][22] = {0};

int nextPiece = 0;
int pieceRotation = 0;
int xPos = 0;
int yPos = 0;
int firstLeftColumn = 0;
int firstRightColumn = 0;

// Each piece is max 4 wide, 4 tall, and 4 rotations.
// Pieces: I, L, J, T, S, Z, O
const int piece_I[] = {
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,
  0,0,0,0,

  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  0,0,1,0,
  
  0,0,0,0,
  0,0,0,0,
  1,1,1,1,
  0,0,0,0,

  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,1,0,0,
};

const int piece_L[] = {
  0,0,1,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  0,1,1,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  1,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const int piece_J[] = {
  1,0,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,1,1,0,
  0,1,0,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,1,0,
  0,0,1,0,
  0,0,0,0,
  
  0,1,0,0,
  0,1,0,0,
  1,1,0,0,
  0,0,0,0,

};

const int piece_T[] = {
  0,1,0,0,
  1,1,1,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,
  
  0,0,0,0,
  1,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,1,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,

};

const int piece_S[] = {
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,

  0,1,0,0,
  0,1,1,0,
  0,0,1,0,
  0,0,0,0,

  0,0,0,0,
  0,1,1,0,
  1,1,0,0,
  0,0,0,0,

  1,0,0,0,
  1,1,0,0,
  0,1,0,0,
  0,0,0,0,
};

const int piece_Z[] = {
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  0,0,0,0,
  
  0,0,1,0,
  0,1,1,0,
  0,1,0,0,
  0,0,0,0,

  0,0,0,0,
  1,1,0,0,
  0,1,1,0,
  0,0,0,0,
  
  0,1,0,0,
  1,1,0,0,
  1,0,0,0,
  0,0,0,0,
};

const int piece_O[] = {
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
  
  1,1,0,0,
  1,1,0,0,
  0,0,0,0,
  0,0,0,0,
};

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

// Pins for LCD (160x128)
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

  lcd.setRotation(1);
  lcd.fillScreen(BLACK);
  lcd.setTextColor(RED);
  lcd.setTextSize(2);
  lcd.print("lcd test\n");

  Serial.print("lcd test\n");

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

  pinMode(startButtonPin, INPUT_PULLUP);

  // Use analog input to generate random noise to choose a correct input
  randomSeed(analogRead(0));

  delay(500);
}

// Main Loop
void loop() {
  // Show start screen
  if (startScreen == false){
    lcd.fillScreen(BLACK);
    lcd.setCursor(0, 0);
    lcd.print("Press Start");
    startScreen = true;
  }  
  
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
    startScreen = false;
    
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

  // Reset selectedGame to 1
  selectedGame = 1;  
}





// Game One: Tetris
// Pieces: I, L, J, T, S, Z, O
void gameOne(){
  // Reset Starting variables
  for (int i = 0; i <= 21; i++){
    for (int j = 0; j <= 9; j++){
      if (i == 21){
        gameMatrix[j][i] = -1;
      }
      else{
        gameMatrix[j][i] = 0;
      }
    }
  }

  success = true;
  bool collision = false;
  bool leftCollision = false;
  bool rightCollision = false;
  pieceRotation = 1;
  //pieceRotation = random(4); // temporary testing
  
  // Draw game background
  lcd.fillScreen(BLACK);
  lcd.drawLine(54, 20, 54, 128, WHITE);
  lcd.drawLine(105, 20, 105, 128, WHITE);
  lcd.drawLine(54, 20, 105, 20, WHITE);
  lcd.drawLine(54, 127, 105, 127, WHITE);

  

  // Main game loop
  while (success == true){
    // Reset xPos, yPos, and collision
    xPos = 3;
    yPos = 0;
    collision = false;
    
    // Color index (1 - 7)
    nextPiece = random(1, 8);
    int pieceColorIndex = getNextPiece();

    // While there is no piece collisions
    while (collision == false) {
      readInputs();

      // If next square to the left is occupied, leftCollision = true
      leftCollision = false;
      for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
          if (gamePiece[i] == 1){
            if (gameMatrix[xPos+(i%4)-1][yPos+((i%16)/4)] != 0)
              leftCollision = true;              
          }
      }

      // If next square to the right is occupied, rightCollision = true
      rightCollision = false;
      for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
          if (gamePiece[i] == 1){
            if (gameMatrix[xPos+(i%4)+1][yPos+((i%16)/4)] != 0)
              rightCollision = true;              
          }
      }

      if (startButtonInput == true && leftCollision == false && xPos > (0-firstLeftColumn)){
        xPos--;
      }

      if (joystickRightInput == true && rightCollision == false && xPos < (9-firstRightColumn)){
        xPos++;
      }

      if (yPos == 0){
        drawGamePiece(55 + xPos*blockSize, 21+(yPos)*blockSize, getPieceColor());
      }
      else{   
        drawGamePiece(55 + xPos*blockSize, 21+(yPos)*blockSize, getPieceColor());
      }

      // If next square below is occupied, collision = true
      for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
          if (gamePiece[i] == 1){
            if (gameMatrix[xPos+(i%4)][yPos+((i%16)/4)+1] != 0)
              collision = true;              
          }
      }

      // Delay between piece moving
      delay(150);

      if (collision == false){
        drawGamePiece(55 + xPos*blockSize, 21+(yPos)*blockSize, BLACK);
        yPos++;
      }      
    }

    // Loop through game piece array and set gameMatrix values  
    for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
      if (gamePiece[i] != 0){
        gameMatrix[xPos+(i%4)][yPos+((i%16)/4)] = gamePiece[i]*pieceColorIndex;
      }
    }

    // Check if top of gameMatrix is occupied
    for (int i = 0; i <= 9; i++){
      if (gameMatrix[i][1] != 0){
        success = false;
        break;        
      }
    }

    printGameMatrix(); // temporary 

    
    //delay(500000);
  }
}

void drawGamePiece(int x1, int y1, int color){
  for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
    if (gamePiece[i] == 1){
      lcd.fillRect(x1+blockSize*(i%4), y1+blockSize*((i%16)/4), blockSize, blockSize, color);    
    }
  }
}

int getNextPiece(){
  int piece = 0;
  firstLeftColumn = -1;
  firstRightColumn = -1;
  
  switch (nextPiece){
    case 1:
      setGamePiece(piece_I);
      piece = 1;
      break;
    case 2:
      setGamePiece(piece_L);
      piece = 2;
      break;
    case 3:
      setGamePiece(piece_J);
      piece = 3;
      break;
    case 4:
      setGamePiece(piece_T);
      piece = 4;
      break;
    case 5:
      setGamePiece(piece_S);
      piece = 5;
      break;
    case 6:
      setGamePiece(piece_Z);
      piece = 6;
      break;
    case 7:
      setGamePiece(piece_O);
      piece = 7;
      break; 
  }

  // Set the first column from the left of a piece that is occupied
  for (int i = 0; i <= 2; i++){
    for (int j = 0; j <= 3; j++){
      if (gamePiece[j*4+i+16*pieceRotation] != 0){
        firstLeftColumn = i;
        break;
      }       
    }    
    if(firstLeftColumn != -1) { break; }  
  }

  // Set the first column from the right of a piece that is occupied
  for (int i = 3; i >= 1; i--){
    for (int j = 0; j <= 3; j++){
      if (gamePiece[j*4+i+16*pieceRotation] != 0){
        firstRightColumn = i;
        break;
      }       
    }    
    if(firstRightColumn != -1) { break; }  
  }

  return piece;
}

int getPieceColor(){
  switch (nextPiece){
    case 1:
      return CYAN;
    case 2:
      return ORANGE;
    case 3:
      return BLUE;
    case 4:
      return PURPLE;
    case 5:
      return GREEN;
    case 6:
      return RED;
    case 7:
      return YELLOW;
      break;      
  }  
}

void setGamePiece(int piece[64]){
  for (int i=0; i<=63; i++){
    gamePiece[i] = piece[i];
  }  
}

void printGameMatrix(){
  for (int i = 0; i <= 20; i++){
    Serial.print(i);
    Serial.print(": ");
    if (i < 10) { Serial.print(" "); }
    
    for (int j = 0; j <= 9; j++){
      int value = gameMatrix[j][i];
      Serial.print(value);
    }
    Serial.print("\n");
  }
  Serial.print("\n");
}

// Game Two:
void gameTwo(){

}

// Game Three:
void gameThree(){

}

// Reads all input pins into bool variables
// Inputs set to pullup are inverted (if LOW -> input = true)
void readInputs() {
  joystickLeftInput = (digitalRead(joystickLeftPin) == HIGH);
  joystickUpInput = (digitalRead(joystickUpPin) == HIGH);
  joystickRightInput = (digitalRead(joystickRightPin) == HIGH);
  joystickDownInput = (digitalRead(joystickDownPin) == HIGH);
  buttonOneInput = (digitalRead(buttonOnePin) == HIGH);
  buttonTwoInput = (digitalRead(buttonTwoPin) == HIGH);
  coinSlotButtonInput = (digitalRead(coinSlotButtonPin) == HIGH);
  startButtonInput = (digitalRead(startButtonPin) == LOW);
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