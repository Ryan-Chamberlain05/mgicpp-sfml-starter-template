
#ifndef PLATFORMER_GAME_H
#define PLATFORMER_GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class GameState
{
  MENU,
  PLAYING
};

class Game
{
 public:
  Game(sf::RenderWindow& window);
  bool init();

  void update(float dt);
  void render();

  void keyPressed(sf::Event event);
  void mouseButtonPressed(sf::Event event);
  void mouseButtonReleased(sf::Event event);

 private:
  void setupMenu();
  void renderMenu();
  bool collisionCheck(sf::Vector2i click, const sf::Sprite& sprite);
  void newAnimal();

  void positionDecisionMenu();
  void positionStampsInsidePassport();

  sf::RenderWindow& window;
  GameState game_state = GameState::MENU;

  sf::Texture background_texture;
  sf::Texture accept_button_tex;
  sf::Texture reject_button_tex;
  sf::Texture accept_stamp_tex;
  sf::Texture reject_stamp_tex;

  sf::Font font;

  sf::Text title_text;
  sf::Text play_option;
  sf::Text quit_option;
  sf::Text lives_text;
  sf::Text score_text;

  sf::Sprite background;
  sf::Sprite character;
  sf::Sprite passport;
  sf::Sprite accept_button;
  sf::Sprite reject_button;
  sf::Sprite accept_stamp;
  sf::Sprite reject_stamp;

  std::vector<sf::Texture> animals;
  std::vector<sf::Texture> passports;

  sf::FloatRect return_zone = sf::FloatRect(0, 0, 200, 200);

  int current_animal_index   = 0;
  int current_passport_index = 0;

  int lives     = 3;
  int max_lives = 3;
  int score     = 0;

  sf::Sprite* dragged = nullptr;
  sf::Vector2f drag_offset;

  float speed = 200.f;

  bool context_menu_open = false;
  bool should_accept     = false;
  bool show_accept_stamp = false;
  bool show_reject_stamp = false;
};

#endif
