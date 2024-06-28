#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct FallingCandy {
    sf::Sprite sprite;
    float targetY;
    float speed;
};

// Function to scale a sprite to fit the window
void scaleSpriteToWindow(sf::Sprite& sprite, const sf::RenderWindow& window) {
    float scaleX = static_cast<float>(window.getSize().x) / sprite.getLocalBounds().width;
    float scaleY = static_cast<float>(window.getSize().y) / sprite.getLocalBounds().height;
    sprite.setScale(scaleX, scaleY);
}

// Function to detect and mark matches
std::vector<std::vector<bool>> detectMatches(const std::vector<std::vector<sf::Sprite>>& grid) {
    const int gridSize = grid.size();
    std::vector<std::vector<bool>> toRemove(gridSize, std::vector<bool>(gridSize, false));

    // Check for horizontal matches
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize - 2; j++) {
            if (grid[i][j].getTexture() == grid[i][j + 1].getTexture() &&
                grid[i][j].getTexture() == grid[i][j + 2].getTexture()) {
                toRemove[i][j] = true;
                toRemove[i][j + 1] = true;
                toRemove[i][j + 2] = true;
            }
        }
    }

    // Check for vertical matches
    for (int i = 0; i < gridSize - 2; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid[i][j].getTexture() == grid[i + 1][j].getTexture() &&
                grid[i][j].getTexture() == grid[i + 2][j].getTexture()) {
                toRemove[i][j] = true;
                toRemove[i + 1][j] = true;
                toRemove[i + 2][j] = true;
            }
        }
    }

    return toRemove;
}

void removeMatches(std::vector<std::vector<sf::Sprite>>& grid, const std::vector<sf::Texture>& candyTextures, std::vector<std::vector<bool>>& toRemove, std::vector<FallingCandy>& fallingCandies) {
    const int gridSize = grid.size();
    const int tileSize = 55;

    for (int j = 0; j < gridSize; j++) {
        int emptySpaces = 0;
        for (int i = gridSize - 1; i >= 0; i--) {
            if (toRemove[i][j]) {
                emptySpaces++;
            }
            else if (emptySpaces > 0) {
                FallingCandy fallingCandy;
                fallingCandy.sprite = grid[i][j];
                fallingCandy.targetY = 230 + (i + emptySpaces) * tileSize;
                fallingCandy.speed = 100.0f; // pixels per second
                fallingCandies.push_back(fallingCandy);

                grid[i + emptySpaces][j] = grid[i][j];
                toRemove[i][j] = true;
            }
        }

        // Add new candies at the top
        for (int i = 0; i < emptySpaces; i++) {
            int randomIndex = rand() % candyTextures.size();
            FallingCandy fallingCandy;
            fallingCandy.sprite.setTexture(candyTextures[randomIndex]);
            fallingCandy.sprite.setPosition(220 + j * tileSize, 230 - (emptySpaces - i) * tileSize);
            fallingCandy.targetY = 230 + i * tileSize;
            fallingCandy.speed = 200.0f; // pixels per second
            fallingCandies.push_back(fallingCandy);

            grid[i][j].setTexture(*fallingCandy.sprite.getTexture());
            grid[i][j].setPosition(220 + j * tileSize, 230 + i * tileSize);
        }
    }
}

bool updateFallingCandies(std::vector<FallingCandy>& fallingCandies, std::vector<std::vector<sf::Sprite>>& grid, float dt) {
    bool stillFalling = false;
    const int gridSize = grid.size();
    const int tileSize = 55;

    for (auto& candy : fallingCandies) {
        float distanceToFall = candy.targetY - candy.sprite.getPosition().y;
        float movement = candy.speed * dt;

        if (movement > distanceToFall) {
            movement = distanceToFall;
        }

        if (distanceToFall > 0) {
            candy.sprite.move(0, movement);
            stillFalling = true;
        }
    }

    // Update grid positions
    if (!stillFalling) {
        for (const auto& candy : fallingCandies) {
            int gridX = static_cast<int>((candy.sprite.getPosition().x - 220) / tileSize);
            int gridY = static_cast<int>((candy.sprite.getPosition().y - 230) / tileSize);
            if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize) {
                grid[gridY][gridX] = candy.sprite;
            }
        }
    }

    return stillFalling;
}

void updateGridFromSprites(std::vector<std::vector<sf::Sprite>>& grid) {
    const int gridSize = grid.size();
    const int tileSize = 55;
    std::vector<std::vector<sf::Sprite>> newGrid(gridSize, std::vector<sf::Sprite>(gridSize));

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            sf::Vector2f pos = grid[i][j].getPosition();
            int gridX = static_cast<int>((pos.x - 220) / tileSize);
            int gridY = static_cast<int>((pos.y - 230) / tileSize);
            if (gridX >= 0 && gridX < gridSize && gridY >= 0 && gridY < gridSize) {
                newGrid[gridY][gridX] = grid[i][j];
            }
        }
    }

    grid = newGrid;
}

// Function to animate removal of candies
void animateRemoval(std::vector<std::vector<sf::Sprite>>& grid, const std::vector<std::vector<bool>>& toRemove, float dt) {
    const float fadeSpeed = 255.0f / 0.5f;  // Fade out in 0.5 seconds
    const int gridSize = grid.size();

    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (toRemove[i][j]) {
                sf::Color color = grid[i][j].getColor();
                color.a = std::max(0.0f, color.a - fadeSpeed * dt);
                grid[i][j].setColor(color);
            }
        }
    }
}

void highlightTile(sf::RenderWindow& window, const std::vector<std::vector<sf::Sprite>>& grid, int gridSize, int tileSize, sf::RectangleShape& highlight) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    int hoverI = (mousePos.y - 230) / tileSize;
    int hoverJ = (mousePos.x - 220) / tileSize;

    if (hoverI >= 0 && hoverI < gridSize && hoverJ >= 0 && hoverJ < gridSize) {
        highlight.setPosition(220 + hoverJ * tileSize, 230 + hoverI * tileSize);
        window.draw(highlight);
    }
}


// Function to check if two tiles are adjacent
bool areAdjacent(const sf::Vector2i& tile1, const sf::Vector2i& tile2) {
    return (tile1.x == tile2.x && abs(tile1.y - tile2.y) == 1) || (tile1.y == tile2.y && abs(tile1.x - tile2.x) == 1);
}

// Function to swap two tiles
void swapTiles(sf::Sprite& tile1, sf::Sprite& tile2) {
    sf::Texture* tempTexture = const_cast<sf::Texture*>(tile1.getTexture());
    tile1.setTexture(*tile2.getTexture());
    tile2.setTexture(*tempTexture);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(900, 900), "CANDY CRUSH!");
    window.setVerticalSyncEnabled(true);

    // Load assets
    sf::Texture startTexture, backgroundTexture, gameBackgroundTexture, settingsBGTexture, pauseBGTexture, buttonTexture, m1buttonTexture, m2buttonTexture, homeButtonTexture, soundButtonTexture, noSoundButtonTexture, restartButtonTexture, resumeButtonTexture, pauseButtonTexture;
    sf::Sprite startSprite, backgroundSprite, gameBackgroundSprite, settingsBGSprite, pauseBGSprite, playButtonSprite, scoreButtonSprite, exitButtonSprite, insButtonSprite, settingsButtonSprite, homeButtonSprite, soundButtonSprite, noSoundButtonSprite, restartButtonSprite, resumeButtonSprite, pauseButtonSprite;

    if (!startTexture.loadFromFile("images/start.jpg") ||
        !backgroundTexture.loadFromFile("images/menuBackground.jpg") ||
        !gameBackgroundTexture.loadFromFile("images/gameBackground.jpg") ||
        !buttonTexture.loadFromFile("images/button1.png") ||
        !m1buttonTexture.loadFromFile("images/ibutton.png") ||
        !m2buttonTexture.loadFromFile("images/settings.png") ||
        !settingsBGTexture.loadFromFile("images/settingsBackground.jpg") ||
        !pauseBGTexture.loadFromFile("images/pauseBackground.jpg") ||
        !homeButtonTexture.loadFromFile("images/home2.png") ||
        !soundButtonTexture.loadFromFile("images/sound2.png") ||
        !noSoundButtonTexture.loadFromFile("images/noSound2.png") ||
        !restartButtonTexture.loadFromFile("images/restart.png") ||
        !resumeButtonTexture.loadFromFile("images/resume.png") ||
        !pauseButtonTexture.loadFromFile("images/pause.png"))
    {
        return EXIT_FAILURE;
    }

    startSprite.setTexture(startTexture);
    backgroundSprite.setTexture(backgroundTexture);
    gameBackgroundSprite.setTexture(gameBackgroundTexture);
    playButtonSprite.setTexture(buttonTexture);
    scoreButtonSprite.setTexture(buttonTexture);
    exitButtonSprite.setTexture(buttonTexture);
    insButtonSprite.setTexture(m1buttonTexture);
    settingsButtonSprite.setTexture(m2buttonTexture);
    settingsBGSprite.setTexture(settingsBGTexture);
    pauseBGSprite.setTexture(pauseBGTexture);
    homeButtonSprite.setTexture(homeButtonTexture);
    soundButtonSprite.setTexture(soundButtonTexture);
    noSoundButtonSprite.setTexture(noSoundButtonTexture);
    restartButtonSprite.setTexture(restartButtonTexture);
    resumeButtonSprite.setTexture(resumeButtonTexture);
    pauseButtonSprite.setTexture(pauseButtonTexture);

    scaleSpriteToWindow(startSprite, window);
    scaleSpriteToWindow(backgroundSprite, window);
    scaleSpriteToWindow(gameBackgroundSprite, window);
    scaleSpriteToWindow(settingsBGSprite, window);
    scaleSpriteToWindow(pauseBGSprite, window);

    // Set positions
    playButtonSprite.setPosition(315, 250);
    scoreButtonSprite.setPosition(315, 350);
    exitButtonSprite.setPosition(315, 450);
    insButtonSprite.setPosition(370, 550);
    settingsButtonSprite.setPosition(460, 550);

    restartButtonSprite.setPosition(370, 250);
    resumeButtonSprite.setPosition(540, 250);

    pauseButtonSprite.setPosition(450, 150);

    homeButtonSprite.setPosition(200, 250);
    soundButtonSprite.setPosition(370, 250);
    noSoundButtonSprite.setPosition(540, 250);
    backgroundSprite.setPosition(
        (window.getSize().x - backgroundSprite.getGlobalBounds().width) / 2.f,
        (window.getSize().y - 30 - backgroundSprite.getGlobalBounds().height) / 2.f
    );


    sf::Font font;
    if (!font.loadFromFile("fonts/font.ttf"))
        return EXIT_FAILURE;

    sf::Text playText("PLAY", font, 30);
    sf::Text scoreText("SCORE", font, 30);
    sf::Text exitText("EXIT", font, 30);

    playText.setFillColor(sf::Color::White);
    scoreText.setFillColor(sf::Color::White);
    exitText.setFillColor(sf::Color::White);

    playText.setPosition(
        playButtonSprite.getPosition().x + (playButtonSprite.getGlobalBounds().width - playText.getGlobalBounds().width) / 2.f,
        playButtonSprite.getPosition().y - 10 + (playButtonSprite.getGlobalBounds().height - playText.getGlobalBounds().height) / 2.f
    );
    scoreText.setPosition(
        scoreButtonSprite.getPosition().x + (scoreButtonSprite.getGlobalBounds().width - scoreText.getGlobalBounds().width) / 2.f,
        scoreButtonSprite.getPosition().y - 10 + (scoreButtonSprite.getGlobalBounds().height - scoreText.getGlobalBounds().height) / 2.f
    );
    exitText.setPosition(
        exitButtonSprite.getPosition().x + (exitButtonSprite.getGlobalBounds().width - exitText.getGlobalBounds().width) / 2.f,
        exitButtonSprite.getPosition().y - 10 + (exitButtonSprite.getGlobalBounds().height - exitText.getGlobalBounds().height) / 2.f
    );

    // Load music
    sf::Music startMusic, backgroundMusic, gameMusic;
    if (!startMusic.openFromFile("audio/intro.mp3") ||
        !backgroundMusic.openFromFile("audio/Theme.mp3") ||
        !gameMusic.openFromFile("audio/gameMusic.mp3"))
    {
        return EXIT_FAILURE;
    }

    startMusic.setLoop(true);
    startMusic.play();


    std::vector<sf::Texture> candyTextures(6);
    if (!candyTextures[0].loadFromFile("images/sprite1.png") ||
        !candyTextures[1].loadFromFile("images/sprite2.png") ||
        !candyTextures[2].loadFromFile("images/sprite3.png") ||
        !candyTextures[3].loadFromFile("images/sprite4.png") ||
        !candyTextures[4].loadFromFile("images/sprite5.png") ||
        !candyTextures[5].loadFromFile("images/sprite6.png"))
    {
        return EXIT_FAILURE;
    }

    // Create Grid
    const int gridSize = 9;
    const int tileSize = 55;

    // Create a rectangle for highlighting
    sf::RectangleShape highlight(sf::Vector2f(tileSize, tileSize));
    highlight.setFillColor(sf::Color(255, 255, 255, 100)); // Semi-transparent white
    
    std::vector<std::vector<sf::Sprite>> grid(gridSize, std::vector<sf::Sprite>(gridSize));
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int randomIndex = rand() % (candyTextures.size() - 1);
            grid[i][j].setTexture(candyTextures[randomIndex]);
            grid[i][j].setPosition(220 + j * tileSize, 230 + i * tileSize);
        }
    }

    sf::Clock clock;
    int switched = 0;

    bool isRemoving = false;
    sf::Clock removalClock;
    std::vector<std::vector<bool>> matches;
    std::vector<FallingCandy> fallingCandies;

    sf::Vector2i firstClick(-1, -1);
    sf::Vector2i secondClick(-1, -1);
    sf::Clock gameClock;
    bool soundCheck = true;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                std::cout << "Mouse position: (" << mousePos.x << ", " << mousePos.y << ")" << std::endl;

                if (switched == 1) {
                    if (exitButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        window.close(); // Exit Game
                    }
                    else if (playButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 2; // 2 = PLAY Game
                        backgroundMusic.stop();
                        if (soundCheck) {
                            gameMusic.setLoop(true);
                            gameMusic.play();
                        }
                    }
                    else if (scoreButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 3; // 3 = SCORE 
                    }
                    else if (settingsBGSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 4; // 4 = SETTINGS
                    }
                }
                else if (switched == 2)
                {
                    int clickedI = (mousePos.y - 230) / tileSize;
                    int clickedJ = (mousePos.x - 220) / tileSize;

                    // Pause Button
                    if (pauseButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) 
                    {
                        switched = 5; // PAUSE GAME
                    }

                    if (clickedI >= 0 && clickedI < gridSize && clickedJ >= 0 && clickedJ < gridSize) {
                        if (firstClick == sf::Vector2i(-1, -1)) {
                            firstClick = sf::Vector2i(clickedI, clickedJ);
                        }
                        else {
                            secondClick = sf::Vector2i(clickedI, clickedJ);
                        }
                    }

                    // If both clicks are valid and adjacent, try swapping
                    if (firstClick != sf::Vector2i(-1, -1) && secondClick != sf::Vector2i(-1, -1)) {
                        if (areAdjacent(firstClick, secondClick)) {
                            // Swap tiles
                            swapTiles(grid[firstClick.x][firstClick.y], grid[secondClick.x][secondClick.y]);

                            // Check for matches
                            std::vector<std::vector<bool>> newMatches = detectMatches(grid);
                            bool hasMatches = false;
                            for (const auto& row : newMatches) {
                                if (std::any_of(row.begin(), row.end(), [](bool b) { return b; })) {
                                    hasMatches = true;
                                    break;
                                }
                            }

                            if (!hasMatches) {
                                // Swap back if no matches
                                swapTiles(grid[firstClick.x][firstClick.y], grid[secondClick.x][secondClick.y]);
                            }
                            else {
                                matches = newMatches;
                                isRemoving = true;
                                removalClock.restart();
                            }
                        }

                        // Reset clicks
                        firstClick = sf::Vector2i(-1, -1);
                        secondClick = sf::Vector2i(-1, -1);
                    }
                }
                else if (switched == 4)
                {
                    if (homeButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 1; // Home Screen
                    }
                    else if (soundButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) 
                    {
                        if (!soundCheck) {
                            backgroundMusic.setLoop(true);
                            backgroundMusic.play();
                        }
                        soundCheck = true;
                    }
                    else if (noSoundButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        if (soundCheck) {
                            backgroundMusic.stop();
                        }
                        soundCheck = false;
                    }
                }
                else if (switched == 5)
                {
                    if (homeButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 1; // Home Screen
                    }
                    else if (resumeButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 2; // Resume GAME Screen
                    }
                    else if (restartButtonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
                    {
                        switched = 2; // Restart WITH NEW GRID Screen
                        for (int i = 0; i < gridSize; i++) {
                            for (int j = 0; j < gridSize; j++) {
                                int randomIndex = rand() % (candyTextures.size() - 1);
                                grid[i][j].setTexture(candyTextures[randomIndex]);
                                grid[i][j].setPosition(220 + j * tileSize, 230 + i * tileSize);
                            }
                        }
                    }
                }
                else 
                {
                    std::cout << "Invalid Switch Value\n";
                }

            }
        }

        // Check if 1 second has passed to switch to the background
        if (switched == 0 && clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            switched = 1; // 1 = MENU Screen
            startMusic.stop();
            if (soundCheck) {
                backgroundMusic.setLoop(true);
                backgroundMusic.play();
            }
        }

        window.clear();

        float dt = gameClock.restart().asSeconds();
        // Draw the appropriate sprite
        switch (switched)
        {
        case 0: // Intro Screen
            window.draw(startSprite);
            break;
        case 1: // Start Screen
            window.draw(backgroundSprite);
            window.draw(playButtonSprite);
            window.draw(playText);
            window.draw(scoreButtonSprite);
            window.draw(scoreText);
            window.draw(exitButtonSprite);
            window.draw(exitText);
            window.draw(insButtonSprite);
            window.draw(settingsButtonSprite);
            break;
        case 2: // Play Screen
            window.draw(gameBackgroundSprite);
            window.draw(pauseButtonSprite);

            if (!isRemoving && fallingCandies.empty()) {
                // Detect matches
                matches = detectMatches(grid);

                // Check if there are any matches
                bool hasMatches = false;
                for (const auto& row : matches) {
                    if (std::any_of(row.begin(), row.end(), [](bool b) { return b; })) {
                        hasMatches = true;
                        break;
                    }
                }

                if (hasMatches) {
                    // Start removal animation
                    isRemoving = true;
                    removalClock.restart();
                }
            }
            else if (isRemoving) {
                float removalTime = removalClock.getElapsedTime().asSeconds();
                if (removalTime < 0.5f) {
                    animateRemoval(grid, matches, dt);
                }
                else {
                    removeMatches(grid, candyTextures, matches, fallingCandies);
                    isRemoving = false;
                }
            }
            else {
                // Update falling candies
                bool stillFalling = updateFallingCandies(fallingCandies, grid, clock.restart().asSeconds());
                if (!stillFalling) {
                    fallingCandies.clear();
                }
            }

            // Draw candy grid
            for (const auto& row : grid) {
                for (const auto& candy : row) {
                    window.draw(candy);
                }
            }

            // Highlight the tile under the mouse cursor
            highlightTile(window, grid, gridSize, tileSize, highlight);

            break;
        case 3: // Score Screen
            // Score screen drawing code
            break;
        case 4:
            window.draw(settingsBGSprite);
            window.draw(homeButtonSprite);
            window.draw(soundButtonSprite);
            window.draw(noSoundButtonSprite);

            break;
        case 5:
            window.draw(pauseBGSprite);
            window.draw(homeButtonSprite);
            window.draw(resumeButtonSprite);
            window.draw(restartButtonSprite);
            //window.draw(settingsButtonSprite);

            break;
        }

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}