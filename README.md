# Cinesweeper

Cinesweeper is a feature-rich Minesweeper game developed in C++ using the SFML library. It combines classic Minesweeper gameplay with modern enhancements including customizable game settings, automatic save functionality, and an integrated music player.

## Key Features

- **Custom Timer Implementation**: High-precision timer using C++ standard library threading and synchronization
- **Auto-Save System**: File I/O operations for game state persistence
- **Dynamic Game Configuration**: Runtime-adjustable grid size and mine count
- **High Score Management**: Data structure implementation for tracking and storing player statistics
- **Audio Integration**: OpenAL implementation for background music playback

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

## License

Cinesweeper is licensed under the GNU General Public License v3.0 (GPL-3.0). For more information, see the [LICENSE](LICENSE) file.
