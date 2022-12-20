#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>
#include <fstream>

// temp
#include <string>
#include <iostream>

using ii = std::pair<int, int>;
using vi = std::vector<int>;
#define all(x) begin(x), end(x)
#define pb push_back
#define F first
#define S second

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

const short td[] = {-1, -1, -1, 0, 1, 1, 1, 0},
            tc[] = {-1, 0, 1, 1, 1, 0, -1, -1};

const short tx[] = {0, 1, 2, 0, 0, 1, 2, 2, 1},
            ty[] = {0, 0, 0, 1, 2, 2, 2, 1, 1};

const short mx[] = {32, 32 * 5, 32, 32 * 5},
            my[] = {32, 32, 32 * 5, 32 * 5};

struct Level {
  int height, width, bombs;
  std::string name;
  Level(std::string n) : name(n) {}
  Level(int h, int w, int b, std::string n)
      : height(h), width(w), bombs(b), name(n) {}
};

const Level defaultLevel[] = {Level(9, 9, 10, "Easy"),
                              Level(16, 16, 40, "Medium"),
                              Level(16, 30, 99, "Hard"),
                              Level("Custom")};

struct MainGame {
  int height, width, bombs, numsCell, cellsRemain;
  vi bombCells, cells, display;
  bool isClicked, isStop;
  short gameStatus;
  sf::Clock clock;
  double lastTime;
  void viWrite(std::ofstream& wf, const vi& v) {
    int size = v.size();
    wf.write((char*)&size, sizeof(int));
    wf.write((char*)v.data(), v.size() * sizeof(int));
  }
  void viRead(std::ifstream& rf, vi& v) {
    int size;
    rf.read((char*)&size, sizeof(int));
    while (size--) {
      int i;
      rf.read((char*)&i, sizeof(int));
      v.push_back(i);
    }
  }
  void save() {
    std::ofstream wf("game.dat", std::ios::out | std::ios::binary);
    if (wf.is_open()) {
      wf.write((char*)&height, sizeof(int));
      wf.write((char*)&width, sizeof(int));
      wf.write((char*)&bombs, sizeof(int));
      wf.write((char*)&cellsRemain, sizeof(int));
      viWrite(wf, bombCells);
      viWrite(wf, cells);
      viWrite(wf, display);
      wf.write((char*)&isClicked, sizeof(bool));
      wf.write((char*)&isStop, sizeof(bool));
      wf.write((char*)&gameStatus, sizeof(short));
      wf.write((char*)&lastTime, sizeof(double));
      wf.close();
    } else {
      std::cerr << "Unable to open data file\n";
    }
  }
  void load() {
    std::ifstream rf("game.dat", std::ios::in | std::ios::binary);
    if (rf.is_open()) {
      rf.read((char*)&height, sizeof(int));
      rf.read((char*)&width, sizeof(int));
      rf.read((char*)&bombs, sizeof(int));
      rf.read((char*)&cellsRemain, sizeof(int));
      viRead(rf, bombCells);
      viRead(rf, cells);
      viRead(rf, display);
      rf.read((char*)&isClicked, sizeof(bool));
      rf.read((char*)&isStop, sizeof(bool));
      rf.read((char*)&gameStatus, sizeof(short));
      rf.read((char*)&lastTime, sizeof(double));
      rf.close();

      numsCell == height* width;
    } else {
      std::cerr << "Unable to open data file\n";
      resizeGrid(defaultLevel[0]);
    }
  }
  void newgame() {
    gameStatus = 0;
    isClicked = 0;
    isStop = 1;
    cells.assign(height * width, 0);
    display.assign(height * width, 10);
    cellsRemain = numsCell;
    bombCells.clear();
  }
  double displayTime() {
    if (!isClicked) {
      return 0;
    }
    if (!isStop) {
      lastTime = clock.getElapsedTime().asSeconds();
    }
    return lastTime;
  }
  int toCell(int x, int y) { return x * width + y; }
  bool checkInside(int ei, int ej) {
    return ei >= 0 && ej >= 0 && ei < height && ej < width;
  }
  void resizeGrid(Level level) {
    height = level.height;
    width = level.width;
    numsCell = height * width;
    bombs = std::min(numsCell - 1, level.bombs);
    newgame();
  }
  void flagCell(int idx) {
    // if cell is opened or game is stopped, then it can't be flagged
    if (isStop || display[idx] == cells[idx]) {
      return;
    }
    display[idx] ^= 1;
  }
  void openCell(int x, int y) {
    int idx = toCell(x, y);
    // if cell is flagged or game is stopped
    if (isStop || display[idx] == 11) {
      return;
    }
    // if cell is openned
    if (display[idx] == cells[idx]) {
      int cnt = 0;
      for (int k = 0; k < 8; k++) {
        int ei = x + td[k], ej = y + tc[k];
        if (checkInside(ei, ej) && display[toCell(ei, ej)] == 11) {
          cnt++;
        }
      }
      if (cnt == cells[idx]) {
        for (int k = 0; k < 8; k++) {
          int ei = x + td[k], ej = y + tc[k];
          if (checkInside(ei, ej) && display[toCell(ei, ej)] == 10) {
            openCell(ei, ej);
          }
        }
      }
      return;
    }
    // if idx is bomb
    if (cells[idx] == 9) {
      for (int i = 0; i < numsCell; i++) {
        if (i == idx) {
          display[i] = 12;
        } else if (cells[i] == 9 && display[i] == 10) {
          display[i] = 9;
        } else if (cells[i] != 9 && display[i] == 11) {
          display[i] = 13;
        }
      }
      gameStatus = 1;
      isStop = 1;
      return;
    }
    display[idx] = cells[idx];
    cellsRemain--;
    // if idx == 0
    if (cells[idx] == 0) {
      std::queue<ii> q;
      q.push({x, y});
      while (!q.empty()) {
        ii u = q.front();
        q.pop();
        for (int k = 0; k < 8; k++) {
          int ei = u.F + td[k], ej = u.S + tc[k];
          int i = toCell(ei, ej);
          if (checkInside(ei, ej) && display[i] == 10) {
            if (cells[i] == 0) {
              q.push({ei, ej});
            }
            display[i] = cells[i];
            cellsRemain--;
          }
        }
      }
    }
    if (cellsRemain == bombs) {
      // win the game
      for (int bomb : bombCells) {
        if (display[bomb] == 10) {
          display[bomb] = 11;
        }
      }
      gameStatus = 2;
      isStop = 1;
      // update leaderboard
    }
  }
  void build(int clickedCell) {
    for (int num = 0; num < numsCell; num++) {
      if (num != clickedCell) {
        bombCells.pb(num);
      }
    }
    shuffle(all(bombCells), rng);
    bombCells.resize(bombs);
    isClicked = 1;

    for (int bomb : bombCells) {
      cells[bomb] = 9;
    }
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        if (cells[toCell(i, j)] != 9) {
          for (int k = 0; k < 8; k++) {
            int ei = i + td[k], ej = j + tc[k];
            if (checkInside(ei, ej) && cells[toCell(ei, ej)] == 9) {
              cells[toCell(i, j)]++;
            }
          }
        }
      }
    }
    isStop = 0;
    clock.restart();
  }
};
int main() {
  sf::Font font;
  sf::Text text, menuText;
  sf::Texture texture;
  sf::Sprite sprite;
  short appState = 0;

  // load assets
  if (!font.loadFromFile("assets/Comfortaa-Regular.ttf") ||
      !texture.loadFromFile("assets/tiles.jpg")) {
    std::cerr << "can not load assets\n";
    return 1;
  }

  sprite.setTexture(texture);
  //
  text.setFont(font);
  text.setFillColor(sf::Color::Red);
  text.setCharacterSize(12);
  text.setStyle(sf::Text::Bold);
  text.setPosition(sf::Vector2f(180, 20));
  //

  menuText.setFont(font);
  menuText.setFillColor(sf::Color::Black);
  menuText.setCharacterSize(15);
  menuText.setStyle(sf::Text::Bold);

  MainGame game;

  // load data
  game.load();

  // create the app
  sf::RenderWindow app(sf::VideoMode(32 * game.width, 32 * (game.height + 2)),
                       "Minesweeper", sf::Style::Close | sf::Style::Titlebar);

  // icon

  // sound

  // run the program as long as the app is open
  while (app.isOpen()) {
    // check all the app's events that were triggered since the last iteration
    // of the loop
    sf::Event event;

    while (app.pollEvent(event)) {
      // catch the resize events
      if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        sf::FloatRect visibleArea(
            sf::IntRect(sf::Vector2i(0, 0),
                        sf::Vector2i(event.size.width, event.size.height)));
        app.setView(sf::View(visibleArea));
      } else if (event.type == sf::Event::Closed) {
        game.save();
        app.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::N) {
          game.newgame();
          appState = 0;
        } else if (event.key.code == sf::Keyboard::Escape) {
          if (appState == 1) {
            appState = 0;
          } else {
            appState = 1;
          }
        } else if (event.key.code == sf::Keyboard::L) {
          if (appState == 2) {
            appState = 0;
          } else {
            appState = 2;
          }
        }
      } else if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i pos = sf::Mouse::getPosition(app);

        if (pos.y >= 5 && pos.y <= 44) {
          // newgame:   [10, 49] x [5, 44]
          // setting:   [54, 93] x [5, 44]
          // highscore: [98, 137] x [5, 44]
          if (pos.x >= 10 && pos.x <= 49) {
            appState = 0;
            game.newgame();
          } else if (pos.x >= 54 && pos.x <= 93) {
            appState = 1;
          } else if (pos.x >= 98 && pos.x <= 137) {
            appState = 2;
          }
        } else if (appState == 0) {
          int posX = pos.x / 32, posY = pos.y / 32 - 2;
          // check if mouse is inside grid
          if (game.checkInside(posY, posX)) {
            int idx = game.toCell(posY, posX);
            if (event.mouseButton.button == sf::Mouse::Left) {
              if (!game.isClicked) {
                game.build(idx);
              }
              game.openCell(posY, posX);
            } else if (event.mouseButton.button == sf::Mouse::Right) {
              game.flagCell(idx);
            }
          }
        } else if (appState == 1) {
          if (pos.y >= 96 && pos.y <= 192) {
            if (pos.x >= 32 && pos.x <= 128) {
              game.resizeGrid(defaultLevel[0]);
              appState = 0;
            } else if (pos.x >= 160 && pos.x <= 256) {
              game.resizeGrid(defaultLevel[1]);
              appState = 0;
            }
          } else if (pos.y >= 224 && pos.y <= 320) {
            if (pos.x >= 32 && pos.x <= 128) {
              game.resizeGrid(defaultLevel[2]);
              appState = 0;
            } else if (pos.x >= 160 && pos.x <= 256) {
            }
          }
        } else if (appState == 2) {
        }
      }
    }

    // start drawing
    app.clear(sf::Color::White);

    sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * (game.gameStatus), 32),
                                      sf::Vector2i(39, 39)));
    sprite.setPosition(sf::Vector2f(10, 5));
    app.draw(sprite);

    sprite.setTextureRect(
        sf::IntRect(sf::Vector2i(39 * 3, 32), sf::Vector2i(39, 39)));
    sprite.setPosition(sf::Vector2f(10 + 39 + 5, 5));
    app.draw(sprite);

    sprite.setTextureRect(
        sf::IntRect(sf::Vector2i(39 * 4, 32), sf::Vector2i(39, 39)));
    sprite.setPosition(sf::Vector2f(10 + 39 + 5 + 39 + 5, 5));
    app.draw(sprite);

    if (appState == 0) {
      app.setSize(sf::Vector2u(32 * game.width, 32 * (game.height + 2)));

      for (int i = 0; i < game.height; i++) {
        for (int j = 0; j < game.width; j++) {
          sprite.setTextureRect(
              sf::IntRect(sf::Vector2i(game.display[game.toCell(i, j)] * 32, 0),
                          sf::Vector2i(32, 32)));
          sprite.setPosition(sf::Vector2f(j * 32, i * 32 + 64));
          app.draw(sprite);
        }
      }
      text.setString(std::to_string(game.displayTime()));
      app.draw(text);
    } else if (appState == 1) {
      app.setSize(sf::Vector2u(288, 352));

      sprite.setTextureRect(sf::IntRect(
          sf::Vector2i(39 * (game.gameStatus), 32), sf::Vector2i(39, 39)));
      sprite.setPosition(sf::Vector2f(10, 5));
      app.draw(sprite);

      sprite.setTextureRect(
          sf::IntRect(sf::Vector2i(39 * 3, 32), sf::Vector2i(39, 39)));
      sprite.setPosition(sf::Vector2f(10 + 39 + 5, 5));
      app.draw(sprite);

      sprite.setTextureRect(
          sf::IntRect(sf::Vector2i(39 * 4, 32), sf::Vector2i(39, 39)));
      sprite.setPosition(sf::Vector2f(10 + 39 + 5 + 39 + 5, 5));
      app.draw(sprite);

      for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
          sprite.setTextureRect(
              sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
          sprite.setPosition(sf::Vector2f(j * 32, i * 32 + 64));
          app.draw(sprite);
        }
      }

      for (int i = 0; i < 4; i++) {
        for (int k = 0; k < 9; k++) {
          sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 5 + 32 * k, 32),
                                            sf::Vector2i(32, 32)));
          sprite.setPosition(
              sf::Vector2f(32 * tx[k] + mx[i], 64 + 32 * ty[k] + my[i]));
          app.draw(sprite);

          menuText.setString(defaultLevel[i].name);
          menuText.setPosition(sf::Vector2f(mx[i] + 15, 64 + my[i] + 15));
          app.draw(menuText);
        }
      }
    } else if (appState == 2) {
      app.setSize(sf::Vector2u(288, 352));
    }
    app.display();
  }
  return 0;
}

// setting (custom board)
// save (best time)
// remake timer
// music
// redraw tiles

// animation (pressed, ...)
// using keyboard