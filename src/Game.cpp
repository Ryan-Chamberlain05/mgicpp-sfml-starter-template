#include "Game.h"
#include <iostream>

Game::Game(sf::RenderWindow& game_window) : window(game_window)
{
  srand(static_cast<unsigned>(time(NULL)));
}

bool Game::init()
{
  if (!background_texture.loadFromFile("../Data/Images/background.png"))
    std::cout << "Failed to load background\n";
  background.setTexture(background_texture);

  if (!font.loadFromFile("../Data/Fonts/OpenSans-Bold.ttf"))
    std::cout << "Failed to load font\n";

  animals.resize(3);
  animals[0].loadFromFile("../Data/Images/moose.png");
  animals[1].loadFromFile("../Data/Images/elephant.png");
  animals[2].loadFromFile("../Data/Images/penguin.png");

  passports.resize(3);
  passports[0].loadFromFile("../Data/Images/moose passport.png");
  passports[1].loadFromFile("../Data/Images/elephant passport.png");
  passports[2].loadFromFile("../Data/Images/penguin passport.png");

  accept_button_tex.loadFromFile("../Data/Images/accept button.png");
  reject_button_tex.loadFromFile("../Data/Images/reject button.png");
  accept_button.setTexture(accept_button_tex);
  reject_button.setTexture(reject_button_tex);

  accept_stamp_tex.loadFromFile("../Data/Images/accept.png");
  reject_stamp_tex.loadFromFile("../Data/Images/reject.png");
  accept_stamp.setTexture(accept_stamp_tex);
  reject_stamp.setTexture(reject_stamp_tex);

  accept_button.setPosition(-200, -200);
  reject_button.setPosition(-200, -200);
  accept_stamp.setPosition(-200, -200);
  reject_stamp.setPosition(-200, -200);

  setupMenu();
  newAnimal();

  lives = max_lives;
  score = 0;

  lives_text.setFont(font);
  lives_text.setCharacterSize(30);
  lives_text.setFillColor(sf::Color::Red);
  lives_text.setPosition(20, 20);

  score_text.setFont(font);
  score_text.setCharacterSize(30);
  score_text.setFillColor(sf::Color::Yellow);
  score_text.setPosition(20, 60);

  return true;
}

void Game::setupMenu()
{
  float W = window.getView().getSize().x;
  float H = window.getView().getSize().y;

  title_text.setFont(font);
  title_text.setString("Critter Crossing");
  title_text.setCharacterSize(60);
  title_text.setFillColor(sf::Color::White);

  float titleX = (W - title_text.getLocalBounds().width) / 2.f;
  title_text.setPosition(titleX, H * 0.15f);

  play_option.setFont(font);
  play_option.setString("Press ENTER to Start");
  play_option.setCharacterSize(30);
  play_option.setFillColor(sf::Color::Yellow);

  float playX = (W - play_option.getLocalBounds().width) / 2.f;
  play_option.setPosition(playX, H * 0.45f);

  quit_option.setFont(font);
  quit_option.setString("Press ESC to Quit");
  quit_option.setCharacterSize(30);
  quit_option.setFillColor(sf::Color::Yellow);

  float quitX = (W - quit_option.getLocalBounds().width) / 2.f;
  quit_option.setPosition(quitX, H * 0.55f);
}

void Game::newAnimal()
{
  current_animal_index   = rand() % 3;
  current_passport_index = rand() % 3;

  should_accept = (current_animal_index == current_passport_index);

  float W = window.getView().getSize().x;
  float H = window.getView().getSize().y;

  character.setTexture(animals[current_animal_index], true);
  character.setScale(1.f, 1.f);
  character.setPosition(W * 0.08f, H * 0.12f);

  passport.setTexture(passports[current_passport_index], true);
  passport.setScale(1.f, 1.f);
  passport.setPosition(W * 0.40f, H * 0.12f);

  show_accept_stamp = false;
  show_reject_stamp = false;
  accept_stamp.setPosition(-200, -200);
  reject_stamp.setPosition(-200, -200);

  positionDecisionMenu();
  positionStampsInsidePassport();
}

void Game::update(float dt)
{
  if (game_state == GameState::PLAYING)
  {
    // character movement disabled by design (commented out)
    /* character.move(speed * dt, 0.f);

    float rightEdge = window.getView().getSize().x;
    if (character.getPosition().x > rightEdge)
      character.setPosition(0.f, character.getPosition().y);
    */

    if (dragged != nullptr)
    {
      sf::Vector2i mousePix   = sf::Mouse::getPosition(window);
      sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePix);
      dragged->setPosition(mouseWorld - drag_offset);

      if (context_menu_open)
        positionDecisionMenu();
      if (show_accept_stamp || show_reject_stamp)
        positionStampsInsidePassport();
    }
    else
    {
      if (context_menu_open)
        positionDecisionMenu();
      if (show_accept_stamp || show_reject_stamp)
        positionStampsInsidePassport();
    }

    lives_text.setString("Lives: " + std::to_string(lives));
    score_text.setString("Score: " + std::to_string(score));
  }
}

void Game::render()
{
  window.clear();

  if (game_state == GameState::MENU)
  {
    renderMenu();
  }
  else
  {
    window.draw(background);
    window.draw(character);
    window.draw(passport);

    if (show_accept_stamp)
      window.draw(accept_stamp);
    if (show_reject_stamp)
      window.draw(reject_stamp);

    if (context_menu_open)
    {
      window.draw(accept_button);
      window.draw(reject_button);
    }

    window.draw(lives_text);
    window.draw(score_text);
  }

  window.display();
}

void Game::renderMenu()
{
  window.draw(background);
  window.draw(title_text);
  window.draw(play_option);
  window.draw(quit_option);
}

void Game::mouseButtonReleased(sf::Event event)
{
  if (event.mouseButton.button == sf::Mouse::Left)
  {
    if (dragged != nullptr)
    {
      if (return_zone.contains(dragged->getPosition()))
      {
        if (show_accept_stamp || show_reject_stamp)
        {
          bool player_correct = false;

          if (show_accept_stamp && should_accept)
            player_correct = true;
          if (show_reject_stamp && !should_accept)
            player_correct = true;

          if (player_correct)
          {
            std::cout << "Correct decision!\n";
            score++;
          }
          else
          {
            std::cout << "Wrong decision!\n";
            lives--;

            if (lives <= 0)
            {
              std::cout << "Game Over!\n";
              game_state = GameState::MENU;
              newAnimal();
              dragged = nullptr;
              return;
            }
          }

          newAnimal();
          show_accept_stamp = false;
          show_reject_stamp = false;
          context_menu_open = false;
        }
      }
    }

    dragged = nullptr;
  }
}

void Game::keyPressed(sf::Event event)
{
  if (game_state == GameState::MENU)
  {
    if (event.key.code == sf::Keyboard::Enter)
    {
      game_state = GameState::PLAYING;

      score = 0;
      lives = max_lives;

      newAnimal();
    }
    else if (event.key.code == sf::Keyboard::Escape)
      window.close();
  }
  else if (game_state == GameState::PLAYING)
  {
    if (event.key.code == sf::Keyboard::Escape)
      game_state = GameState::MENU;
    else if (event.key.code == sf::Keyboard::Space)
      newAnimal();
  }
}

void Game::mouseButtonPressed(sf::Event event)
{
  sf::Vector2f pos =
    window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

  if (event.mouseButton.button == sf::Mouse::Left)
  {
    if (passport.getGlobalBounds().contains(pos))
    {
      dragged     = &passport;
      drag_offset = pos - passport.getPosition();
    }

    if (context_menu_open && accept_button.getGlobalBounds().contains(pos))
    {
      show_accept_stamp = true;
      show_reject_stamp = false;

      positionStampsInsidePassport();
      context_menu_open = false;
    }

    if (context_menu_open && reject_button.getGlobalBounds().contains(pos))
    {
      show_reject_stamp = true;
      show_accept_stamp = false;

      positionStampsInsidePassport();
      context_menu_open = false;
    }
  }

  if (event.mouseButton.button == sf::Mouse::Right)
  {
    context_menu_open = !context_menu_open;

    if (context_menu_open)
    {
      positionDecisionMenu();
    }
    else
    {
      accept_button.setPosition(-200, -200);
      reject_button.setPosition(-200, -200);
    }
  }
}

bool Game::collisionCheck(sf::Vector2i click, const sf::Sprite& sprite)
{
  return sprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(click));
}

void Game::positionDecisionMenu()
{
  sf::FloatRect p = passport.getGlobalBounds();

  float paddingX = 20.f;
  float paddingY = 8.f;

  float rightX = p.left + p.width + paddingX;
  float topY   = p.top;

  float acceptH = accept_button.getGlobalBounds().height;
  float rejectH = reject_button.getGlobalBounds().height;
  float spacing = 10.f;

  accept_button.setPosition(rightX, topY);
  reject_button.setPosition(rightX, topY + acceptH + spacing);
}

void Game::positionStampsInsidePassport()
{
  sf::FloatRect p = passport.getGlobalBounds();

  float offsetX = p.width * 0.12f;
  float offsetY = p.height * 0.12f;

  accept_stamp.setPosition(p.left + offsetX, p.top + offsetY);
  reject_stamp.setPosition(p.left + offsetX, p.top + offsetY);
}



