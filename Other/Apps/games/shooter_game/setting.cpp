#include "main.h"

GameConfig g_config;

std::string GetAppUptime() {
    auto now = std::chrono::high_resolution_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - g_appStartTime).count();
    long h = seconds / 3600;
    long m = (seconds % 3600) / 60;
    long s = seconds % 60;
    char buf[32];
    snprintf(buf, sizeof(buf), "%02ld:%02ld:%02ld", h, m, s);
    return std::string(buf);
}

static sf::RectangleShape btnFPS30({220.0f, 50.0f});
static sf::RectangleShape btnFPS60({220.0f, 50.0f});
static sf::RectangleShape btnHP100({220.0f, 50.0f});
static sf::RectangleShape btnHP500({220.0f, 50.0f});
static sf::RectangleShape btnBack({220.0f, 50.0f});

void RenderSettingsUI(sf::RenderWindow& window, sf::Vector2f mousePos) {
    btnFPS30.setPosition({400.0f, 300.0f});
    btnFPS60.setPosition({700.0f, 300.0f});
    btnHP100.setPosition({400.0f, 450.0f});
    btnHP500.setPosition({700.0f, 450.0f});
    btnBack.setPosition({550.0f, 650.0f});

    auto applyStyle = [](sf::RectangleShape& btn, sf::Vector2f mPos, sf::Color baseColor) {
        btn.setFillColor(baseColor);
        btn.setOutlineThickness(2.0f);
        if (btn.getGlobalBounds().contains(mPos)) {
            btn.setOutlineColor(sf::Color::Yellow);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) btn.setOutlineThickness(4.0f);
        } else {
            btn.setOutlineColor(sf::Color::White);
        }
    };

    applyStyle(btnFPS30, mousePos, g_config.fpsLimit == 30 ? sf::Color(0,180,180) : sf::Color(40,45,55));
    applyStyle(btnFPS60, mousePos, g_config.fpsLimit == 60 ? sf::Color(0,180,180) : sf::Color(40,45,55));
    applyStyle(btnHP100, mousePos, g_config.playerMaxHealth == 100 ? sf::Color(0,150,0) : sf::Color(40,45,55));
    applyStyle(btnHP500, mousePos, g_config.playerMaxHealth == 500 ? sf::Color(0,150,0) : sf::Color(40,45,55));
    applyStyle(btnBack, mousePos, sf::Color(100,20,20));

    window.draw(btnFPS30);
    window.draw(btnFPS60);
    window.draw(btnHP100);
    window.draw(btnHP500);
    window.draw(btnBack);

    sf::RectangleShape uptimeRadar({40.0f, 10.0f});
    uptimeRadar.setPosition({30.0f, 30.0f});
    uptimeRadar.setFillColor((std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - g_appStartTime).count() % 2 == 0) ? sf::Color::Cyan : sf::Color::Blue);
    window.draw(uptimeRadar);
}

void HandleSettingsClick(sf::Vector2f mousePos) {
    if (btnFPS30.getGlobalBounds().contains(mousePos)) g_config.fpsLimit = 30;
    if (btnFPS60.getGlobalBounds().contains(mousePos)) g_config.fpsLimit = 60;
    if (btnHP100.getGlobalBounds().contains(mousePos)) g_config.playerMaxHealth = 100;
    if (btnHP500.getGlobalBounds().contains(mousePos)) g_config.playerMaxHealth = 500;
    if (btnBack.getGlobalBounds().contains(mousePos)) g_currentMode = Mode::Menu;
}
