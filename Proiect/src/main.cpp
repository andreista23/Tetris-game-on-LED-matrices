#include <LiquidCrystal.h>
#include <Arduino.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Define size and output pins
#define MAX_DEVICES 4
#define DIN 11
#define CLK 13
#define CS_PIN 9
#define JOYSTICKX A1
#define JOYSTICKY A2
#define JOYSTICKB 2 // Use pin 2 for interrupt

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Initialize the LCD interface pins
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Function headers
void updateMenu();
void selectMenuItem();
void showHighscore();
void enterName();
void changeDifficulty();
void delayCustom(unsigned long duration);
void joystickButtonISR();
// Generate a new random tetromino
void generateNewTetromino();

// Check if moving the tetromino to a new position would result in a collision
bool checkCollision(int newX, int newY);

// Lock the current tetromino into the grid and handle line clearing
void lockTetromino();

// Move the current tetromino down by one unit
void moveDown();

// Move the current tetromino to the left by one unit
void moveLeft();

// Move the current tetromino to the right by one unit
void moveRight();

// Rotate the current tetromino clockwise
void rotateTetromino();

// Clear any full lines in the grid and shift remaining rows down
void clearLines();

// Draw the game grid and current tetromino on the LED matrix
void drawGame();

// Display the "Game Over" message and reset the game
void displayGameOver();

// Handle user input for movement and rotation of the tetromino
void handleUserInput();

// Reset the game state
void resetGame();

// Variables for joystick and menu
int valX = 0;
int valY = 0;
volatile bool buttonPressed = false; // Flag for button press
int menuIndex = 0;
const char* menuItems[] = {"Start", "Highscore", "Enter Name", "Difficulty"};
int numMenuItems = sizeof(menuItems) / sizeof(menuItems[0]);
const char* difficulties[] = {"Easy", "Medium", "Hard"};
int difficultyIndex = 0;


// Game variables
bool gameStarted = false;
const int gridWidth = 16;
const int gridHeight = 8;
bool grid[gridHeight][gridWidth] = {false};
const int tetrominoSize = 4;
int score = 0;

// Tetromino shapes (I, J, L, O, S, T, Z)
const int tetrominoes[7][tetrominoSize][tetrominoSize] = {
  {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}}, // I
  {{1, 0, 0}, {1, 1, 1}, {0, 0, 0}}, // J
  {{0, 0, 1}, {1, 1, 1}, {0, 0, 0}}, // L
  {{1, 1}, {1, 1}},                 // O
  {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}}, // S
  {{0, 1, 0}, {1, 1, 1}, {0, 0, 0}}, // T
  {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}}  // Z
};

// Current tetromino
int currentTetromino[4][4];
int currentX = 14; // Starting position (centered horizontally)
int currentY = 0;
int rotation = 0;
bool gameOver = false;

// Timer variables
unsigned long lastFallTime = 0;
unsigned long fallDelay = 500; // Initial fall delay (in milliseconds)


unsigned long lastActionTime = 0;
const unsigned long debounceDelay = 400; // 200 ms for joystick debounce
unsigned long welcomeStartTime = 0;
const unsigned long welcomeDuration = 3000; // Show welcome for 3 seconds
volatile unsigned long lastInterruptTime = 0;
unsigned long selectedMenuTime = 0;
// Highscore variables
char playerName[16] = "Player"; // Default player name
char highScoreName[16] = "None"; // Name of the high score holder
int highScore = 0;

bool showingWelcome = true;

void setup() {
  // Initialize the LCD
  lcd.begin(16, 2);
  
  // Show the welcome message
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Tetris Game!");

  // Record the welcome message start time
  welcomeStartTime = millis();
  // Initialize the display
  myDisplay.begin();
  myDisplay.setIntensity(0);
  myDisplay.displayClear();
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 0); // Set brightness to a low level
  mx.clear(); // Clear the display

  // Initialize serial communication
  Serial.begin(28800);

  // Configure joystick inputs
  pinMode(JOYSTICKX, INPUT_PULLUP);
  pinMode(JOYSTICKY, INPUT_PULLUP);
  pinMode(JOYSTICKB, INPUT_PULLUP);

  // Attach interrupt for joystick button
  attachInterrupt(digitalPinToInterrupt(JOYSTICKB), joystickButtonISR, FALLING);
}

void loop() {
  unsigned long currentTime = millis();

  // Check if welcome message duration has elapsed
  if(gameStarted){
    // Game loop
    if (gameOver) {
    displayGameOver();
    return;
  }
    lcd.setCursor(0, 1);
    lcd.print("Score: ");
    // Draw the grid and current tetromino
    drawGame();
    if (millis() - lastFallTime >= fallDelay) {
      moveDown();
      lastFallTime = millis();
    }

    // Handle user input for movement and rotation
    handleUserInput();

      }
  else{
    if (showingWelcome && (currentTime - welcomeStartTime >= welcomeDuration)) {
      showingWelcome = false;
      lcd.clear();
      lcd.print(menuItems[menuIndex]);
    }

    // If welcome message is still showing, do nothing else
    if (showingWelcome) {
      return;
    }

    // Read joystick values
    valX = analogRead(JOYSTICKX);
    valY = analogRead(JOYSTICKY);

    // Joystick navigation (up and down)
    if (currentTime - lastActionTime > debounceDelay) {
      if (buttonPressed) {
        buttonPressed = false;
        selectedMenuTime = millis(); 
        selectMenuItem();
        lastActionTime = currentTime;
      }
      else{
        if (valY < 200) { // Joystick pushed up
          menuIndex = (menuIndex - 1 + numMenuItems) % numMenuItems;
          updateMenu();
          lastActionTime = currentTime;
        } else if (valY > 800) { // Joystick pushed down
          menuIndex = (menuIndex + 1) % numMenuItems;
          updateMenu();
          lastActionTime = currentTime;
        }
      }
  }
  }
}

void updateMenu() {
  lcd.clear();
  lcd.print(menuItems[menuIndex]);
  if (menuIndex == 3) { // Show difficulty option for "Difficulty"
    lcd.setCursor(0, 1);
    lcd.print(difficulties[difficultyIndex]);
  }
}

void selectMenuItem() {
  switch (menuIndex) {
    case 0: // Start Game
      gameStarted = true;
      resetGame(); // Reset the game state
      break;
    case 1: // Highscore
        showHighscore();
      break;
    case 2: // Enter Name
      enterName();
      break;
    case 3: // Difficulty
      changeDifficulty();
      break;
  }
}

void showHighscore() {
  lcd.clear();
  lcd.print("High: ");
  lcd.print(highScore);
  lcd.setCursor(0, 1);
  lcd.print(highScoreName);
}

void enterName() {
  lcd.clear();
  lcd.print("Enter Name:");

  // Read name via Serial
  Serial.println("Enter your name:");
  int index = 0;
  while (index < 15) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') break; // Stop on newline
      playerName[index++] = c;
      playerName[index] = '\0';
      lcd.setCursor(0, 1);
      lcd.print(playerName);
    }
  }

  //delayCustom(2000); // Custom delay using millis
  updateMenu();
}

void changeDifficulty() {
  difficultyIndex = (difficultyIndex + 1) % 3; // Cycle through difficulties
  if (difficultyIndex == 0)
    fallDelay = 700; // Easy
  else if (difficultyIndex == 1)
    fallDelay = 500; // Medium
  else
    fallDelay = 300; // Hard
  updateMenu();
}

void delayCustom(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    // Wait without blocking other operations
  }
}

void joystickButtonISR() {
  unsigned long interruptTime = millis(); // Get the current time
  
  // If interrupts come faster than debounce delay, ignore them
  if (interruptTime - lastInterruptTime > debounceDelay) {
    buttonPressed = true; // Set the flag for button press
    lastInterruptTime = interruptTime; // Update the last interrupt time
  }
}

void generateNewTetromino() {
  int tetrominoIndex = random(0, 7); // Randomly select a tetromino
  memset(currentTetromino, 0, sizeof(currentTetromino));

  // Copy the selected tetromino shape
  for (int i = 0; i < tetrominoSize; i++) {
    for (int j = 0; j < tetrominoSize; j++) {
      currentTetromino[i][j] = tetrominoes[tetrominoIndex][i][j];
    }
  }

  // Set the initial position
  currentX = gridWidth - tetrominoSize; // Topmost position (highest X value)
  currentY = gridHeight / 2 - tetrominoSize / 2; // Center horizontally (middle of height)

  // Check for collision at the starting position
  if (checkCollision(currentX, currentY)) {
    gameOver = true; // End the game if a collision is detected
  }
}


bool checkCollision(int newX, int newY) {
  for (int i = 0; i < tetrominoSize; i++) {
    for (int j = 0; j < tetrominoSize; j++) {
      if (currentTetromino[i][j]) {
        int x = newX + j;
        int y = newY + i;
        if (x < 0 || x >= gridWidth || y >= gridHeight || y < 0 || (y >= 0 && grid[y][x])) {
          return true;
        }
      }
    }
  }
  return false;
}

void lockTetromino() {
  for (int i = 0; i < tetrominoSize; i++) {
    for (int j = 0; j < tetrominoSize; j++) {
      if (currentTetromino[i][j]) {
        int x = currentX + j;
        int y = currentY + i;
        if (y >= 0) {
          grid[y][x] = true;
        }
      }
    }
  }

  if (score > highScore) {
    highScore = score;
    strncpy(highScoreName, playerName, sizeof(highScoreName));
  }

  clearLines();
  generateNewTetromino();
  if (checkCollision(currentX, currentY)) {
    gameOver = true;
  }
}

void moveDown() {
  if (!checkCollision(currentX-1, currentY)) {
    currentX--;
  } else {
    lockTetromino();
  }
}

void moveLeft() {
  if (!checkCollision(currentX, currentY+1)) {
    currentY++;
  }
}

void moveRight() {
  if (!checkCollision(currentX, currentY-1)) {
    currentY--;
  }
}

void rotateTetromino() {
  int rotatedTetromino[4][4];
  memset(rotatedTetromino, 0, sizeof(rotatedTetromino));
  for (int i = 0; i < tetrominoSize; i++) {
    for (int j = 0; j < tetrominoSize; j++) {
      rotatedTetromino[j][tetrominoSize - i - 1] = currentTetromino[i][j];
    }
  }
  if (!checkCollision(currentX, currentY)) {
    memcpy(currentTetromino, rotatedTetromino, sizeof(rotatedTetromino));
  }
}

void clearLines() {
  for (int x = 0; x < gridWidth; x++) {
    bool fullLine = true;

    // Check if the current column is full
    for (int y = 0; y < gridHeight; y++) {
      if (!grid[y][x]) {
        fullLine = false;
        break;
      }
    }

    // If the column is full, clear it and shift columns to the left
    if (fullLine) {
      // Increment the score for clearing a line
      score += 100;
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(score);

      // Shift columns to the left from the current column onwards
      for (int col = x; col < gridWidth - 1; col++) {
        for (int y = 0; y < gridHeight; y++) {
          grid[y][col] = grid[y][col + 1];
        }
      }

      // Clear the rightmost column (last column)
      for (int y = 0; y < gridHeight; y++) {
        grid[y][gridWidth - 1] = false;
      }

      // Since the current column has been cleared, re-check the same column index
      // (because all columns to the right have shifted left)
      x--;
    }
  }
}


void drawGame() {
  mx.clear(); // Clear the display

  // Draw grid
  for (int y = 0; y < gridHeight; y++) {
    for (int x = 0; x < gridWidth; x++) {
      if (grid[y][x]) {
        int device = x / 8;             // Determine which device to use
        int localX = x % 8;             // Determine the column within the device
        mx.setPoint(y, x + 8, true); // Set the LED on
      }
    }
  }

  // Draw current tetromino
  for (int i = 0; i < tetrominoSize; i++) {
    for (int j = 0; j < tetrominoSize; j++) {
      if (currentTetromino[i][j]) {
        int x = currentX + j;
        int y = currentY + i;
        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight) {
          int device = x / 8;               // Determine which device to use
          int localX = x % 8;               // Determine the column within the device
          mx.setPoint(y, x+8, true); // Set the LED on
        }
      }
    }
  }

  mx.update(); // Ensure the changes are applied
}

void displayGameOver() {
  myDisplay.displayClear();
  lcd.clear();
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print(" Score: ");
  lcd.print(score);

  delay(3000); // Pause to show the message

  // Reset the game state
  gameOver = false;
  gameStarted = false;
  lcd.clear();
  lcd.print("Select Option:");
  lcd.setCursor(0, 1);
  lcd.print(menuItems[menuIndex]); // Show the menu
}

void handleUserInput() {
  int joystickX = analogRead(JOYSTICKX);
  int joystickY = analogRead(JOYSTICKY);

  // Check horizontal movement (left or right)
  if (joystickX < 200) { // Joystick pushed left
    moveLeft();
    delay(100); // Small delay to prevent rapid movement
  } else if (joystickX > 800) { // Joystick pushed right
    moveRight();
    delay(100); // Small delay to prevent rapid movement
  }

  // Check vertical movement (down for faster drop)
  if (joystickY < 200) { // Joystick pushed down
    moveDown();
    delay(100); // Small delay to prevent rapid movement
  }

  // Rotation is handled by the button press interrupt
  if (buttonPressed) {
    buttonPressed = false;
    rotateTetromino();
  }
}

void resetGame() {
  memset(grid, 0, sizeof(grid)); // Clear the grid
  gameOver = false; // Reset the game over flag
  generateNewTetromino(); // Spawn the first tetromino
  lcd.clear();
}