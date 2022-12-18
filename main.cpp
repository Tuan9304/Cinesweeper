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

const short td[] = { -1, -1, -1, 0, 1, 1, 1, 0 },
          tc[] = { -1, 0, 1, 1, 1, 0, -1, -1 };

const short tx[] = { 0, 1, 2, 0, 0, 1, 2, 2, 1 },
            ty[] = { 0, 0, 0, 1, 2, 2, 2, 1, 1 };

const short mx[] = { 32, 32 * 5, 32, 32 * 5 },
            my[] = { 32, 32, 32 * 5, 32 * 5 };

const std::string mt[] = { "Easy", "Medium", "Hard", "Custom" };

struct MainGame {
    int height, width, bombs, numsCell, cellsRemain;
    vi bombCells, cells, display;
    bool isClicked, isStop;
    short gameStatus;
    sf::Clock clock;
    sf::Time lastTime;
    void save(std::ofstream &wf) {
        wf.write((char*) &height, sizeof(height));
        wf.write((char*) &width, sizeof(width));
        wf.write((char*) &bombs, sizeof(bombs));
        wf.write((char*) &numsCell, sizeof(numsCell));
        wf.write((char*) &cellsRemain, sizeof(cellsRemain));
        wf.write((char*) &bombCells, sizeof(bombCells));
        wf.write((char*) &cells, sizeof(cells));
        wf.write((char*) &display, sizeof(display));
        wf.write((char*) &isClicked, sizeof(isClicked));
        wf.write((char*) &isStop, sizeof(isStop));
        wf.write((char*) &gameStatus, sizeof(gameStatus));
    }
    void load(std::ifstream &rf) {
        rf.read((char*) &height, sizeof(height));
        rf.read((char*) &width, sizeof(width));
        rf.read((char*) &bombs, sizeof(bombs));
        rf.read((char*) &numsCell, sizeof(numsCell));
        rf.read((char*) &cellsRemain, sizeof(cellsRemain));
        rf.read((char*) &bombCells, sizeof(bombCells));
        rf.read((char*) &cells, sizeof(cells));
        rf.read((char*) &display, sizeof(display));
        rf.read((char*) &isClicked, sizeof(isClicked));
        rf.read((char*) &isStop, sizeof(isStop));
        rf.read((char*) &gameStatus, sizeof(gameStatus));
    }
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
int main()
{
    sf::Font font;
    sf::Text text, menuText;
    sf::Texture texture;
    sf::Sprite sprite;
    short gameState = 0;

    // load assets
    if(!font.loadFromFile("assets/Comfortaa-Regular.ttf") || !texture.loadFromFile("assets/tiles.jpg")) {
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

    // load leaderboard here
    std::ifstream rf("game.dat", std::ios::in | std::ios::binary);
    game.resizeGrid(9,9,10);
    rf.close();

    // create the app
    sf::RenderWindow app(sf::VideoMode(32 * game.width, 32 * (game.height + 2)), "Minesweeper", sf::Style::Close | sf::Style::Titlebar);

    // icon

    // sound

    // run the program as long as the app is open
    while (app.isOpen()) {

        // check all the app's events that were triggered since the last iteration of the loop
        sf::Event event;

        while(app.pollEvent(event)) {
            // catch the resize events
            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(event.size.width, event.size.height)));
                app.setView(sf::View(visibleArea));
            }
            else if(event.type == sf::Event::Closed) {
                std::ofstream wf("game.dat", std::ios::out | std::ios::binary);
                wf.close();
                app.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::N) {
                    // fix later
                    if(gameState == 0) {
                        game.newgame();
                    }
                    else gameState = 0;
                }
                else if(event.key.code == sf::Keyboard::Escape) {
                    gameState = 1;
                }
                else if(event.key.code == sf::Keyboard::L) {
                    gameState = 2;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(app);

                if (pos.y >= 5 && pos.y <= 44) {
                    // newgame:   [10, 49] x [5, 44]
                    // setting:   [54, 93] x [5, 44]
                    // highscore: [98, 137] x [5, 44]
                    if (pos.x >= 10 && pos.x <= 49) {
                        gameState = 0;
                        game.newgame();
                    }
                    else if (pos.x >= 54 && pos.x <= 93) {
                        gameState = 1;
                    }
                    else if (pos.x >= 98 && pos.x <= 137) {
                        gameState = 2;
                    }
                }
                else if(gameState == 0) {
                    int posX = pos.x / 32, posY = pos.y / 32 - 2;
                    // check if mouse is inside grid
                    if (game.checkInside(posY, posX)) {
                        int idx = game.toCell(posY, posX);
                        if (event.mouseButton.button == sf::Mouse::Left) {
                            if (!game.isClicked) {
                                game.build(idx);
                            }
                            game.openCell(posY, posX);
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right) {
                            game.flagCell(idx);
                        }
                    }
                }
                else if(gameState == 1) {
                    if(pos.y >= 96 && pos.y <= 192) {
                        if(pos.x >= 32 && pos.x <= 128) {
                            game.resizeGrid(9, 9, 10);
                            gameState = 0;
                        }
                        else if(pos.x >= 160 && pos.x <= 256) {
                            game.resizeGrid(16, 16, 40);
                            gameState = 0;
                        }
                    }
                    else if(pos.y >= 224 && pos.y <= 320) {
                        if(pos.x >= 32 && pos.x <= 128) {
                            game.resizeGrid(16, 30, 99);
                            gameState = 0;
                        }
                        else if(pos.x >= 160 && pos.x <= 256) {
                        }
                    }
                }
                else if(gameState == 2) {

                }
            }
        }

        // start drawing
        app.clear(sf::Color::White);

        sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * (game.gameStatus), 32), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10, 5));
        app.draw(sprite);

        sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 3, 32), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10 + 39 + 5, 5));
        app.draw(sprite);

        sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 4, 32), sf::Vector2i(39, 39)));
        sprite.setPosition(sf::Vector2f(10 + 39 + 5 + 39 + 5, 5));
        app.draw(sprite);

        if(gameState == 0) {
            app.setSize(sf::Vector2u(32 * game.width, 32 * (game.height + 2)));

            for (int i = 0; i < game.height; i++) {
                for (int j = 0; j < game.width; j++) {
                    sprite.setTextureRect(sf::IntRect(sf::Vector2i(game.display[game.toCell(i, j)] * 32, 0), sf::Vector2i(32, 32)));
                    sprite.setPosition(sf::Vector2f(j * 32, i * 32 + 64));
                    app.draw(sprite);
                }
            }
            text.setString(std::to_string((game.displayTime().asSeconds())));
            app.draw(text);
        }
        else if(gameState == 1) {
            app.setSize(sf::Vector2u(288, 352));

            sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * (game.gameStatus), 32), sf::Vector2i(39, 39)));
            sprite.setPosition(sf::Vector2f(10, 5));
            app.draw(sprite);

            sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 3, 32), sf::Vector2i(39, 39)));
            sprite.setPosition(sf::Vector2f(10 + 39 + 5, 5));
            app.draw(sprite);

            sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 4, 32), sf::Vector2i(39, 39)));
            sprite.setPosition(sf::Vector2f(10 + 39 + 5 + 39 + 5, 5));
            app.draw(sprite);

            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32)));
                    sprite.setPosition(sf::Vector2f(j * 32, i * 32 + 64));
                    app.draw(sprite);
                }
            }

            for(int i = 0; i < 4; i++) {
                for(int k = 0; k < 9; k++) {
                    sprite.setTextureRect(sf::IntRect(sf::Vector2i(39 * 5 + 32 * k, 32), sf::Vector2i(32, 32)));
                    sprite.setPosition(sf::Vector2f(32 * tx[k] + mx[i], 64 + 32 * ty[k] + my[i]));
                    app.draw(sprite);

                    menuText.setString(mt[i]);
                    menuText.setPosition(sf::Vector2f(mx[i] + 15, 64 + my[i] + 15));
                    app.draw(menuText);
                }
            }
        }
        else if(gameState == 2) {
            app.setSize(sf::Vector2u(288, 352));

        }
        app.display();
    }
    return 0;
}

// ?? music :)
// remake clock
// save game (state(whole struct?), best time)
// setting (resize)
// ?? using keyboard
// redraw tiles
// animation (pressed, ...)

// https://www.sfml-dev.org/tutorials/2.5/system-stream.php
