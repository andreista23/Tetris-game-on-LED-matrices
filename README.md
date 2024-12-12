# Tetris game on LED matrices

## Introduction

The project is a game similar to Tetris. For the display, two 8x8 LED matrices will be used, positioned vertically adjacent to each other. A piece will appear from the top of the "screen" and gradually descend. The player can use a controller to move the piece left or right and press a button to rotate the piece. The piece that appears at the top will be randomly selected from a list of possible pieces. The objective of the game is to arrange the pieces to complete lines. For each completed line, the score will increase, the respective line will disappear, and the lines above it will shift down by one position.

The purpose of the project is entertainment. Tetris is the most successful game ever released, with over 500 million copies sold. It is a simple yet engaging game that is very easy to learn and can be enjoyed by anyone, regardless of age.

## General description
The game is controlled by a joystick, which is connected to the Arduino board by a analog pin. Moving the joystick to the left or right will cause the piece to be moved to the respective direction, and pressing it will the cause the piece to be rotated. The LCD is connected to the Arduino board. Before the game starts, the LCD will show the rules of the game, the controls and the highest score. After the game has started, it will display the current score. The gameplay itself will be display on the two matrices. Randomly selected pieces will spawn in the top side of the screen and will slowly move down. The player will move the pieces in a way that will make them fit. The goal is to complete as many lines as possible. When a line is completed, it will disappear, causing the lines from above to move down with one position and the score will increase. If the placed pieces reach the top of the screen, the game ends.

## Hardware Design
 Compenents used:
 - Arduino Uno board with ATMega328 controller
 - joystick
 - 2 8x8 LED matrices, connected through a shield that allows to matrices to be controlled with 3 pins
 - LCD
 - Breadboard
 - Wires
 - Resistors
![proiect_robo](https://github.com/user-attachments/assets/05a782c5-34b4-4b0b-8fc2-ab796ee68ff7)

## Software Design

## Final results
