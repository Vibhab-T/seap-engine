#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

struct PlayerConfig
{
    int sr, cr, fr, fg, fb, otr, otg, otb, ott, v;
    float s;
};

struct EnemyConfig
{
    int sr, cr, otr, otg, otb, ott, vmin, vmax, l, si;
    float smin, smax;
};

struct BulletConfig
{
    int sr, cr, fr, fg, fb, otr, otg, otb, ott, v, l;
    float s;
};

class Game
{
    sf::RenderWindow m_window; // the window to draw to
    EntityManager m_entities;  // vector of entities to maintain
    sf::Font m_font;
    sf::Text m_text;
    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;

    int m_score = 0;
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;
    int m_lives = 5;

    bool m_paused = false;
    bool m_gameOver = false;
    bool m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string &config);
    void setPaused(bool paused);

    void sMovement();     // the movement system
    void sUserInput();    // the input system
    void sLifespan();     // enemy lifespan system
    void sRender();       // renderer system
    void sCollision();    // collision system
    void sEnemySpawner(); // enemy spawner system
    void sUiSystem();     // UI system

    void resetGame();
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 &mousePosition);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:
    Game(const std::string &config);
    void run();
};
