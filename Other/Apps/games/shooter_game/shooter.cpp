#include "main.h"
#include <cmath>
#include <random>
#include <optional>
#include <vector>

Mode g_currentMode = Mode::Menu;
std::chrono::high_resolution_clock::time_point g_appStartTime;

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    int damage;

    Bullet(sf::Vector2f startPos, sf::Vector2f targetPos, bool isMinigan) {
        shape.setRadius(isMinigan ? 3.0f : 6.0f);
        shape.setFillColor(isMinigan ? sf::Color(255, 140, 0) : sf::Color::Yellow);
        shape.setOrigin({shape.getRadius(), shape.getRadius()});
        shape.setPosition(startPos);
        damage = isMinigan ? g_config.miniganDamage : g_config.bulletDamage;

        sf::Vector2f direction = targetPos - startPos;
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) direction /= length;
        velocity = direction * (isMinigan ? 18.0f : 14.0f);
    }
};

struct Enemy {
    sf::RectangleShape shape;
    float speed;
    int type;
    int hp;

    Enemy(sf::Vector2f startPos, int enemyType) {
        type = enemyType;
        if (type == 1) {
            shape.setSize({30.0f, 30.0f});
            shape.setFillColor(sf::Color(255, 50, 50));
            speed = 4.2f;
            hp = g_config.enemyBaseHealth;
        } else if (type == 2) {
            shape.setSize({70.0f, 70.0f});
            shape.setFillColor(sf::Color(139, 0, 139));
            speed = 1.5f;
            hp = g_config.enemyTankHealth;
        } else {
            shape.setSize({45.0f, 45.0f});
            shape.setFillColor(sf::Color(220, 80, 80));
            speed = 2.8f;
            hp = g_config.enemyBaseHealth;
        }
        shape.setOrigin(shape.getSize() / 2.0f);
        shape.setPosition(startPos);
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(2.0f);
    }

    void update(sf::Vector2f playerPos) {
        sf::Vector2f direction = playerPos - shape.getPosition();
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length != 0) direction /= length;
        shape.move(direction * speed);
    }
};

struct SpawnPoint {
    sf::CircleShape shape;
    sf::Vector2f position;
    int enemyType;

    SpawnPoint(sf::Vector2f pos, int type) {
        position = pos;
        enemyType = type;
        shape.setRadius(20.0f);
        shape.setOrigin({20.0f, 20.0f});
        shape.setPosition(pos);
        shape.setOutlineThickness(3.0f);
        shape.setOutlineColor(sf::Color::White);
        if (type == 1) shape.setFillColor(sf::Color(255, 0, 0, 100));
        else if (type == 2) shape.setFillColor(sf::Color(255, 0, 255, 100));
        else shape.setFillColor(sf::Color(200, 50, 50, 100));
    }
};

int main() {
    g_appStartTime = std::chrono::high_resolution_clock::now();
    
    sf::RenderWindow window(sf::VideoMode({g_config.windowWidth, g_config.windowHeight}), "NetTERM shooter");
    window.setFramerateLimit(g_config.fpsLimit);

        sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/dejavu/DejaVuSans.ttf") && 
        !font.openFromFile("/usr/share/fonts/noto/NotoSans-Regular.ttf") &&
        !font.openFromFile("/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf")) {
        font.openFromFile("DejaVuSans.ttf");
    }

    int selectedEditorType = 0;
    int health = g_config.playerMaxHealth;
    int score = 0;
    bool crosshairColorCyan = true;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);

    sf::RectangleShape player({50.0f, 50.0f});
    player.setFillColor(sf::Color(50, 205, 50));
    player.setOrigin({25.0f, 25.0f});
    player.setPosition({960.0f, 540.0f});

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<SpawnPoint> editorSpawns;

    sf::VertexArray crosshair(sf::PrimitiveType::Lines, 4);

    UIButton btnPlay(860.0f, 400.0f, 200.0f, 60.0f, 0, sf::Color(0, 150, 0), "PLAY", font);
    UIButton btnEdit(860.0f, 490.0f, 200.0f, 60.0f, 1, sf::Color(0, 100, 150), "EDITOR", font);
    UIButton btnSets(860.0f, 580.0f, 200.0f, 60.0f, 2, sf::Color(120, 120, 120), "SETTINGS", font);
    UIButton btnExit(860.0f, 670.0f, 200.0f, 60.0f, 3, sf::Color(180, 0, 0), "EXIT", font);

    sf::RectangleShape healthBarOuter({400.0f, 25.0f}); healthBarOuter.setPosition({50.0f, 50.0f}); healthBarOuter.setFillColor(sf::Color::Transparent); healthBarOuter.setOutlineColor(sf::Color::White); healthBarOuter.setOutlineThickness(2.0f);
    sf::RectangleShape healthBarInner({400.0f, 25.0f}); healthBarInner.setPosition({50.0f, 50.0f}); healthBarInner.setFillColor(sf::Color::Green);
    sf::RectangleShape scoreIndicator({16.0f, 16.0f}); scoreIndicator.setFillColor(sf::Color::Yellow);

    auto lastSpawn = std::chrono::steady_clock::now();
    auto lastMiniganShot = std::chrono::steady_clock::now();

    while (window.isOpen()) {
        window.setFramerateLimit(g_config.fpsLimit);
        
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(window.mapPixelToCoords(mousePos));

        sf::Color currentCrosshairColor = crosshairColorCyan ? sf::Color::Cyan : sf::Color::Yellow;
        crosshair[0].position = {mousePosF.x - 12.0f, mousePosF.y - 12.0f}; crosshair[0].color = currentCrosshairColor;
        crosshair[1].position = {mousePosF.x + 12.0f, mousePosF.y + 12.0f}; crosshair[1].color = currentCrosshairColor;
        crosshair[2].position = {mousePosF.x + 12.0f, mousePosF.y - 12.0f}; crosshair[2].color = currentCrosshairColor;
        crosshair[3].position = {mousePosF.x - 12.0f, mousePosF.y + 12.0f}; crosshair[3].color = currentCrosshairColor;

        auto handleBtnStyle = [](UIButton& btn, sf::Vector2f mPos) {
            if (btn.rect.contains(mPos)) {
                btn.shape.setOutlineColor(sf::Color::Yellow);
                btn.shape.setOutlineThickness(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) ? 5.0f : 3.0f);
            } else {
                btn.shape.setOutlineColor(sf::Color::White);
                btn.shape.setOutlineThickness(2.0f);
            }
        };

        if (g_currentMode == Mode::Menu) {
            handleBtnStyle(btnPlay, mousePosF); handleBtnStyle(btnEdit, mousePosF);
            handleBtnStyle(btnSets, mousePosF); handleBtnStyle(btnExit, mousePosF);
        }

        while (std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    if (g_currentMode == Mode::Menu) {
                        if (btnPlay.rect.contains(mousePosF)) {
                            health = g_config.playerMaxHealth; score = 0; bullets.clear(); enemies.clear();
                            if (editorSpawns.empty()) editorSpawns.push_back(SpawnPoint({100.0f, 100.0f}, 0));
                            player.setPosition({960.0f, 540.0f});
                            g_currentMode = Mode::Playing;
                        }
                        else if (btnEdit.rect.contains(mousePosF)) g_currentMode = Mode::Editor;
                        else if (btnSets.rect.contains(mousePosF)) g_currentMode = Mode::Settings;
                        else if (btnExit.rect.contains(mousePosF)) window.close();
                    }
                    else if (g_currentMode == Mode::Settings) {
                        HandleSettingsClick(mousePosF);
                    }
                    else if (g_currentMode == Mode::Editor && mousePosF.y > 100.0f) {
                        editorSpawns.push_back(SpawnPoint(mousePosF, selectedEditorType));
                    }
                    else if (g_currentMode == Mode::Playing && health > 0) {
                        bullets.push_back(Bullet(player.getPosition(), mousePosF, false));
                    }
                }
            }
        }

        if (g_currentMode == Mode::Playing && health > 0 && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
            auto currentShotTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(currentShotTime - lastMiniganShot).count() > 66) {
                bullets.push_back(Bullet(player.getPosition(), mousePosF, true));
                lastMiniganShot = currentShotTime;
            }
        }

        if (g_currentMode == Mode::Playing && health > 0) {
            sf::Vector2f movement(0.0f, 0.0f);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) movement.x = -g_config.playerSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) movement.x = g_config.playerSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) movement.y = -g_config.playerSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) movement.y = g_config.playerSpeed;
            player.move(movement);

            sf::Vector2f pPos = player.getPosition();
            if (pPos.x < 25) player.setPosition({25, pPos.y}); if (pPos.x > 1895) player.setPosition({1895, pPos.y});
            if (pPos.y < 25) player.setPosition({pPos.x, 25}); if (pPos.y > 1055) player.setPosition({pPos.x, 1055});

            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSpawn).count() > 1200) {
                for (const auto& spawn : editorSpawns) enemies.push_back(Enemy(spawn.position, spawn.enemyType));
                lastSpawn = now;
            }

            for (size_t i = 0; i < bullets.size();) {
            bullets[i].shape.move(bullets[i].velocity);
   sf::Vector2f bPos = bullets[i].shape.getPosition();
   if (bPos.x < -30 || bPos.x > 1950 || bPos.y < -30 || bPos.y > 1110) bullets.erase(bullets.begin() + i);
   else i++;
}
   sf::FloatRect playerBounds = player.getGlobalBounds();
   for (size_t i = 0; i < enemies.size();) {
   enemies[i].update(player.getPosition());
   sf::FloatRect enemyBounds = enemies[i].shape.getGlobalBounds();
   if (playerBounds.findIntersection(enemyBounds)) {
   health -= (enemies[i].type == 2) ? g_config.enemyTankDamage : g_config.enemyDamage;
   if (health < 0) health = 0;
   enemies.erase(enemies.begin() + i);
   continue;
}
bool destroyed = false;
for (size_t j = 0; j < bullets.size(); j++) {
if (bullets[j].shape.getGlobalBounds().findIntersection(enemyBounds)) {
enemies[i].hp -= bullets[j].damage;
bullets.erase(bullets.begin() + j);
if (enemies[i].hp <= 0) {
score++;
enemies.erase(enemies.begin() + i);
destroyed = true;
}
break;
}
}
 if (!destroyed) i++;
 }
            healthBarInner.setSize({(static_cast<float>(health) / static_cast<float>(g_config.playerMaxHealth)) * 400.0f, 25.0f});
            healthBarInner.setFillColor(health < (g_config.playerMaxHealth * 0.4f) ? sf::Color::Red : sf::Color::Green);
}
else if (g_currentMode == Mode::Editor) {
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) selectedEditorType = 0;
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) selectedEditorType = 1;
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) selectedEditorType = 2;
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) editorSpawns.clear();
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) g_currentMode = Mode::Menu;
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape) && g_currentMode == Mode::Playing) g_currentMode = Mode::Menu;
window.clear(sf::Color(5, 7, 12));
if (g_currentMode == Mode::Menu) {
window.draw(btnPlay.shape); window.draw(btnPlay.shape);
window.draw(btnEdit.shape); window.draw(btnEdit.shape);
window.draw(btnSets.shape); window.draw(btnSets.shape);
window.draw(btnExit.shape); window.draw(btnExit.shape);
}
else if (g_currentMode == Mode::Settings) {
RenderSettingsUI(window, mousePosF);
}
else if (g_currentMode == Mode::Editor) {
sf::RectangleShape topBar({1920.0f, 80.0f}); topBar.setFillColor(sf::Color(25, 30, 40)); window.draw(topBar);
sf::RectangleShape indicator({30.0f, 30.0f});
if (selectedEditorType == 0) indicator.setFillColor(sf::Color(220, 80, 80));
else if (selectedEditorType == 1) indicator.setFillColor(sf::Color::Red);
else indicator.setFillColor(sf::Color(139, 0, 139));
indicator.setPosition({50.0f, 25.0f}); window.draw(indicator);
for (const auto& spawn : editorSpawns) window.draw(spawn.shape);
}
else if (g_currentMode == Mode::Playing) {
for (auto& b : bullets) window.draw(b.shape);
for (auto& e : enemies) window.draw(e.shape);
window.draw(player); window.draw(healthBarInner); window.draw(healthBarOuter);
for (int k = 0; k < score; k++) {
scoreIndicator.setPosition({1850.0f - (k * 22.0f), 50.0f});
window.draw(scoreIndicator);
}
if (health <= 0) {
window.clear(sf::Color(50, 5, 5));
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) g_currentMode = Mode::Menu;
}
}
window.draw(crosshair);
window.display();
}
return 0;
}
