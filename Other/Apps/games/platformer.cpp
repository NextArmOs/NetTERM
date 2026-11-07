#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>

enum class GameState {
    MainMenu,
    LevelSelect,
    Playing
};

struct Platform {
    sf::FloatRect rect;
    sf::RectangleShape shape;

    Platform(float x, float y, float w, float h) {
        rect.position = {x, y};
        rect.size = {w, h};
        
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(sf::Color(120, 120, 120));
    }
};

struct UIButton {
    sf::FloatRect rect;
    sf::RectangleShape shape;
    int id;

    UIButton(float x, float y, float w, float h, int buttonId, sf::Color color) {
        rect.position = {x, y};
        rect.size = {w, h};
        
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(color);
        id = buttonId;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "NetTERM Platformer Arcade");
    window.setFramerateLimit(60);

    GameState currentState = GameState::MainMenu;
    int currentLevel = 1;

    sf::RectangleShape player({40.0f, 40.0f});
    player.setFillColor(sf::Color::Green);
    
    sf::Vector2f velocity{0.0f, 0.0f};
    const float gravity = 0.4f;
    const float jumpSpeed = -13.5f;
    const float moveSpeed = 5.0f;
    bool isGrounded = false;

    std::vector<Platform> platforms;

    std::vector<UIButton> menuButtons;
    menuButtons.push_back(UIButton(300.0f, 250.0f, 200.0f, 60.0f, 0, sf::Color(0, 150, 0)));
    menuButtons.push_back(UIButton(300.0f, 350.0f, 200.0f, 60.0f, 1, sf::Color(150, 0, 0)));

    std::vector<UIButton> levelButtons;
    levelButtons.push_back(UIButton(200.0f, 250.0f, 150.0f, 80.0f, 1, sf::Color(0, 100, 150)));
    levelButtons.push_back(UIButton(450.0f, 250.0f, 150.0f, 80.0f, 2, sf::Color(150, 100, 0)));

    sf::RectangleShape levelIndicator({20.0f, 20.0f});
    levelIndicator.setFillColor(sf::Color::Cyan);
    levelIndicator.setPosition({20.0f, 20.0f});

    while (window.isOpen()) {
        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseButtonEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    if (currentState == GameState::MainMenu) {
                        for (const auto& btn : menuButtons) {
                            if (btn.rect.contains(mousePosF)) {
                                if (btn.id == 0) currentState = GameState::LevelSelect;
                                if (btn.id == 1) window.close();
                            }
                        }
                    }
                    else if (currentState == GameState::LevelSelect) {
                        for (const auto& btn : levelButtons) {
                            if (btn.rect.contains(mousePosF)) {
                                currentLevel = btn.id;
                                platforms.clear();
                                platforms.push_back(Platform(0.0f, 550.0f, 800.0f, 50.0f));
                                
                                if (currentLevel == 1) {
                                    platforms.push_back(Platform(150.0f, 450.0f, 150.0f, 20.0f));
                                    platforms.push_back(Platform(380.0f, 350.0f, 150.0f, 20.0f));
                                    platforms.push_back(Platform(600.0f, 250.0f, 150.0f, 20.0f));
                                } else {
                                    platforms.push_back(Platform(200.0f, 400.0f, 100.0f, 20.0f));
                                    platforms.push_back(Platform(450.0f, 280.0f, 100.0f, 20.0f));
                                    platforms.push_back(Platform(250.0f, 160.0f, 100.0f, 20.0f));
                                }
                                
                                player.setPosition({100.0f, 400.0f});
                                velocity = {0.0f, 0.0f};
                                currentState = GameState::Playing;
                            }
                        }
                    }
                }
            }
        }

        if (currentState == GameState::Playing) {
            velocity.x = 0.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
                velocity.x = -moveSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
                velocity.x = moveSpeed;
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) && isGrounded) {
                velocity.y = jumpSpeed;
                isGrounded = false;
            }

            velocity.y += gravity;
            player.move({velocity.x, 0.0f});

            sf::FloatRect playerBounds = player.getGlobalBounds();
            for (const auto& plat : platforms) {
                if (playerBounds.findIntersection(plat.rect)) {
                    if (velocity.x > 0.0f) player.setPosition({plat.rect.position.x - playerBounds.size.x, player.getPosition().y});
                    if (velocity.x < 0.0f) player.setPosition({plat.rect.position.x + plat.rect.size.x, player.getPosition().y});
                }
            }

            player.move({0.0f, velocity.y});
            playerBounds = player.getGlobalBounds();
            isGrounded = false;

            for (const auto& plat : platforms) {
                if (playerBounds.findIntersection(plat.rect)) {
                    if (velocity.y > 0.0f) {
                        player.setPosition({player.getPosition().x, plat.rect.position.y - playerBounds.size.y});
                        velocity.y = 0.0f;
                        isGrounded = true;
                    }
                    else if (velocity.y < 0.0f) {
                        player.setPosition({player.getPosition().x, plat.rect.position.y + plat.rect.size.y});
                        velocity.y = 0.0f;
                    }
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                currentState = GameState::MainMenu;
            }
        }

        window.clear(sf::Color(15, 15, 15));

        if (currentState == GameState::MainMenu) {
            for (const auto& btn : menuButtons) {
                window.draw(btn.shape);
            }
        }
        else if (currentState == GameState::LevelSelect) {
            for (const auto& btn : levelButtons) {
                window.draw(btn.shape);
            }
        }
        else if (currentState == GameState::Playing) {
            for (const auto& plat : platforms) {
                window.draw(plat.shape);
            }
            window.draw(player);
            
            levelIndicator.setPosition({20.0f * currentLevel, 20.0f});
            window.draw(levelIndicator);
        }

        window.display();
    }

    return 0;
}
