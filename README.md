# Connect 4 Game with SDL2

This is a graphical implementation of the classic Connect 4 game built using SDL2 and SDL_ttf for rendering text. The game allows for player vs. player and player vs. computer gameplay, complete with animations and an endgame screen that displays the winner.

## Table of Contents
1. [Features](#features)
2. [Requirements](#requirements)
3. [Installation](#installation)
4. [How to Run](#how-to-run)
5. [Game Rules](#game-rules)
6. [Code Overview](#code-overview)
7. [Contributing](#contributing)
8. [License](#license)

## Features
- **Player vs. Player Mode** and **Player vs. Computer Mode**
- **Computer AI** with basic strategic logic
- **Drop Animation** for pieces falling into place
- **Winning Highlight** for displaying the winning sequence
- **Endgame Screen** with Restart Prompt
- **Real-time Turn Display**

## Requirements
To build and run this game, you'll need:
- SDL2
- SDL2_ttf library for rendering text
- A C++ compiler (e.g., `g++`, `clang++`)
- (Optional) Fonts: A compatible TrueType font (e.g., Arial) is required, or configure your own in the code.

## Installation
### Linux
1. **Install SDL2 and SDL2_ttf:**
   ```bash
   sudo apt-get install libsdl2-dev libsdl2-ttf-dev
   ```

2. **Clone this repository:**
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

3. **Compile the Code:**
   ```bash
   g++ connect4.cpp -o connect4 -lSDL2 -lSDL2_ttf
   ```

### Windows
1. **Install SDL2 and SDL2_ttf:**
   - Download the development libraries for SDL2 and SDL2_ttf from [SDL2's official website](https://libsdl.org/).
   - Extract the libraries and configure them in your compiler's settings.

2. **Clone this repository:**
   ```bash
   git clone <repository-url>
   cd <repository-directory>
   ```

3. **Compile the Code:**
   - Use a compatible compiler and ensure the SDL2 and SDL2_ttf paths are linked.

## How to Run
Once compiled, execute the following command:
```bash
./connect4
```

## Game Rules
The objective is to connect four of your pieces in a row, either vertically, horizontally, or diagonally, before your opponent does.

1. **Player vs. Player Mode:** Both players take turns selecting a column to drop their piece.
2. **Player vs. Computer Mode:** The player takes their turn first, and the computer follows using basic AI to attempt to block or win.
3. **Winning Condition:** A player wins when they successfully connect four consecutive pieces in any direction.
4. **Tie:** If the board is full and no player has connected four pieces, the game is a draw.

## Code Overview
The main components and functions in the code are as follows:

### 1. **Game Structure (`Game`):**
   - Holds the game board, current player, game state, selected column, drop animation information, and winner.
   
### 2. **Main Functions:**
   - `initSDL`: Initializes SDL and creates a window and renderer.
   - `cleanupSDL`: Cleans up SDL resources.
   - `initGame`: Initializes the game board and sets the initial game state.
   - `drawBoard`: Draws the board, pieces, and animations on the screen.
   - `dropPiece`: Handles the logic for placing a piece in a column.
   - `checkVictory`: Checks for four consecutive pieces to determine a win.
   - `switchPlayer`: Alternates the player turns.
   - `computerMove`: Logic for the computer's move, including strategic checks for winning and blocking.
   - `showEndGameScreen`: Displays the winning message and highlights the winning sequence.
   - `displayTurn`: Displays the current player’s turn.

### 3. **Computer AI (`computerMove`):**
   - Contains basic strategic logic:
     - Attempts to win by connecting four pieces.
     - Blocks the player if they are about to win.
     - Prioritizes the center column for strategic control.

### 4. **Additional Features:**
   - **Animated Piece Drop**: Displays a falling effect as pieces are placed.
   - **Endgame and Restart Prompt**: Shows the winner and prompts to restart or exit.
   - **Dynamic Turn Display**: Displays the current player's turn at the bottom of the screen.

## Customization
- **Fonts**: By default, the code attempts to use Arial. Modify the font path for your system if needed.
- **Color Customization**: Update the `drawBoard` function to change colors for the pieces or board.
- **AI Improvements**: Enhance the `computerMove` function to make the computer AI more challenging.
