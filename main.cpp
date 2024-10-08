#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>
#include <fstream>
#include <string>
#include <iostream>

#include "Clock.h"

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

struct NumField : public sf::Drawable {
    NumField(unsigned int maxd, const sf::Vector2f &pos) {
        maxDigit = maxd;
        rect.setSize(sf::Vector2f(25 * maxDigit, 39));
        hasfocus = 0;
        if(!font.loadFromFile("assets/Comfortaa-Regular.ttf")) {
            std::cerr << "can not load font!\n";
        }
        rect.setOutlineThickness(2);
        rect.setFillColor(sf::Color::White);
        rect.setOutlineColor(sf::Color(127, 127, 127));
        dText.setFont(font);
        dText.setCharacterSize(28);
        dText.setFillColor(sf::Color::Black);
        setPosition(pos);
        num = 0;
    }
    int getNum() const
    {
        return num;
    }
    void setPosition(const sf::Vector2f &pos)
    {
        rect.setPosition(pos);
        dText.setPosition(sf::Vector2f(pos.x + 10, pos.y + 5));
    }
    bool contains(sf::Vector2f point) const
    {
        return rect.getGlobalBounds().contains(point);
    }
    void setFocus(bool focus)
    {
        hasfocus = focus;
        if (focus) {
            rect.setOutlineColor(sf::Color::Blue);
        }
        else {
            rect.setOutlineColor(sf::Color(127, 127, 127)); // Gray color
        }
    }
    void handleInput(sf::Event e)
    {
        if (!hasfocus || e.type != sf::Event::TextEntered) {
            return;
        }
        if (e.text.unicode == 8 && !text.empty()) { // Delete key
            text.pop_back();
            num /= 10;
        }
        else if (text.size() < maxDigit && e.text.unicode >= 48 && e.text.unicode <= 57) {
            text.push_back(e.text.unicode);
            num = num * 10 + (e.text.unicode - 48);
        }
        dText.setString(text);
    }
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(rect, states);
        target.draw(dText, states);
    }
private:
    unsigned int maxDigit;
    sf::Font font;
    sf::Text dText;
    std::string text;
    int num;
    sf::RectangleShape rect;
    bool hasfocus;
};

struct Level {
    int height, width, bombs, dif;
    Level(int h, int w, int b, int d)
        : height(h)
        , width(w)
        , bombs(b)
        , dif(d)
    {
    }
    bool cmp(Level lvl) {
        return ((lvl.height == height && lvl.width == width) || (lvl.height == width && lvl.width == height)) && bombs == lvl.bombs;
    }
};

const std::string levelName[] = {"Easy", "Medium", "Hard", "Custom" };

const Level defaultLevel[] = { Level(9, 9, 10, 0),
    Level(16, 16, 40, 1),
    Level(16, 30, 99, 2),
    Level(0, 0, 0, 3) };

struct MainGame {
    int height, width, bombs, numsCell, cellsRemain, highscore[3] = {9999, 9999, 9999}, bombsRemain;
    vi display;
    bool isClicked, isStop;
    short gameStatus;
    void save()
    {
        std::ofstream wf("game.dat", std::ios::out | std::ios::binary);
        if (wf.is_open()) {
            int rawTime = clock.getRawTime();

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
            wf.write((char*)&lastTime, sizeof(int));
            wf.write((char*)&rawTime, sizeof(int));
            wf.write((char*)&highscore, sizeof(int) * 3);
            wf.write((char*)&bombsRemain, sizeof(int));
            wf.close();
        }
        else {
            std::cerr << "Unable to open data file\n";
        }

        clock.stop();
    }
    void load()
    {
        std::ifstream rf("game.dat", std::ios::in | std::ios::binary);
        if (rf.is_open()) {
            int rawTime;

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
            rf.read((char*)&lastTime, sizeof(int));
            rf.read((char*)&rawTime, sizeof(int));
            rf.read((char*)&highscore, sizeof(int) * 3);
            rf.read((char*)&bombsRemain, sizeof(int));
            rf.close();

            numsCell = height * width;
            if(!isStop) {
                lastTime = rawTime >> clock.hs;
            }
            clock.load(rawTime);
        }
        else {
            std::cerr << "Unable to open data file\n";
            resizeGrid(defaultLevel[0]);
        }
    }
    void newgame()
    {
        gameStatus = 0;
        isClicked = 0;
        isStop = 1;
        cells.assign(numsCell, 0);
        display.assign(numsCell, 10);
        cellsRemain = numsCell;
        bombCells.clear();
        bombsRemain = bombs;
    }
    void resetHighscore() {
        highscore[0] = highscore[1] = highscore[2] = 9999;
    }
    int displayTime()
    {
        if (!isClicked) {
            return 0;
        }
        if (!isStop) {
            lastTime = clock.getElapsedTime();
        }
        return lastTime;
    }
    int toCell(int x, int y) { return x * width + y; }
    bool checkInside(int ei, int ej)
    {
        return ei >= 0 && ej >= 0 && ei < height && ej < width;
    }
    void resizeGrid(Level level)
    {
        height = level.height;
        width = level.width;
        lvl = level.dif;
        numsCell = height * width;
        bombs = level.bombs;
        newgame();
    }
    void flagCell(int idx)
    {
        // if cell is opened or game is stopped, then it can't be flagged
        if ((isStop && isClicked) || display[idx] == cells[idx]) {
            return;
        }
        if(display[idx] == 11) {
            bombsRemain++;
            display[idx] ^= 1;
        }
        else if(bombsRemain > 0) {
            bombsRemain--;
            display[idx] ^= 1;
        }
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
            displayTime();
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
            displayTime();
            gameStatus = 2;
            isStop = 1;

            // update leaderboard
            if(lvl < 3 && highscore[lvl] > displayTime()) {
                highscore[lvl] = displayTime();
            }
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
private:
    Time::Clock clock;
    int lastTime, lvl;
    vi bombCells, cells;

    void viWrite(std::ofstream& wf, const vi& v)
    {
        int size = v.size();
        wf.write((char*)&size, sizeof(int));
        wf.write((char*)v.data(), v.size() * sizeof(int));
    }
    void viRead(std::ifstream& rf, vi& v)
    {
        int size;
        rf.read((char*)&size, sizeof(int));
        while (size--) {
            int i;
            rf.read((char*)&i, sizeof(int));
            v.push_back(i);
        }
    }
};
void drawSprite(sf::Sprite sprite, sf::RenderWindow& app, int posX, int posY, int w, int h, double aposX, double aposY)
{
    sprite.setTextureRect(sf::IntRect(sf::Vector2i(posX, posY), sf::Vector2i(w, h)));
    sprite.setPosition(sf::Vector2f(aposX, aposY));
    app.draw(sprite);
}
bool checkCustomMode(int height, int width, int bombs, int &errc) {
    auto res = sf::VideoMode::getDesktopMode();
    errc = 0;
    if(height == 0) {
        errc |= 1;
    }
    if(height > (res.height / 32) - 7) {
        errc |= 2;
    }
    if(width < 9) {
        errc |= 4;
    }
    if(width > (res.width / 32) - 2) {
        errc |= 8;
    }
    if(bombs >= height * width) {
        errc |= 16;
    }
    return !errc;
}
int main()
{
    sf::Font font;
    sf::Text text, menuText, customText, errText;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Image icon;
    sf::Music music;
    short appState = 0;
    int errc = 0, curCusCell = 0;

    // load assets
    if (!font.loadFromFile("assets/Comfortaa-Regular.ttf") ||
        !texture.loadFromFile("assets/tiles.jpg") ||
        !icon.loadFromFile("assets/icon.png") ||
        !music.openFromFile("assets/music.ogg")) {

        std::cerr << "can not load assets\n";
        return 1;
    }

    errText.setFont(font);
    errText.setFillColor(sf::Color::Red);
    errText.setCharacterSize(15);
    errText.setStyle(sf::Text::Bold);

    NumField customHeight(3, sf::Vector2f(200, 70)),
             customWidth(3, sf::Vector2f(200, 150)),
             customBombs(3, sf::Vector2f(200, 230));

    customText.setFont(font);
    customText.setFillColor(sf::Color::Black);
    customText.setCharacterSize(28);

    sprite.setTexture(texture);
    //
    text.setFont(font);
    text.setCharacterSize(20);
    text.setStyle(sf::Text::Bold);
    text.setPosition(sf::Vector2f(180, 20));
    //

    menuText.setFont(font);
    menuText.setFillColor(sf::Color::Black);
    menuText.setCharacterSize(20);

    MainGame game;

    // load data
    game.load();

    // create the app
    sf::RenderWindow app(sf::VideoMode(32 * game.width, 32 * (game.height + 2)),
        "Minesweeper", sf::Style::Close | sf::Style::Titlebar);

    // icon
    app.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // sound
    music.setLoop(1);
    music.setVolume(17);
    music.setPlayingOffset(sf::seconds(rng() % 200));
    music.play();

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
            }
            else if (event.type == sf::Event::Closed) {
                game.save();
                app.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::N) {
                    game.newgame();
                    appState = 0;
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    if (appState == 1) {
                        appState = 0;
                    }
                    else {
                        appState = 1;
                    }
                }
                else if (event.key.code == sf::Keyboard::L) {
                    if (appState == 2) {
                        appState = 0;
                    }
                    else {
                        appState = 2;
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i pos = sf::Mouse::getPosition(app);

                if (pos.y >= 5 && pos.y <= 43) {
                    if (pos.x >= 10 && pos.x <= 48) {
                        appState = 0;
                        game.newgame();
                    }
                    else if (pos.x >= 54 && pos.x <= 92) {
                        appState = 1;
                    }
                    else if (pos.x >= 98 && pos.x <= 136) {
                        appState = 2;
                    }
                    else if(pos.x >= 236 && pos.x <= 274 && appState) {
                        if(appState == 3) {
                            appState = 1;
                        }
                        else if(appState == 1 || appState == 2) {
                            appState = 0;
                        }
                    }
                    else if (pos.x >= 192 && pos.x <= 230) {
                        if(appState == 2) {
                            game.resetHighscore();
                        }
                        else if(appState == 1) {
                            if(music.getStatus() == sf::Music::Paused) {
                                music.play();
                            }
                            else {
                                music.pause();
                            }
                        }
                    }
                }
                else if (appState == 0) {
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
                else if (appState == 1) {
                    if (pos.y >= 84 && pos.y <= 179) {
                        if (pos.x >= 32 && pos.x <= 127) {
                            game.resizeGrid(defaultLevel[0]);
                            appState = 0;
                        }
                        else if (pos.x >= 160 && pos.x <= 255) {
                            game.resizeGrid(defaultLevel[1]);
                            appState = 0;
                        }
                    }
                    else if (pos.y >= 212 && pos.y <= 307) {
                        if (pos.x >= 32 && pos.x <= 127) {
                            game.resizeGrid(defaultLevel[2]);
                            appState = 0;
                        }
                        else if (pos.x >= 160 && pos.x <= 255) {
                            appState = 3;
                        }
                    }
                }
                else if (appState == 2) {
                }
                else if(appState == 3) {
                    if(pos.x >= 64 && pos.x <= 223 && pos.y >= 285 && pos.y <= 348) {
                        int h = customHeight.getNum();
                        int w = customWidth.getNum();
                        int b = customBombs.getNum();
                        curCusCell = h * w;
                        if(checkCustomMode(h, w, b, errc)) {
                            Level cst(h, w, b, 3);
                            for(int i = 0; i <= 2; i++) {
                                if(cst.cmp(defaultLevel[i])) {
                                    cst.dif = i;
                                    break;
                                }
                            }
                            game.resizeGrid(cst);
                            appState = 0;
                        }
                    }
                }
            }
            else if(appState == 3) {
                auto pos = sf::Vector2f(sf::Mouse::getPosition(app));
                if(event.type == sf::Event::MouseButtonReleased) {
                    customBombs.setFocus(customBombs.contains(pos));
                    customHeight.setFocus(customHeight.contains(pos));
                    customWidth.setFocus(customWidth.contains(pos));
                }
                else {
                    customBombs.handleInput(event);
                    customHeight.handleInput(event);
                    customWidth.handleInput(event);
                }
            }
        }

        // start drawing
        app.clear(sf::Color(230, 230, 230));

        drawSprite(sprite, app, 39 * (game.gameStatus), 32, 39, 39, 10, 5);
        drawSprite(sprite, app, 39 * 3, 32, 39, 39, 10 + 39 + 5, 5);
        drawSprite(sprite, app, 39 * 4, 32, 39, 39, 10 + 39 + 5 + 39 + 5, 5);

        if (appState == 0) {
            app.setSize(sf::Vector2u(32 * game.width, 32 * (game.height + 2)));
            for (int i = 0; i < game.height; i++) {
                for (int j = 0; j < game.width; j++) {
                    drawSprite(sprite, app, game.display[game.toCell(i, j)] * 32, 0, 32, 32, j * 32, i * 32 + 64);
                }
            }

            text.setFillColor(game.gameStatus == 2 ? sf::Color::Blue : sf::Color::Red);

            std::string timer = std::to_string(game.displayTime());
            text.setString(timer);
            text.setPosition(sf::Vector2f(32 * game.width - 15 - timer.size() * 10, 14));
            app.draw(text);

            text.setString(std::to_string(game.gameStatus == 2 ? 0 : game.bombsRemain));
            text.setPosition(sf::Vector2f(32 * game.width - 125, 14));
            app.draw(text);
        }
        else if (appState == 1) {
            app.setSize(sf::Vector2u(288, 352));
            drawSprite(sprite, app, 483, 0, 39, 39, 236, 5);
            drawSprite(sprite, app, 522, 39 * (music.getStatus() == sf::Music::Paused), 39, 39, 236 - 5 - 39, 5);

            for (int i = 0; i < 4; i++) {
                for (int k = 0; k < 9; k++) {
                    drawSprite(sprite, app, 39 * 5 + 32 * k, 32, 32, 32, 32 * tx[k] + mx[i], 52 + 32 * ty[k] + my[i]);
                }
                menuText.setString(levelName[i]);
                menuText.setPosition(sf::Vector2f(mx[i] + 5 + (96 - levelName[i].size() * 16) / 2, 52 + my[i] + 33));
                app.draw(menuText);
            }
        }
        else if (appState == 2) {
            app.setSize(sf::Vector2u(288, 352));
            drawSprite(sprite, app, 483, 0, 39, 39, 236, 5);
            drawSprite(sprite, app, 483, 39, 39, 39, 236 - 5 - 39, 5);
            // add name later
            customText.setPosition(sf::Vector2f(13, 73));
            customText.setString("Easy");
            app.draw(customText);
            customText.setPosition(sf::Vector2f(203, 73));
            customText.setString(std::to_string(game.highscore[0]));
            app.draw(customText);

            customText.setPosition(sf::Vector2f(13, 165));
            customText.setString("Medium");
            app.draw(customText);
            customText.setPosition(sf::Vector2f(203, 165));
            customText.setString(std::to_string(game.highscore[1]));
            app.draw(customText);

            customText.setPosition(sf::Vector2f(13, 257));
            customText.setString("Hard");
            app.draw(customText);
            customText.setPosition(sf::Vector2f(203, 257));
            customText.setString(std::to_string(game.highscore[2]));
            app.draw(customText);
        }
        else if (appState == 3) {
            drawSprite(sprite, app, 483, 0, 39, 39, 236, 5);
            auto res = sf::VideoMode::getDesktopMode();

            app.draw(customBombs);
            customText.setPosition(sf::Vector2f(13, 73));
            customText.setString("Height");
            app.draw(customText);
            if(errc & 3) {
                if(errc & 1) errText.setString("Height must >= 1");
                else errText.setString("Height must <= " + std::to_string(res.height / 32 - 7));
                errText.setPosition(sf::Vector2f(13, 50));
                app.draw(errText);
            }

            app.draw(customHeight);
            customText.setPosition(sf::Vector2f(13, 153));
            customText.setString("Width");
            app.draw(customText);
            if(errc & 12) {
                if(errc & 4) errText.setString("Width must >= 9");
                else errText.setString("Width must <= " + std::to_string(res.width / 32 - 2));
                errText.setPosition(sf::Vector2f(13, 130));
                app.draw(errText);
            }

            app.draw(customWidth);
            customText.setPosition(sf::Vector2f(13, 233));
            customText.setString("Bombs");
            app.draw(customText);
            if(!(errc & 15) && errc & 16) {
                errText.setString("Bombs must <= " + std::to_string(curCusCell - 1));
                errText.setPosition(sf::Vector2f(13, 210));
                app.draw(errText);
            }

            drawSprite(sprite, app, 39 * 5, 32, 32, 32, 64, 285);
            drawSprite(sprite, app, 39 * 5 + 32, 32, 32, 32, 64 + 32, 285);
            drawSprite(sprite, app, 39 * 5 + 32, 32, 32, 32, 64 + 64, 285);
            drawSprite(sprite, app, 39 * 5 + 32, 32, 32, 32, 64 + 96, 285);
            drawSprite(sprite, app, 39 * 5 + 64, 32, 32, 32, 64 + 128, 285);
            drawSprite(sprite, app, 39 * 5 + 32 * 4, 32, 32, 32, 64, 285 + 32);
            drawSprite(sprite, app, 39 * 5 + 32 * 5, 32, 32, 32, 64 + 32, 285 + 32);
            drawSprite(sprite, app, 39 * 5 + 32 * 5, 32, 32, 32, 64 + 64, 285 + 32);
            drawSprite(sprite, app, 39 * 5 + 32 * 5, 32, 32, 32, 64 + 96, 285 + 32);
            drawSprite(sprite, app, 39 * 5 + 32 * 6, 32, 32, 32, 64 + 128, 285 + 32);
            customText.setPosition(sf::Vector2f(72, 300));
            customText.setString("Let's start");
            app.draw(customText);
        }
        app.display();
    }

    return 0;
}

// animation (pressed, released, ...)
