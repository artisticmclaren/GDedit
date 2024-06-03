#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "textures.cpp"
#include "ui.cpp"

using namespace std;

sf::Sprite getSprite(int id, int x, int y,float z) {
    sf::Sprite output;
    output.setTexture(textures[id]);
    output.setPosition(sf::Vector2f(x,y));
    output.setScale(sf::Vector2f(0.6*z,0.6*z));
    return output;
}

bool stopHold=false;

struct block {
    int x;
    int y;
    int id;
};

bool paused=false;
int objectCount=0;

sf::Vector2i roundPositions(sf::Vector2i pos) {
    int xr = pos.x % 30;
    int yr = pos.y % 30;

    sf::Vector2i output;

    if (xr == 0) {
        output.x = xr;
    } else {
        output.x = pos.x  - xr;
    }

    if (yr == 0) {
        output.y = pos.y;
    } else {
        output.y = pos.y - yr;
    }

    return output;
}

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

void placeObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition,int nobjid, block blocks[80000]) {
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x-cameraPosition.x;
    mpcp.y = mousePosition.y-cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    blocks[objectCount].id = nobjid;
    blocks[objectCount].x=rounded.x;
    blocks[objectCount].y=rounded.y;
    objectCount++;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "GDedit");

    auto icon = sf::Image();
    icon.loadFromFile("assets/icon.png");
    window.setIcon(icon.getSize().x,icon.getSize().y,icon.getPixelsPtr());

    // clocks
    sf::Clock fpsClock;
    initializeUI();

    // load all fonts
    sf::Font roboto;
    if (!roboto.loadFromFile("assets/fonts/roboto.ttf")) { std::cout << "could not load font file 'roboto.ttf'.\n"; return -1; }
    
    sf::Sprite bg;
    
    // define ui
    Button saveLevelbtn(1000,650,1,0.25,"Save Level");

    sf::Vector2i cameraPosition;
    sf::Vector2i mousePosition;

    // camera
    sf::Vector2i oldmp;
    sf::Vector2i cpOnClick;
    bool dragging=false;
    float zoom=1;
    int nobjid=1;

    block blocks[80000];

    for (int b=0; b<80000;b++) {
        blocks[b].id=0;
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
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Button::Right) {
                    oldmp=sf::Mouse::getPosition();
                    cpOnClick=cameraPosition;
                    dragging=true;
                }

                if (event.mouseButton.button == sf::Mouse::Button::Left) {
                    placeObject(mousePosition,cameraPosition,nobjid,blocks);
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right) {
                dragging=false;
            }
            if (event.type==sf::Event::KeyPressed) {
                if (event.key.code==sf::Keyboard::Key::Escape) {
                    paused=true;
                }
                if (event.key.code==sf::Keyboard::Key::S) {
                    saveLevel("test",blocks);
                }

                if (event.key.code==sf::Keyboard::Key::Up) {
                    nobjid++;
                }
                if (event.key.code==sf::Keyboard::Key::Down) {
                    nobjid--;
                }
            }

            if (event.type==sf::Event::KeyReleased) {
                if (event.key.code==sf::Keyboard::Key::Escape) {
                    paused=false;
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

        sf::FloatRect visibleArea(
            0-30,
            0-30,
            window.getSize().x+30,
            window.getSize().y+30);

        for (int b=0; b<80000; b++) {
            sf::Vector2f objectPosition(blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y);

            if (blocks[b].id==0) {
                continue;
            }
            if (!visibleArea.contains(objectPosition)) {
                continue;
            }

            window.draw(getSprite(blocks[b].id,blocks[b].x+cameraPosition.x,blocks[b].y+cameraPosition.y,zoom));
            rendered++;
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

        sf::Text mpos;
        mpos.setCharacterSize(18);
        mpos.setString(std::string("nobjid: ").append(std::to_string(nobjid)));
        mpos.setFont(roboto);
        mpos.setPosition(0,40);
        window.draw(mpos);
        

        fpsClock.restart().asSeconds();

        mousePosition = sf::Mouse::getPosition(window);
        // display UI
        completeButton slc = saveLevelbtn.draw();
        window.draw(slc.sprite);
        window.draw(slc.label);

        window.display();
        rendered=0;
    }

    return 0;
}