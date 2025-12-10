#include "Game.h"
#include <SFML/Graphics.hpp>

int main()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Critter Crossing", sf::Style::Fullscreen);

    const float VIEW_WIDTH = 1080;
    const float VIEW_HEIGHT = 720;

    sf::View view(sf::FloatRect(0.f, 0.f, VIEW_WIDTH, VIEW_HEIGHT));
    window.setView(view);
    window.setFramerateLimit(60);

    Game game(window);
    if (!game.init())
        return 0;

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized)
            {
                float window_ratio = float(event.size.width) / float(event.size.height);
                float view_ratio = VIEW_WIDTH / VIEW_HEIGHT;

                float sizeX = 1.f;
                float sizeY = 1.f;

                if (window_ratio < view_ratio)
                    sizeY = window_ratio / view_ratio;
                else
                    sizeX = view_ratio / window_ratio;

                sf::View newView(sf::FloatRect(0.f, 0.f, VIEW_WIDTH, VIEW_HEIGHT));
                newView.setViewport(
                    { (1.f - sizeX) * 0.5f, (1.f - sizeY) * 0.5f, sizeX, sizeY });

                window.setView(newView);
            }

            if (event.type == sf::Event::KeyPressed)
                game.keyPressed(event);
            else if (event.type == sf::Event::MouseButtonPressed)
                game.mouseButtonPressed(event);
            else if (event.type == sf::Event::MouseButtonReleased)
                game.mouseButtonReleased(event);
        }

        float dt = clock.restart().asSeconds();
        game.update(dt);
        game.render();
    }
    return 0;
}