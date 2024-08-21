# 🕹️ **Cinesweeper** 🕹️

*Cinesweeper* is a custom Minesweeper game developed in C++ using the SFML library. It features classic gameplay with added functionalities like customizable settings, game-saving, and music integration.

## ✨ **Features** ✨

- 🎮 **Classic Minesweeper Gameplay**: Enjoy the traditional Minesweeper experience.
- ⏱️ **Custom Timer**: The game includes a specially designed timer for accurate time tracking.
- 🎛️ **Game Customization**: Players can adjust the grid size and the number of bombs.
- 💾 **Save and Resume**: The game auto-saves your progress so you can pick up where you left off.
- 🏆 **High Scores**: Keep track of your best completion times.
- 🎶 **Background Music**: Integrated music to enhance the gaming experience (volume control coming soon).

## 🛠️ **Getting Started** 🛠️

### **Prerequisites**

- **Code::Blocks 20.03**: Download with MinGW from [here](https://www.codeblocks.org/downloads/binaries/).
- **SFML Library**:
  - Download the development version from [SFML Snapshots](https://artifacts.sfml-dev.org/by-branch/master/windows-gcc-810-mingw-64.zip).
  - Extract the files to your preferred directory.

### **Installation**

1. **Configure SFML**:
   - In Code::Blocks, create a global variable named `sfml`, pointing to the extracted SFML directory.

2. **Building the Project**:
   - Open `Minesweeper.cbp` in Code::Blocks.
   - Set the build target to `Release`.
   - Compile the project using the build options.

3. **Running the Game**:
   - Launch the game directly from Code::Blocks after compiling.
   - Alternatively, run the compiled executable found in `./bin/Release/`. Ensure the `assets` folder and `openal32.dll` are in the same directory as the executable.

## 📂 **Project Structure** 📂

- **`assets/`**: Contains all game assets (images, music, etc.).
- **`icon.ico`, `icon.rc`**: Files for the application's icon.
- **`main.cpp`**: The main game source code.
- **`Minesweeper.cbp`**: Code::Blocks project file.
- **`openal32.dll`**: Audio support file.

## 🛠️ **Technical Details** 🛠️

- **Timer Implementation**: The game uses a custom timer built with C++ standard threading and synchronization mechanisms, ensuring precise interval management and reliable game timing.

- **Game Logic**: The `MainGame` struct handles all core gameplay, including user interactions, game state updates, and save/load operations.

## 📜 **License** 📜

Cinesweeper is licensed under the GNU General Public License v3.0 (GPL-3.0). For more information, see the [LICENSE](LICENSE) file.

## 🤝 **Contribution** 🤝

*Cinesweeper* is a personal project. Contributions are welcome, but there are no strict guidelines. Feel free to fork the repository and submit pull requests if you'd like to improve the game!