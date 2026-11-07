#ifndef MAIN_H
#define MAIN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <string>

enum class Mode { Menu, Editor, Settings, Playing };

struct GameConfig {
    unsigned int windowWidth = 1920;
    unsigned int windowHeight = 1080;
    unsigned int fpsLimit = 60;
    int playerMaxHealth = 100;
    float playerSpeed = 5.5f;
    int bulletDamage = 25;
    int miniganDamage = 10;
    int enemyBaseHealth = 25;
    int enemyTankHealth = 100;
    int enemyDamage = 10;
    int enemyTankDamage = 30;
};

struct UIButton {
    sf::FloatRect rect;
    sf::RectangleShape shape;
    sf::Text text;
    int id;

    
    UIButton(float x, float y, float w, float h, int buttonId, sf::Color baseColor, const std::string& label, const sf::Font& font) 
        : text(font) 
    {
        rect.position = {x, y};
        rect.size = {w, h};
        
        shape.setSize({w, h});
        shape.setPosition({x, y});
        shape.setFillColor(baseColor);
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(2.0f);
        
        text.setString(label);
        text.setCharacterSize(22);
        text.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition({x + (w - textBounds.size.x) / 2.0f, y + (h - textBounds.size.y) / 2.0f - 5.0f});
        
        id = buttonId;
    }
};

extern GameConfig g_config;
extern Mode g_currentMode;
extern std::chrono::high_resolution_clock::time_point g_appStartTime;

std::string GetAppUptime();
void RenderSettingsUI(sf::RenderWindow& window, sf::Vector2f mousePos);
void HandleSettingsClick(sf::Vector2f mousePos);

#endif
