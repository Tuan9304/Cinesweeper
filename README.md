# Cinesweeper

Cinesweeper is a feature-rich Minesweeper game developed in C++ using the SFML library. It combines classic Minesweeper gameplay with modern enhancements including customizable game settings, automatic save functionality, and an integrated music player.

## Features

- **Classic Minesweeper Gameplay**: Traditional Minesweeper mechanics with left-click to reveal cells and right-click to flag mines
- **Custom Timer**: High-precision timer implementation for accurate time tracking using C++ threading
- **Game Customization**: Configure grid size and mine count to adjust difficulty levels
- **Auto-Save System**: Automatic progress saving allows resuming games after closing the application
- **High Score Tracking**: Records and displays your best completion times for each difficulty setting
- **Background Music**: Built-in audio player with background music (volume controls planned for future release)

## Getting Started

### Prerequisites

- **Code::Blocks 20.03**: Download with MinGW from [here](https://www.codeblocks.org/downloads/binaries/).
- **SFML Library**:
  - Download the development version from [SFML Snapshots](https://artifacts.sfml-dev.org/by-branch/master/windows-gcc-810-mingw-64.zip).
  - Extract the files to your preferred directory.

### Installation

1. **Configure SFML**:
   - In Code::Blocks, create a global variable named `sfml`, pointing to the extracted SFML directory.

2. **Building the Project**:
   - Open `Minesweeper.cbp` in Code::Blocks.
   - Set the build target to `Release`.
   - Compile the project using the build options.

3. **Running the Game**:
   - Launch the game directly from Code::Blocks after compiling.
   - Alternatively, run the compiled executable found in `./bin/Release/`. Ensure the `assets` folder and `openal32.dll` are in the same directory as the executable.

## Technical Details

### Architecture

- **Language**: C++ with object-oriented design principles
- **Graphics Library**: SFML (Simple and Fast Multimedia Library)
- **Audio**: OpenAL for sound playback

### Timer Implementation

The game features a custom-built timer using C++ standard library threading and synchronization mechanisms. This implementation ensures:
- Precise interval management with sub-millisecond accuracy
- Reliable game timing across different system configurations
- Thread-safe operations for concurrent game state updates

### Game Logic

The `MainGame` struct serves as the core game controller, managing:
- User input handling (mouse clicks, keyboard events)
- Game state management (playing, paused, won, lost)
- Cell revelation and mine flagging logic
- Save/load operations for game persistence
- High score recording and retrieval

## How to Play

1. **Starting a Game**: Launch the application and configure your desired grid size and mine count
2. **Revealing Cells**: Left-click on a cell to reveal it
3. **Flagging Mines**: Right-click on a cell to place or remove a flag where you suspect a mine
4. **Winning**: Successfully reveal all non-mine cells to win the game
5. **Game Over**: Clicking on a mine ends the game

The numbers displayed on revealed cells indicate how many mines are adjacent to that cell.

## License

Cinesweeper is licensed under the GNU General Public License v3.0 (GPL-3.0). For more information, see the [LICENSE](LICENSE) file.

## Contributing

Cinesweeper is a personal project, and contributions are welcome! While there are no strict guidelines, please:
- Fork the repository
- Create a feature branch for your changes
- Test your modifications thoroughly
- Submit a pull request with a clear description of your improvements

All contributions will be reviewed and considered for inclusion in the project.
