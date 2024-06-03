#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "textures.cpp"
#include "ui.cpp"

sf::Sprite getSprite(int id, int x, int y,float z) {
    sf::Sprite output;
    output.setTexture(textures[id]);
    output.setPosition(sf::Vector2f(x,y));
    output.setScale(sf::Vector2f(0.6*z,0.6*z));
    return output;
}

struct block {
    int x;
    int y;
    int id;
};

bool paused=false;

bool saveLevel(std::string levelName,block blocks[80000]) {
    std::ofstream save(std::string("saves/").append(levelName));
    std::string data="";

    for (int b=0; b<80000; b++) {
        if (blocks[b].id>0) {
            std::string all = "";
            std::string id = std::string(":").append(std::to_string(blocks[b].id));
            std::string x = std::string(";").append(std::to_string(blocks[b].x));
            std::string y = std::string(";").append(std::to_string(blocks[b].y));
            all.append(id).append(x).append(y);
            data.append(all);
        } else { continue; }
    }

    save << data;
    return true;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "GDedit");

    // clocks
    sf::Clock fpsClock;

    // load all fonts
    sf::Font roboto;
    if (!roboto.loadFromFile("assets/fonts/roboto.ttf")) { std::cout << "could not load font file 'roboto.ttf'.\n"; return -1; }
    
    sf::Sprite bg;

    sf::Vector2i cameraPosition;

    // camera
    sf::Vector2i oldmp;
    sf::Vector2i cpOnClick;
    bool dragging=false;
    float zoom=1;
    int nobjid=1;

    block blocks[80000];

    for (int b=0; b<80000;b++) {
        blocks[b].id=6;
        blocks[b].x=0;
        blocks[b].y=0;
    }

    int rendered=0;

    initializeTextures();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Button::Left) {
                oldmp=sf::Mouse::getPosition();
                cpOnClick=cameraPosition;
                dragging=true;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Left) {
                dragging=false;
            }
            if (event.type==sf::Event::KeyPressed) {
                if (event.key.code==sf::Keyboard::Key::Escape) {
                    paused=true;
                }
                if (event.key.code == sf::Keyboard::Key::S) {
                    saveLevel("test",blocks);
                }
            }

           if (event.type==sf::Event::KeyReleased) {
                if (event.key.code==sf::Keyboard::Key::Escape) {
                    paused=false;
                }
            }

            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta>0) {

                } else if (event.mouseWheelScroll.delta<0) {

                }
            }
        }

        if (dragging && !paused) {
            sf::Vector2i cmp = sf::Mouse::getPosition();
            cameraPosition.x = cpOnClick.x+(cmp.x-oldmp.x);
            cameraPosition.y = cpOnClick.y+(cmp.y-oldmp.y);
        }

        window.clear(sf::Color(40,125,255,255));

        if (paused) {
            continue;
        }

        for (int b=0; b<80000; b++) {
            if (blocks[b].x*(30*zoom)+cameraPosition.x>1280+(30*zoom) || blocks[b].x*(30*zoom)+cameraPosition.x<0-(30*zoom)) {
                continue;
            }
            if (blocks[b].y*(30*zoom)+cameraPosition.y>720+(30*zoom) || blocks[b].y*(30*zoom)+cameraPosition.y<0-(30*zoom)) {
                continue;
            }
            if (blocks[b].id>0) {
                window.draw(getSprite(blocks[b].id,blocks[b].x+cameraPosition.x,blocks[b].y+cameraPosition.y,zoom));
                rendered++;
            }
        }

        float fps = 1 / fpsClock.getElapsedTime().asSeconds();

        sf::Text tbf;
        tbf.setCharacterSize(18);
        tbf.setFillColor(sf::Color::White);
        tbf.setFont(roboto);
        tbf.setString(std::string("fps: ").append(std::to_string((int)fps)));
        window.draw(tbf);

        sf::Text rText;
        rText.setCharacterSize(18);
        rText.setString(std::string("rendered: ").append(std::to_string(rendered)));
        rText.setFont(roboto);
        rText.setPosition(0,20);

        window.draw(rText);

        fpsClock.restart().asSeconds();

        window.display();
        rendered=0;
    }

    return 0;
}