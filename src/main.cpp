#include <iostream>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

struct CircleData
{
    float r, x, y, sx, sy;
};

struct Point
{
    float x, y;
};

void checkCollisions(std::vector<CircleData> &circles, float windowX, float windowY)
{
    for (auto &c : circles)
    {
        // circle bounding points
        Point topLeft = {c.x, c.y};                                 // x, y
        Point topRight = {(c.x + 2 * (c.r)), c.y};                  // x+2r, y
        Point bottomLeft = {c.x, (c.y + 2 * (c.r))};                // x,y+2r
        Point bottomRight = {(c.x + 2 * (c.r)), (c.y + 2 * (c.r))}; // x+2r, y+2r

        // collision on x axis
        if (topLeft.x <= 0)
        {
            c.sx = -1 * c.sx;
        }
        if (topRight.x >= windowX)
        {
            c.sx = -1 * c.sx;
        }

        // collision on y axis
        if (topLeft.y <= 0 || topRight.y <= 0)
        {
            c.sy = -1 * c.sy;
        }
        if (bottomLeft.y >= windowY || bottomRight.y >= windowY)
        {
            c.sy = -1 * c.sy;
        }
    }
}

void drawCircle(sf::RenderWindow &window, float radius, float x, float y)
{
    sf::CircleShape circle(radius);
    circle.setPosition(x, y);
    circle.setFillColor(sf::Color::Green);
    window.draw(circle);
}

int main(int argc, char *argv[])
{

    // create a new window of size w*h. Reade from config file.
    const int wHeight = 720;
    const int wWidth = 1280;
    sf::RenderWindow window(sf::VideoMode({wWidth, wHeight}), "Seap Engine");
    window.setFramerateLimit(60);

    // init imgui - needs a clock for internal timing
    if (!ImGui::SFML::Init(window))
    {
        std::cout << "Could not init windows" << std::endl;
        return -1;
    }
    sf::Clock deltaClock;

    // scale imgui ui and text size by 2
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    // for the circle
    std::vector<CircleData> circles;
    float radius = 10.f;
    float posX = 100.0f;
    float posY = 100.0f;
    float speedX = 10.f;
    float speedY = 10.f;

    // Main Loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        // update imgui
        ImGui::SFML::Update(window, deltaClock.restart());

        // imgui window to spawn circle
        ImGui::Begin("Cirlce Spawner");
        ImGui::InputFloat("Radius", &radius);
        ImGui::InputFloat("X", &posX);
        ImGui::InputFloat("Y", &posY);
        ImGui::InputFloat("Speed X", &speedX);
        ImGui::InputFloat("Speed Y", &speedY);
        if (ImGui::Button("Spawn Circle"))
        {
            circles.push_back({radius, posX, posY, speedX, speedY});
        }
        if (ImGui::Button("Undo Last Spawn"))
        {
            if (circles.size() != 0)
            {
                circles.pop_back();
            }
            else
            {
                ImGui::Text("Window Empty");
            }
        }
        ImGui::End();

        // clear sfml window
        window.clear(sf::Color(50, 50, 50));

        // update positions
        for (auto &c : circles)
        {
            c.x += c.sx;
            c.y += c.sy;
        }

        // collision check
        checkCollisions(circles, wWidth, wHeight);

        // draw
        for (auto &c : circles)
        {
            drawCircle(window, c.r, c.x, c.y);
        }

        // draw imgui
        ImGui::SFML::Render(window);

        // display everything
        window.display();
    }

    // shutdown imgui-sfml
    ImGui::SFML::Shutdown();

    return 0;
}