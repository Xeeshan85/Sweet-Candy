//#include <SFML/Graphics.hpp>
////#include <cstdlib>
//#include <ctime>
//int main()
//{
//    sf::RenderWindow window(sf::VideoMode(1200, 650), "GAY CHECKER!");
//    window.setVerticalSyncEnabled(true);
//
//    sf::Font font;
//    if (!font.loadFromFile("fonts/font.ttf"))
//        return EXIT_FAILURE;
//    sf::Text text("GAY", font, 50);
//    text.setPosition(-100, -125);
//
//    sf::RectangleShape button(sf::Vector2f(150, 50));
//    button.setFillColor(sf::Color(20, 211, 13));
//    button.setOutlineThickness(8);
//    button.setOutlineColor(sf::Color::Black);
//    button.setPosition(550, 275);
//    sf::FloatRect buttonBounds = button.getGlobalBounds();
//    sf::Text buttonText("Click Me", font, 24);
//    buttonText.setFillColor(sf::Color::White);
//    buttonText.setPosition(buttonBounds.left + (buttonBounds.width - buttonText.getGlobalBounds().width) / 2,
//        buttonBounds.top + (buttonBounds.height - buttonText.getGlobalBounds().height) / 2);
//
//
//
//    bool gameStarted = false;
//    srand(time(0));
//
//    int moveSpeedx = 3;
//    int moveSpeedy = 3;
//    while (window.isOpen())
//    {
//        sf::Event event;
//        while (window.pollEvent(event))
//        {
//            if (event.type == sf::Event::Closed)
//                window.close();
//            if (event.type == sf::Event::MouseButtonPressed && !gameStarted)
//            {
//                // Start the game and set text position
//                gameStarted = true;
//                int ranx = rand() % (window.getSize().x - static_cast<int>(text.getLocalBounds().width));
//                int rany = rand() % (window.getSize().y - static_cast<int>(text.getLocalBounds().height));
//                text.setPosition(ranx, rany);
//                //window.draw(buttonText);
//            }
//            /*if (event.type == sf::Event::MouseButtonPressed && gameStarted && moveSpeedx <= 50) {
//                moveSpeedx += 1; moveSpeedy += 1;
//            }*/
//        }
//
//        if (gameStarted)
//        {
//            sf::Vector2f pos = text.getPosition();
//            pos.x += moveSpeedx;
//            pos.y += moveSpeedy;
//            if (pos.x >= window.getSize().x - text.getLocalBounds().width || pos.x <= 0)
//            {
//                moveSpeedx = -moveSpeedx;
//            }
//            if (pos.y >= window.getSize().y - text.getLocalBounds().height || pos.y <= 0)
//            {
//                moveSpeedy = -moveSpeedy;
//            }
//            text.setPosition(pos);
//            if (clock() % 2 == 0)
//            {
//                text.setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
//                text.setOutlineColor(sf::Color(rand() % 256, rand() % 256, rand() % 256));
//            }
//            if (clock() % 10 == 0)
//            {
//                moveSpeedx += 2;
//                moveSpeedy += 2;
//            }
//        }
//
//        window.clear();
//        if (!gameStarted)
//        {
//            window.draw(button);
//            window.draw(buttonText);// Draw the button if the game hasn't started
//        }
//        window.draw(text);
//        window.display();
//    }
//
//    return 0;
//}