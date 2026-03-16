#include "Game.h"
#include <iostream>

Game::Game(const std::string &config)
{
    init(config);
}

void Game::init(const std::string &path)
{
    // TODO: read the config file here

    m_window.create(sf::VideoMode(1280, 720), "Geo Wars");
    m_window.setFramerateLimit(60);

    spawnPlayer();
}

void Game::run()
{
    // TODO: add pause functionality here

    while (m_running)
    {
        m_entities.update();

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sRender();

        m_currentFrame++; // should be outside when paused
    }
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    // finish adding all properties of player from the config - currently haaaard coded

    // every entity is creatyed by calling the entity manager's add entity with a tag
    // return std::shared_ptr<Entity>, auto to save typing
    auto entity = m_entities.addEntity("player");

    // give it a transform so it spawn at 200, 200, with velocity 1,1 and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), 0.0f);

    // give it a shape component as well - read from config file
    entity->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // input component so moves
    entity->cInput = std::make_shared<CInput>();

    // Game's player variable should be set to this, as this is the entity we want to be our player.
    m_player = entity;
}

void Game::spawnEnemy()
{
    // make sure the enemy is spawned with the config variables
    //  within the bounds of the window

    // record when the latest enemy is spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    // spawn small enemies at the location of the input enemy e

    // read the values of the original enemy
    // spawn the same amount as og's vertices
    // same color
    // double points
}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2 &target)
{
    // spawnbullet which travels towards the target entity
    // speed is given as scalar speed in config
    // implement velocity
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // wooooo
}

void Game::sMovement()
{
    // read m_player->cInput and implement movement
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                std::cout << "W Key Pressed\n";
                m_player->cInput->up = true;
                break;
            case sf::Keyboard::A:
                std::cout << "A Key Pressed\n";
                m_player->cInput->left = true;
                break;
            case sf::Keyboard::S:
                std::cout << "S Key Pressed\n";
                m_player->cInput->down = true;
                break;
            case sf::Keyboard::D:
                std::cout << "D Key Pressed\n";
                m_player->cInput->right = true;
                break;
            default:
                break;
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                std::cout << "W Key Released\n";
                m_player->cInput->up = false;
                break;
            case sf::Keyboard::A:
                std::cout << "A Key Released\n";
                m_player->cInput->left = false;
                break;
            case sf::Keyboard::S:
                std::cout << "S Key Released\n";
                m_player->cInput->down = false;
                break;
            case sf::Keyboard::D:
                std::cout << "D Key Released\n";
                m_player->cInput->right = false;
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                std::cout << "Left Mouse Clicked At: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;

                // call spawnBulletHere
            }
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "Right Mouse Clicked At: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;

                // call specialWeaponHere
            }
        }
    }
}

void Game::sRender()
{
    m_window.clear();

    // set the position of the shape based on the entity's transform
    m_player->cShape->circle.setPosition(m_player->cTransform->pos.x, m_player->cTransform->pos.y);

    // set the rotation of the shape based on the entity's transform->angle
    m_player->cTransform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTransform->angle);

    // draw the entitys circle shape
    m_window.draw(m_player->cShape->circle);

    m_window.display();
}

void Game::sEnemySpawner()
{

    // add cooldown logic and all and call
    spawnEnemy();
}