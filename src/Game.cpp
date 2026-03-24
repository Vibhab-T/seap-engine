#include "Game.h"

#include "imgui.h"
#include "imgui-SFML.h"

#include <iostream>
#include <cmath>

Game::Game(const std::string &config) // the constructor, init() is called here. the run() is called from main.cpp
{
    init(config);
}

void Game::init(const std::string &path)
{
    // TODO: read the config file here

    m_window.create(sf::VideoMode(1920, 1080), "Geo Wars");
    m_window.setFramerateLimit(60);
    ImGui::SFML::Init(m_window);
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf", 20.0f);
    ImGui::SFML::UpdateFontTexture();

    spawnPlayer();
}

void Game::run()
{
    sf::Clock deltaClockForImGui;

    while (m_running && m_window.isOpen())
    {
        sUserInput(); // always handle input
        ImGui::SFML::Update(m_window, deltaClockForImGui.restart());

        if (!m_paused)
        {
            m_entities.update(); // the update method of the Enitity Manager.
            sEnemySpawner();
            sMovement();
            sCollision();
            sLifespan();
            m_currentFrame++;
        }

        sUiSystem();
        sRender(); // always render
    }

    ImGui::SFML::Shutdown();
    m_window.close();
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    // finish adding all properties of player from the config - currently haaaard coded
    float playerRadius = 32.0f;

    // every entity is creatyed by calling the entity manager's add entity with a tag
    // return std::shared_ptr<Entity>, auto to save typing
    auto entity = m_entities.addEntity("player");

    // give it a transform so it spawn at 200, 200, with velocity 1,1 and angle 0
    entity->cTransform = std::make_shared<CTransform>(Vec2(200.0f, 200.0f), Vec2(1.0f, 1.0f), 0.0f);

    // give it a shape component as well - read from config file
    entity->cShape = std::make_shared<CShape>(playerRadius, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

    // input component so moves
    entity->cInput = std::make_shared<CInput>();

    // collision shape - read from .seap, same as playerRadius rn
    entity->cCollision = std::make_shared<CCollision>(playerRadius);

    // Game's player variable should be set to this, as this is the entity we want to be our player.
    m_player = entity;
}

void Game::spawnEnemy()
{
    // make sure the enemy is spawned with the config variables
    //  within the bounds of the window
    float enemyRadius = 32.0f;
    float x = enemyRadius + (std::rand() % (int)(m_window.getSize().x - 2 * enemyRadius));
    float y = enemyRadius + (std::rand() % (int)(m_window.getSize().y - 2 * enemyRadius));
    Vec2 spawnPosition(x, y);

    // take the direction to the player from the spawnPosition
    // assign velocity accordingly
    // velocity calc done same as bullet velocity calculation
    float enemySpeed = 5.0f;
    Vec2 directionVector = m_player->cTransform->pos - spawnPosition; // distance = b - a
    float directionVectorLength = directionVector.length();
    Vec2 velocityOfEnemy = directionVector / directionVectorLength * enemySpeed;

    // randomize vertices between 3 and 8
    int vertices = 3 + (std::rand() % 6);

    // randomize enemy color
    sf::Color fill(std::rand() % 256, std::rand() % 256, std::rand() % 256);
    sf::Color outline(255, 255, 255); // read from .seap file - currently hardcoded

    auto entity = m_entities.addEntity("enemy");
    entity->cTransform = std::make_shared<CTransform>(spawnPosition, velocityOfEnemy, 0.0f);
    entity->cShape = std::make_shared<CShape>(enemyRadius, vertices, fill, outline, 3.0f);

    // the collision radius as well - read from .seap, currently its the same as shape radius
    entity->cCollision = std::make_shared<CCollision>(enemyRadius);

    // points!
    entity->cScore = std::make_shared<CScore>(100);

    // record when the latest enemy is spawned
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    // dont spawn if the parent entity already has a lifespan component that means it was already a small enemy
    if (e->cLifespan)
    {
        return;
    }
    // spawn small enemies at the location of the input enemy e

    // read the values of the original enemy
    // spawn the same amount as og's vertices
    // same color
    // double points

    int vertices = e->cShape->circle.getPointCount();
    sf::Color fillColor = e->cShape->circle.getFillColor();
    sf::Color outlineColor = e->cShape->circle.getOutlineColor();
    Vec2 positionToSpawn = e->cTransform->pos;
    float smallEnemyRadius = e->cCollision->radius / 2.0f;
    float speed = 4.0f;
    int scoreValue = e->cScore->score * 2;

    // spread it evenly around the original's vertices
    // full circle is 2pi. 2pi/vertices = equal angle steps
    float angleStep = (2.0f * 3.14159265f) / vertices;

    for (int i = 0; i < vertices; i++)
    {
        float angle = i * angleStep;

        // cos and sine fives x and y.
        // cos, sin is always a unit vector towards the angle. since unit vector, multiply by speed
        Vec2 velocity(std::cos(angle) * speed, std::sin(angle) * speed);

        auto small = m_entities.addEntity("enemy");
        small->cTransform = std::make_shared<CTransform>(positionToSpawn, velocity, 0.0f);
        small->cShape = std::make_shared<CShape>(smallEnemyRadius, vertices, fillColor, outlineColor, 2.0f);
        small->cCollision = std::make_shared<CCollision>(smallEnemyRadius);
        small->cScore = std::make_shared<CScore>(scoreValue);
        small->cLifespan = std::make_shared<CLifespan>(90); // 1.5 seconds at 60fps
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2 &target)
{
    float bulletSpeed = 20.0f;

    // spawnbullet which travels towards the target entity
    // speed is given as scalar speed in config
    // implement velocity
    Vec2 currentPlayerPosition = m_player->cTransform->pos;
    // target position = target.x, target. y

    // find the direction vector
    // normalize it by finding the unit vector in that direction
    // multiply the unit vector with speed to get the velocity for the bullet with.
    Vec2 directionVector = target - currentPlayerPosition;
    float lengthOfDirectionVector = directionVector.length();
    Vec2 velocityOfBullet = directionVector / lengthOfDirectionVector * bulletSpeed; // (dir/len) is finding the unit vector * bullet speed gives x and y of velocity.

    auto bulletEntity = m_entities.addEntity("bullet");

    bulletEntity->cTransform = std::make_shared<CTransform>(currentPlayerPosition, velocityOfBullet, 0);
    bulletEntity->cShape = std::make_shared<CShape>(10, 32, sf::Color(0, 0, 255), sf::Color(0, 0, 0), 3);
    bulletEntity->cCollision = std::make_shared<CCollision>(10.0f);
    bulletEntity->cLifespan = std::make_shared<CLifespan>(50);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    // wooooo
}

void Game::sMovement()
{
    // PLAYER MOVEMENT
    m_player->cTransform->velocity = {0.0, 0.0};

    // read m_player->cInput and implement movement
    if (m_player->cInput->up)
    {
        m_player->cTransform->velocity.y = -5;
    }

    if (m_player->cInput->down)
    {
        m_player->cTransform->velocity.y = 5;
    }

    if (m_player->cInput->right)
    {
        m_player->cTransform->velocity.x = 5;
    }

    if (m_player->cInput->left)
    {
        m_player->cTransform->velocity.x = -5;
    }

    // and then update
    // m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    // m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    // Update positions for other entities based on their velocity as well
    for (auto &e : m_entities.getEntities())
    {
        e->cTransform->pos += e->cTransform->velocity;
    }
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(m_window, event);

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
            case sf::Keyboard::P:
                std::cout << "P Key Pressed\n";
                setPaused(!m_paused);
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

        // fire mouse events if ImGui isnt consuming it
        if (!ImGui::GetIO().WantCaptureMouse)
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "Left Mouse Clicked At: " << event.mouseButton.x << ", " << event.mouseButton.y << std::endl;

                    spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
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

    auto allEntities = m_entities.getEntities();

    for (auto &e : allEntities)
    {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        e->cTransform->angle += 1.5f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        m_window.draw(e->cShape->circle);
    }
    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::sEnemySpawner()
{

    // spawn an enemy at a random point in the window
    // at regular interval every 3-4 seconds i.e 3*60 = 180 frames.
    // for enemy's movement, add velocity CTransform by calculating velocity from point of spawn to player at that moment
    //  add cooldown logic

    if ((m_currentFrame - m_lastEnemySpawnTime) >= 200)
    {
        spawnEnemy();
    }
}

void Game::sCollision()
{
    auto &bullets = m_entities.getEntities("bullet");
    auto &enemies = m_entities.getEntities("enemy");

    // bullet vs enemy
    for (auto &bullet : bullets)
    {
        for (auto &enemy : enemies)
        {
            float distance = bullet->cTransform->pos.dist(enemy->cTransform->pos); // distance between the origins of the entities
            float sumOfRadius = bullet->cCollision->radius + enemy->cCollision->radius;

            if (distance < sumOfRadius) // THOKKYOOO
            {
                m_score += enemy->cScore->score;
                spawnSmallEnemies(enemy);
                bullet->destroy();
                enemy->destroy();
            }
        }
    }

    // player vs enemy
    for (auto &enemy : enemies)
    {
        float distanceBetween = m_player->cTransform->pos.dist(enemy->cTransform->pos);
        float sumOfRadius = m_player->cCollision->radius + enemy->cCollision->radius;

        if (distanceBetween < sumOfRadius)
        {
            enemy->destroy();
            m_lives--;

            if (m_lives <= 0)
            {
                m_gameOver = true;
                setPaused(true);
            }
            else
            {
                // respawn player at center - just move the player
                m_player->cTransform->pos = Vec2(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);
            }
        }
    }

    // player vs window bounds
    Vec2 &playerPosition = m_player->cTransform->pos;
    float playerCollisionRadius = m_player->cCollision->radius;

    if (playerPosition.x - playerCollisionRadius < 0)
        playerPosition.x = playerCollisionRadius;
    if (playerPosition.y - playerCollisionRadius < 0)
        playerPosition.y = playerCollisionRadius;
    if (playerPosition.x + playerCollisionRadius > m_window.getSize().x)
        playerPosition.x = m_window.getSize().x - playerCollisionRadius;
    if (playerPosition.y + playerCollisionRadius > m_window.getSize().y)
        playerPosition.y = m_window.getSize().y - playerCollisionRadius;

    // bullet vs window - destroy bullet when out of bounds
    for (auto &bullet : bullets)
    {
        auto &bulletPosition = bullet->cTransform->pos;
        if (bulletPosition.x < 0 || bulletPosition.x > m_window.getSize().x || bulletPosition.y < 0 || bulletPosition.y > m_window.getSize().y)
            bullet->destroy();
    }
}

void Game::sLifespan()
{
    for (auto &e : m_entities.getEntities())
    {
        if (!e->cLifespan)
            continue;

        if (e->cLifespan->remaining <= 0)
        {
            e->destroy();
            continue;
        }

        e->cLifespan->remaining--;

        // fade colors proportionally to remaining lifespan
        float ratio = (float)e->cLifespan->remaining / (float)e->cLifespan->total;
        auto fill = e->cShape->circle.getFillColor();
        auto outline = e->cShape->circle.getOutlineColor();

        fill.a = (sf::Uint8)(255 * ratio);
        outline.a = (sf::Uint8)(255 * ratio);

        e->cShape->circle.setFillColor(fill);
        e->cShape->circle.setOutlineColor(outline);
    }
}

void Game::sUiSystem()
{

    // pin the window to top left
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(250, 0), ImGuiCond_Always);

    ImGui::Begin("##hud", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImGui::SetWindowFontScale(1.4f);
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Score: %d", m_score);
    ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Lives: %d", m_lives);

    ImGui::Spacing();

    if (!m_gameOver)
    {
        if (ImGui::Button(m_paused ? "Resume" : "Pause", ImVec2(110, 40)))
            setPaused(!m_paused);
        ImGui::SameLine();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Exit", ImVec2(110, 40)))
        m_running = false;
    ImGui::PopStyleColor(2);
    ImGui::End();

    // game over screen
    if (!m_gameOver)
    {
        return;
    }

    ImVec2 center(m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_Always);
    ImGui::Begin("##gameover", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SetWindowFontScale(1.4f);
    ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "GAME OVER");
    ImGui::Text("Final Score: %d", m_score);
    ImGui::Spacing();

    if (ImGui::Button("Play Again", ImVec2(150, 40)))
        resetGame();
    ImGui::SameLine();
    if (ImGui::Button("Exit", ImVec2(150, 40)))
        m_running = false;

    ImGui::End();
}

void Game::resetGame()
{
    // destroy all entites
    for (auto &e : m_entities.getEntities())
        e->destroy();

    m_score = 0;
    m_lives = 5;
    m_currentFrame = 0;
    m_lastEnemySpawnTime = 0;
    m_gameOver = false;
    m_paused = false;

    spawnPlayer();
}