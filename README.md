# Tetris game on LED matrices

## Introduction

The project is a game similar to ***Tetris***. 

<img src="https://github.com/user-attachments/assets/87cda5a4-3e2a-4b0e-9147-e3be9092892f" alt="Image 1" width="500"/>

For the display, two **8x8 LED matrices** will be used, positioned vertically adjacent to each other. 

A piece will appear from the top of the "screen" and gradually descend. The player can use a controller to move the piece left or right and press a button to rotate the piece. The piece that appears at the top will be randomly selected from a list of possible pieces. 

The objective of the game is to arrange the pieces to complete lines. For each completed line, the score will increase, the respective line will disappear, and the lines above it will shift down by one position.

The purpose of the project is entertainment. Tetris is the most successful game ever released, with over 500 million copies sold. It is a simple yet engaging game that is very easy to learn and can be enjoyed by anyone, regardless of age.

## General description
The game is controlled by a joystick, which is connected to the Arduino board by a analog pin. Moving the joystick to the left or right will cause the piece to be moved to the respective direction, and pressing it will the cause the piece to be rotated. 

The LCD is connected to the Arduino board. Before the game starts, the LCD will show the rules of the game, the controls and the highest score. After the game has started, it will display the current score. The gameplay itself will be display on the two matrices. 

Randomly selected pieces will spawn in the top side of the screen and will slowly move down. The player will move the pieces in a way that will make them fit. 

The goal is to complete as many lines as possible. When a line is completed, it will disappear, causing the lines from above to move down with one position and the score will increase. If the placed pieces reach the top of the screen, the game ends.

## Hardware Design
 Components used:
 - Arduino Uno board with ATMega328 controller
 - joystick
 - 4 8x8 LED matrices MAX7219, connected through a shield that allows to matrices to be controlled with 3 pins https://cleste.ro/modul-4-x-matrice-led-max7219-8x8.html
 - LCD
 - Breadboard
 - Wires
 - Resistors

   ### Wiring

   ---
   **LCD**
- LCD RS pin to digital pin 7
- LCD Enable pin to digital pin 6
- LCD D4 pin to digital pin 5
- LCD D5 pin to digital pin 4
- LCD D6 pin to digital pin 3
- LCD D7 pin to digital pin 2
- LCD R/W pin to GND
- LCD VSS pin to GND
- LCD VCC pin to 5V
- LCD LED+ to 5V through a 220 ohm resistor
- LCD LED- to GND

---
   **LED matrices**
- DIN pin to digital pin 11
- CLK pin to digital pin 13
- CS pin to digital pin 9
- VCC pin to 5V
- GND pin to GND

---
   **Joystick**
- VRX pin to analog pin A1
- VRY pin to analog pin A2
- SW pin to analog pin A0
- VCC pin to 5V
- GND pin to GND

Electrical diagram
![proiect_robo](https://github.com/user-attachments/assets/05a782c5-34b4-4b0b-8fc2-ab796ee68ff7)
Photo of the hardware
![WhatsApp Image 2024-12-17 at 10 55 50 AM](https://github.com/user-attachments/assets/ca034c7b-ca3a-4e60-8c38-cafaced2c091)

## Software Design

The software implements Tetris gameplay, menu navigation, and player interaction using Arduino. Below are the key components:

### Initialization
- **LCD and LED Matrices**:
  - The LCD (16x2) displays the menu, current score, and high scores.
  - The LED matrices (4x 8x8) are used to display the Tetris game grid.
- **Joystick**:
  - The joystick controls movement and rotation of the tetrominoes and navigates the menu.
  - Button presses are handled using an interrupt for quick response.

### Menu and Navigation
- **Menu Options**:
  - **Start Game**: Begins the Tetris game.
  - **Highscore**: Displays the highest score and player name.
  - **Enter Name**: Allows entering a name via the serial monitor.
  - **Difficulty**: Adjusts game difficulty (Easy, Medium, Hard).
- The joystick navigates the menu, and button presses select options.

### Gameplay
- **Tetromino Movements**:
  - Pieces can move left, right, or down and rotate clockwise.
  - Pieces are locked when they reach the bottom or collide with other pieces.
- **Line Clearing**:
  - Completed rows are cleared, and rows above shift down. The score increases accordingly.
- **Game Over**:
  - The game ends if new pieces cannot spawn due to filled rows.

### High Score Tracking
- The game tracks the highest score and associates it with the player's name entered via the serial monitor.
- Updates the high score whenever a player's score exceeds the current record.

### Difficulty Levels
- Three levels (Easy, Medium, Hard) control the speed of falling pieces (`fallDelay`).

### Key Functions
- **`setup()`**: Initializes the hardware and displays a welcome message.
- **`loop()`**: Handles menu navigation or gameplay based on the current state.
- **`generateNewTetromino()`**: Spawns a random tetromino at the top of the grid.
- **`checkCollision()`**: Ensures valid movement and placement of tetrominoes.
- **`lockTetromino()`**: Locks tetrominoes into the grid and triggers line clearing.
- **`clearLines()`**: Clears filled rows and updates the score.
- **`updateMenu()`**: Updates the LCD with the current menu option.
- **`showHighscore()`**: Displays the highest score and associated player name.
- **`enterName()`**: Accepts the player's name via the serial monitor.
- **`changeDifficulty()`**: Adjusts the game speed based on selected difficulty.

This modular design ensures efficient gameplay mechanics and easy adaptation for different hardware configurations.
## Final results
https://youtube.com/watch/VkTXeH6DotI?feature=share
