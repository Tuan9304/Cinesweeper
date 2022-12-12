#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>

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

const int td[] = { -1, -1, -1, 0, 1, 1, 1, 0 },
          tc[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

struct MainGame {
    int height, width, bombs, numsCell, cellsRemain;
    vi bombCells, cells, display;
    bool isClicked, isStop;
    short gameStatus;
    sf::Clock clock;
    sf::Time lastTime;
    void newgame()
    {
        gameStatus = 0;
        isClicked = 0;
        isStop = 1;
        cells.assign(height * width, 0);
        display.assign(height * width, 10);
        cellsRemain = numsCell;
        bombCells.clear();
    }
    sf::Time displayTime()
    {
        if (!isClicked) {
            return sf::Time();
        }
        if (!isStop) {
            lastTime = clock.getElapsedTime();
        }
        return lastTime;
    }
    int toCell(int x, int y)
    {
        return x * width + y;
    }
    bool checkInside(int ei, int ej)
    {
        return ei >= 0 && ej >= 0 && ei < height && ej < width;
    }
    void resizeGrid(int h, int w, int b)
    {
        height = h;
        width = w;
        numsCell = h * w;
        bombs = std::min(numsCell - 1, b);
        newgame();
    }
    void flagCell(int idx)
    {
        // if cell is opened or game is stopped, then it can't be flagged
        if (isStop || display[idx] == cells[idx]) {
            return;
        }
        display[idx] ^= 1;
    }
    void openCell(int x, int y)
    {
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
                }
                else if (cells[i] == 9 && display[i] == 10) {
                    display[i] = 9;
                }
                else if (cells[i] != 9 && display[i] == 11) {
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
            q.push({ x, y });
            while (!q.empty()) {
                ii u = q.front();
                q.pop();
                for (int k = 0; k < 8; k++) {
                    int ei = u.F + td[k], ej = u.S + tc[k];
                    int i = toCell(ei, ej);
                    if (checkInside(ei, ej) && display[i] == 10) {
                        if (cells[i] == 0) {
                            q.push({ ei, ej });
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
    void build(int clickedCell)
    {
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
struct UI {
    sf::Text text;
    sf::Font font;
    sf::Texture texture;
    sf::Sprite sprite;
    UI()
    {
        font.loadFromFile("assets/Comfortaa-Regular.ttf");
        text.setFont(font);
        text.setFillColor(sf::Color::Red);
        text.setCharacterSize(12);
        text.setStyle(sf::Text::Bold);
        text.setPosition(sf::Vector2f(180, 20));
        texture.loadFromFile("assets/tiles.jpg");
        sprite.setTexture(texture);
    }
    void draw(sf::RenderWindow& app, MainGame& game)
    {
        sprite.setTextureRect(sf::IntRect(sf::Vector2i(14 * 32 + 39 * (game.gameStatus), 0), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10, 5));
        app.draw(sprite);

        sprite.setTextureRect(sf::IntRect(sf::Vector2i(14 * 32 + 39 * 3, 0), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10 + 39 + 5, 5));
        app.draw(sprite);

        sprite.setTextureRect(sf::IntRect(sf::Vector2i(14 * 32 + 39 * 4, 0), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10 + 39 + 5 + 39 + 5, 5));
        app.draw(sprite);

        for (int i = 0; i < game.height; i++) {
            for (int j = 0; j < game.width; j++) {
                sprite.setTextureRect(sf::IntRect(sf::Vector2i(game.display[game.toCell(i, j)] * 32, 0), sf::Vector2i(32, 32)));
                sprite.setPosition(sf::Vector2f(i * 32, j * 32 + 64));
                app.draw(sprite);
            }
        }
        text.setString(std::to_string((game.displayTime().asSeconds())));
        app.draw(text);
    }
};
int main()
{
    // create the app
    sf::RenderWindow app(sf::VideoMode(288, 357), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);
    MainGame game;
    UI ui;
    game.resizeGrid(9, 9, 10);

    // load leaderboard here

    // run the program as long as the app is open
    while (app.isOpen()) {

        // check all the app's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (app.pollEvent(event)) {
            // "close requested" event: we close the app
            if (event.type == sf::Event::Closed) {
                app.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(app);
                int posX = pos.x / 32, posY = pos.y / 32 - 2;
                // check if mouse is inside grid
                if (!game.checkInside(posX, posY)) {
                    if (pos.y >= 5 && pos.y <= 44) {
                        if (pos.x >= 10 && pos.x <= 49) {
                            game.newgame();
                        }
                        else if (pos.x >= 54 && pos.x <= 93) {
                        }
                        else if (pos.x >= 98 && pos.x <= 137) {
                        }
                    }
                    // newgame:   [10, 49] x [5, 44]
                    // setting:   [54, 93] x [5, 44]
                    // highscore: [98, 137] x [5, 44]
                    continue;
                }
                int idx = game.toCell(posX, posY);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (!game.isClicked) {
                        game.build(idx);
                    }
                    game.openCell(posX, posY);
                }
                else if (event.mouseButton.button == sf::Mouse::Right) {
                    game.flagCell(idx);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::N) {
                    game.newgame();
                }
            }
        }
        app.clear(sf::Color::White);
        ui.draw(app, game);
        app.display();
    }

    return 0;
}

// ?? music :)
// remake clock
// save game (state, best time)
// setting (resize)
// ?? using keyboard
// redraw tiles
// animation (pressed, ...)

// https://www.sfml-dev.org/tutorials/2.5/system-stream.php
