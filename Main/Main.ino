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
int startMenuRows = 2;
int startMenuColumns = 3;
int numGames = 6;
unsigned long backgroundColor = 0;
unsigned long score = 0;

// Game One: Tetris
int blockSize = 5;
int gamePiece[64] = {0};
int nextGamePiece[64] = {0};
int gameMatrix[10][22] = {0};

int nextPiece = 0;
int pieceRotation = 0;
int xPos = 0;
int yPos = 0;
int firstLeftColumn = 0;
int firstRightColumn = 0;
int gameDelay = 0;
int lines = 0;
int level = lines/10;
unsigned long rotationTime = 0;
unsigned long moveTime = 0;

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
int joystickLeftPin = 4;
int joystickUpPin = 5;
int joystickRightPin = 6;
int joystickDownPin = 7;

// Pins for two input buttons
int buttonOnePin = 8;
int buttonTwoPin = 9;

// Pins for coin slot button
int coinSlotButtonPin = 3;

// Pin for Start button
int startButtonPin = 12;

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

  lcd.setRotation(3);
  lcd.fillScreen(BLACK);
  backgroundColor = 0xD283;
  lcd.setTextColor(RED);
  lcd.setTextSize(1);

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
  pinMode(joystickLeftPin, INPUT_PULLUP);
  pinMode(joystickUpPin, INPUT_PULLUP);
  pinMode(joystickRightPin, INPUT_PULLUP);
  pinMode(joystickDownPin, INPUT_PULLUP);

  pinMode(buttonOnePin, INPUT_PULLUP);
  pinMode(buttonTwoPin, INPUT_PULLUP);

  pinMode(coinSlotButtonPin, INPUT_PULLUP);

  pinMode(startButtonPin, INPUT_PULLUP);

  // Use analog input to generate random noise to choose a correct input
  randomSeed(analogRead(0));
}

// Main Loop
void loop() {
  // Show start screen
  if (startScreen == false){
    selectedGame = 1;
    lcd.fillScreen(BLACK);
    lcd.setTextColor(0x9800);

    // Draw game selectors
    lcd.fillRect(0, 0, 160, 20, backgroundColor);
    lcd.setTextSize(2);
    lcd.setCursor(25, 2);
    lcd.print("Game Menu");
    lcd.setTextSize(1);

    drawStartMenuGame(10, 30, "Tetris");
    drawStartMenuGame(60, 30, "Game 2");
    drawStartMenuGame(110, 30, "Game 3");
    drawStartMenuGame(10, 80, "Game 4");
    drawStartMenuGame(60, 80, "Game 5");
    drawStartMenuGame(110, 80, "Game 6");

    startScreen = true;
    delay(100);
  }
  
  readInputs();

  // Change game selction 
  if(joystickLeftInput == true && selectedGame >= 2){
    lcd.drawRoundRect(10+50*(((selectedGame-1)%(startMenuColumns))), 30+50*((selectedGame/(startMenuColumns+1))), 40, 40, 7, WHITE);
    selectedGame--;           
  }
  else if(joystickUpInput == true && selectedGame > startMenuColumns){
    lcd.drawRoundRect(10+50*(((selectedGame-1)%(startMenuColumns))), 30+50*((selectedGame/(startMenuColumns+1))), 40, 40, 7, WHITE);
    selectedGame -= startMenuColumns;
  }
  else if(joystickRightInput == true && selectedGame < numGames){
    lcd.drawRoundRect(10+50*(((selectedGame-1)%(startMenuColumns))), 30+50*((selectedGame/(startMenuColumns+1))), 40, 40, 7, WHITE);
    selectedGame++;
  }
  else if(joystickDownInput == true && selectedGame <= numGames-startMenuColumns){
    lcd.drawRoundRect(10+50*(((selectedGame-1)%(startMenuColumns))), 30+50*((selectedGame/(startMenuColumns+1))), 40, 40, 7, WHITE);
    selectedGame += startMenuColumns;
  }

  // Draw cyan border around selected game
  lcd.drawRoundRect(10+50*(((selectedGame-1)%(startMenuColumns))), 30+50*((selectedGame/(startMenuColumns+1))), 40, 40, 7, CYAN);

  // Multiple delays to prevent missed start button input
  for (int i = 1; i <= 10; i++){
    readInputs();
    delay(15);

    if(startButtonInput == true){
      playSelectedGame();         
    }
  }

}





// Game One: Tetris
// Pieces: I, L, J, T, S, Z, O
void gameOne(){
  // Reset Starting variables
  success = true;
  score = 0;
  lines = 0;
  level = 0;

  int numRowsCleared = 0;
  int pieceColorIndex = 0;
  int nextPieceColorIndex = 0;
  bool collision = false;
  bool leftCollision = false;
  bool rightCollision = false;
  bool rowOccupied = false;
  bool leftRotated = false;
  bool rightRotated = false;
  pieceRotation = 0;
  
  // Game delay needs to be multiple of 25 ms
  int defaultGameDelay = 25*20;
  gameDelay = defaultGameDelay - (level*25);
  if (gameDelay < 50)
    gameDelay = 50; 

  // Reset gameMatrix
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
  
  // Draw game background
  lcd.setTextColor(RED);
  lcd.fillScreen(BLACK);
  lcd.drawRect(54, 20, 52, 108, WHITE); // main game area
  lcd.drawRect(2, 20, 44, 24, WHITE); // score
  lcd.drawRect(2, 48, 44, 48, WHITE); // level and lines
  lcd.drawRect(114, 20, 30, 30, WHITE); // next piece

  displayScore();

  // Set first piece
  nextPiece = random(1, 8);
  nextPieceColorIndex = getNextPiece();

  

  // Main game loop
  while (success == true){
    // Reset xPos, yPos, and collision
    xPos = 3;
    yPos = 0;
    pieceRotation = 0;
    collision = false;
    
    // Color index (1 - 7)
    setGamePiece(nextGamePiece);
    pieceColorIndex = nextPieceColorIndex;

    nextPiece = random(1, 8);
    nextPieceColorIndex = getNextPiece();
    drawNextPiece(nextPieceColorIndex);
    
    

    // While there is no piece collisions
    moveTime = millis();
    rotationTime == millis();
    while (collision == false) {
      for (int k = 0; k <= gameDelay/25; k++){
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
        
        if (joystickLeftInput == true && leftCollision == false && xPos > (0-firstLeftColumn) && (millis() - moveTime > 100)){
          xPos--;
          moveTime = millis();
        }

        if (joystickRightInput == true && rightCollision == false && xPos < (9-firstRightColumn) && (millis() - moveTime > 100)){
          xPos++;
          moveTime = millis();
        }     

        if (buttonOneInput == false){
          rightRotated = false;
        } 

        if (buttonTwoInput == false){
          leftRotated = false;
        }
        
        if (buttonOneInput == true && rightRotated == false){
          rightRotated = true;
          pieceRotation = ++pieceRotation%4;

          getFirstColumns();

          if (xPos == -2 && firstLeftColumn == 0){
            xPos++;
          }
          if (xPos == -1 && firstLeftColumn == 0){
            xPos++;
          }

          if (xPos == 9 && (firstRightColumn == 3 || firstRightColumn == 2)){
            xPos--;
          }
          if (xPos == 8 && (firstRightColumn == 3 || firstRightColumn == 2)){
            xPos--;
          }

          if (xPos == 7 && firstRightColumn == 3){
            xPos--;
          }     

          rotationTime = millis();                         
        }

        if (buttonTwoInput == true && leftRotated == false){
          leftRotated = true;

          if (pieceRotation == 0){
            pieceRotation = 3;
          }
          else{
            pieceRotation -= 1;
          }

          getFirstColumns();

          if (xPos == -2 && firstLeftColumn == 0){
            xPos++;
          }
          if (xPos == -1 && firstLeftColumn == 0){
            xPos++;
          }

          if (xPos == 9 && (firstRightColumn == 3 || firstRightColumn == 2)){
            xPos--;
          }
          if (xPos == 8 && (firstRightColumn == 3 || firstRightColumn == 2)){
            xPos--;
          }

          if (xPos == 7 && firstRightColumn == 3){
            xPos--;
          }     

          rotationTime = millis();                         
        }
        


        // Draw pieces
        if (yPos == 0){
          drawGamePiece(55 + xPos*blockSize, 22+(yPos)*blockSize, getPieceColor(pieceColorIndex), gamePiece);
        }
        else{   
          drawGamePiece(55 + xPos*blockSize, 22+(yPos)*blockSize, getPieceColor(pieceColorIndex), gamePiece);
        }

        // Delay before piece moves
        if (joystickDownInput){
          gameDelay = 25;
        }
        else{
          gameDelay = defaultGameDelay - (level*25);
            if (gameDelay < 50)
            gameDelay = 50; 
        }

        delay(25);

        if (k!=gameDelay/25){
          drawGamePiece(55 + xPos*blockSize, 22+(yPos)*blockSize, BLACK, gamePiece);
        }                         
      }

      // If next square below is occupied, collision = true
      for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
          if (gamePiece[i] == 1){
            if (gameMatrix[xPos+(i%4)][yPos+((i%16)/4)+1] != 0)
              collision = true;              
          }
      }

      // If no collision, erase piece from display and increase yPos
      if (collision == false){
        drawGamePiece(55 + xPos*blockSize, 22+(yPos)*blockSize, BLACK, gamePiece);
        yPos++;
      }      
    }

    // Loop through game piece array and set gameMatrix values  
    for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
      if (gamePiece[i] != 0){
        gameMatrix[xPos+(i%4)][yPos+((i%16)/4)] = gamePiece[i]*pieceColorIndex;
      }
    }

    // Check if an entire row is occupied
    for (int i = 1; i <= 20; i++){
      rowOccupied = true;

      for (int j = 0; j <= 9; j++){
        if (gameMatrix[j][i] == 0){
          rowOccupied = false;
          break;      
        }
      }

      if (rowOccupied == true){
        for (int m = i; m >= 1; m--){
          for (int n = 0; n <= 9; n++){
            gameMatrix[n][m] = gameMatrix[n][m-1];           
          }
        }

        drawGameMatrix();
        numRowsCleared++;
        lines++;
        level = lines/10;
      }    
    }

    // Increase score based on level and lines cleared
    switch (numRowsCleared){
      case 0:
        break;
      case 1:
        score += 40*(level+1);
        break;
      case 2:
        score += 100*(level+1);
        break;
      case 3:
        score += 300*(level+1);
        break;
      case 4:
        score += 1200*(level+1);
        break; 
    }
    numRowsCleared = 0;

    // Display Score
    displayScore();

    // Check if top of gameMatrix is occupied
    for (int i = 0; i <= 9; i++){
      if (gameMatrix[i][1] != 0){
        success = false;
        delay(1000);
        break;        
      }
    }

    //printGameMatrix(); // temporary 
  }
}

void drawGameMatrix(){
  lcd.fillRect(55, 21, 104-55, 126-21, BLACK);
  
  for (int i = 1; i <= 20; i++){
    for (int j = 0; j <= 9; j++){
      // if zero, don't create white outline
      if (gameMatrix[j][i] == 0){
        lcd.fillRect(55 + j*blockSize, 22 + i*blockSize, blockSize, blockSize, getPieceColor(gameMatrix[j][i]));
      }
      else{
        lcd.drawRect(55 + j*blockSize, 22 + i*blockSize, blockSize, blockSize, WHITE);
        lcd.fillRect(55 + j*blockSize+1, 22 + i*blockSize+1, blockSize-1, blockSize-1, getPieceColor(gameMatrix[j][i]));
      }
    } 
  }
}

void displayScore(){
  lcd.setTextSize(1);

  lcd.fillRect(3, 21, 42, 22, BLACK);
  lcd.setCursor(5, 23);
  lcd.print("Score:\n");
  lcd.setCursor(5, lcd.getCursorY());
  lcd.print(score);

  lcd.fillRect(3, 49, 42, 46, BLACK);
  lcd.setCursor(5, 51);
  lcd.print("Level:");
  lcd.print("\n");
  lcd.setCursor(5, lcd.getCursorY());
  lcd.print(level);
  lcd.print("\n");
  lcd.setCursor(5, lcd.getCursorY());
  lcd.print("Lines:");
  lcd.print("\n");
  lcd.setCursor(5, lcd.getCursorY());
  lcd.print(lines);

  lcd.setTextSize(2);
}

void drawNextPiece(int nextPieceColorIndex){
  lcd.setTextSize(1);

  lcd.fillRect(115, 21, 28, 28, BLACK);
  lcd.setCursor(117, 23);
  lcd.print("Next");
  drawGamePiece(119, 32, getPieceColor(nextPieceColorIndex), nextGamePiece);

  lcd.setTextSize(2);
}


void drawGamePiece(int x1, int y1, int color, int piece[64]){
  for (int i = 0+pieceRotation*16; i <= 15+pieceRotation*16; i++){
    if (piece[i] == 1){
      if (color == BLACK){
        lcd.fillRect(x1+blockSize*(i%4), y1+blockSize*((i%16)/4), blockSize, blockSize, BLACK);    
      }
      else{
        lcd.drawRect(x1+blockSize*(i%4), y1+blockSize*((i%16)/4), blockSize, blockSize, WHITE);
        lcd.fillRect(x1+blockSize*(i%4)+1, y1+blockSize*((i%16)/4)+1, blockSize-1, blockSize-1, color);    
      }
    }
  }
}

int getNextPiece(){  
  switch (nextPiece){
    case 1:
      setNextGamePiece(piece_I);
      break;
    case 2:
      setNextGamePiece(piece_L);
      break;
    case 3:
      setNextGamePiece(piece_J);
      break;
    case 4:
      setNextGamePiece(piece_T);
      break;
    case 5:
      setNextGamePiece(piece_S);
      break;
    case 6:
      setNextGamePiece(piece_Z);
      break;
    case 7:
      setNextGamePiece(piece_O);
      break; 
  }

  getFirstColumns();

  return nextPiece;
}

void getFirstColumns(){
  firstLeftColumn = -1;
  firstRightColumn = -1;

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
}

int getPieceColor(int nxtPiece){
  switch (nxtPiece){
    case 0:
      return BLACK;
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

void setNextGamePiece(int piece[64]){
  for (int i=0; i<=63; i++){
    nextGamePiece[i] = piece[i];
  }  
}

void printGameMatrix(){
  for (int i = 0; i <= 21; i++){
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

void playSelectedGame(){
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
      case 4:
        gameFour();
        break;
      case 5:
        gameFive();
        break;
      case 6:
        gameSix();
        break;
    }

    // Reset selectedGame to 1
    selectedGame = 1;
}

// Game Two:
void gameTwo(){
  lcd.fillScreen(BLACK);
  lcd.setCursor(10, lcd.height()/2);
  lcd.print("Game 2 not implemented");
  delay(5000);
}

// Game Three:
void gameThree(){
  lcd.fillScreen(BLACK);
  lcd.setCursor(10, lcd.height()/2);
  lcd.print("Game 3 not implemented");
  delay(5000);
}
// Game Four:
void gameFour(){
  lcd.fillScreen(BLACK);
  lcd.setCursor(10, lcd.height()/2);
  lcd.print("Game 4 not implemented");
  delay(5000);
}

// Game Five:
void gameFive(){
  lcd.fillScreen(BLACK);
  lcd.setCursor(10, lcd.height()/2);
  lcd.print("Game 5 not implemented");
  delay(5000);
}

// Game Six:
void gameSix(){
  lcd.fillScreen(BLACK);
  lcd.setCursor(10, lcd.height()/2);
  lcd.print("Game 6 not implemented");
  delay(5000);
}

// Draws games in start menu
void drawStartMenuGame(int x, int y, char text[]){
    lcd.drawRoundRect(x, y, 40, 40, 7, WHITE);
    lcd.setCursor(x+2, y+15);
    lcd.print(text);
}

// Reads all input pins into bool variables
// Inputs set to pullup are inverted (if LOW -> input = true)
void readInputs() {
  joystickLeftInput = (digitalRead(joystickLeftPin) == LOW);
  joystickUpInput = (digitalRead(joystickUpPin) == LOW);
  joystickRightInput = (digitalRead(joystickRightPin) == LOW);
  joystickDownInput = (digitalRead(joystickDownPin) == LOW);
  buttonOneInput = (digitalRead(buttonOnePin) == LOW);
  buttonTwoInput = (digitalRead(buttonTwoPin) == LOW);
  coinSlotButtonInput = (digitalRead(coinSlotButtonPin) == LOW);
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