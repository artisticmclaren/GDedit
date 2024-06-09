#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "textures.cpp"
#include "ui.cpp"

using namespace std;

sf::Sprite getSprite(int id, int x, int y, float z)
{
    sf::Sprite output;
    output.setTexture(textures[id]);
    output.setPosition(sf::Vector2f(x, y));
    output.setScale(sf::Vector2f(0.6 * z, 0.6 * z));
    return output;
}

bool stopHold = false;

struct block
{
    int x;
    int y;
    int id;
};

bool paused = false;


int objectCount = 0;

sf::Vector2i roundPositions(sf::Vector2i pos)
{
    int xr = pos.x % 30;
    int yr = pos.y % 30;

    sf::Vector2i output;

    if (xr == 0)
    {
        output.x = xr;
    }
    else
    {
        output.x = pos.x - xr;
    }

    if (yr == 0)
    {
        output.y = pos.y;
    }
    else
    {
        output.y = pos.y - yr;
    }

    return output;
}

std::string reason; // latest reason for function failure

bool saveLevel(std::string levelName, block blocks[80000])
{
    std::ofstream save(std::string("saves/").append(levelName));
    std::string data = "";

	if (std::filesystem::is_directory(std::string("saves"))==false) {
		reason=std::string("directory 'saves' does not exist.");
		return false;
	}

    for (int b = 0; b < 80000; b++)
    {
        if (blocks[b].id > 0)
        {
            std::string all = "";
            std::string id = std::string(":").append(std::to_string(blocks[b].id));
            std::string x = std::string(";").append(std::to_string(blocks[b].x));
            std::string y = std::string(";").append(std::to_string(blocks[b].y));
            all.append(id).append(x).append(y);
            data.append(all);
        }
        else
        {
            continue;
        }
    }

    save << data;
    return true;
}

void placeObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, int nobjid, block blocks[80000])
{
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;
    blocks[objectCount].id = nobjid;
    blocks[objectCount].x = rounded.x;
    blocks[objectCount].y = rounded.y;
    objectCount++;
}

bool clicked;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "GDedit");

    auto icon = sf::Image();
    icon.loadFromFile("assets/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // clocks
    sf::Clock fpsClock;
    initializeUI();

    sf::Sprite ground;
    sf::Sprite ground2;
    sf::Sprite ground3;
    sf::Sprite ground4;
    sf::Texture groundTex;



    groundTex.loadFromFile("assets/grounds/groundSquare_01_001-uhd.png");
    ground.setTexture(groundTex);
    ground.setPosition(0,680);
    ground.setColor(sf::Color(40, 125, 255, 255));
    ground2.setTexture(groundTex);
    ground2.setPosition(512,680);
    ground2.setColor(sf::Color(40, 125, 255, 255));
    ground3.setTexture(groundTex);
    ground3.setPosition(1024,680);
    ground3.setColor(sf::Color(40, 125, 255, 255));
    ground4.setPosition(1536,680);
    ground4.setTexture(groundTex);
    ground4.setColor(sf::Color(40, 125, 255, 255));
    
    int groundlc=0;

    // load all fonts
    sf::Font roboto;
    if (!roboto.loadFromFile("assets/fonts/roboto.ttf"))
    {
        std::cout << "could not load font file 'roboto.ttf'.\n";
        return -1;
    }

    sf::RectangleShape line;
    line.setSize(sf::Vector2f(5,1280));
    line.setPosition(0,0);

    // define pause ui
    Button Resume(640,50+150,1,0.25,"Resume",18);
    Button SaveLevelBtn(640, 150+150, 1, 0.25, "Save", 18);
    Button SaveAndQuit(640,250+150,1,0.25,"Save and Quit",18);
    Button Quit(640,350+150,1,0.25,"Quit",18);

    Canvas pauseBG(0, 0, 5, 5,200);

    sf::Vector2i cameraPosition;
    sf::Vector2i mousePosition;

    // camera
    sf::Vector2i oldmp;
    sf::Vector2i cpOnClick;
    bool dragging = false;
    float zoom = 1;
    int nobjid = 1;

    block blocks[80000];

    for (int b = 0; b < 80000; b++)
    {
        blocks[b].id = 0;
        blocks[b].x = 0;
        blocks[b].y = 0;
    }

    int rendered = 0;

    initializeTextures();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Button::Right)
                {
                    oldmp = sf::Mouse::getPosition();
                    cpOnClick = cameraPosition;
                    dragging = true;
                }

                if (event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    clicked=true;

                    if (!paused)
                    {
                        placeObject(mousePosition, cameraPosition, nobjid, blocks);
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)
            {
                dragging = false;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == sf::Keyboard::P)
                {
                    paused=!paused;
                }
                if (event.key.code == sf::Keyboard::Key::S)
                {
                    saveLevel("test", blocks);
                }

                if (event.key.code == sf::Keyboard::Key::Up)
                {
                    nobjid++;
                }
                if (event.key.code == sf::Keyboard::Key::Down)
                {
                    nobjid--;
                }
            }
        }

        if (dragging && !paused)
        {
            sf::Vector2i cmp = sf::Mouse::getPosition();
            cameraPosition.x = cpOnClick.x + (cmp.x - oldmp.x);
            cameraPosition.y = cpOnClick.y + (cmp.y - oldmp.y);
        }

        window.clear(sf::Color(40, 105, 235, 235));
        line.setPosition(cameraPosition.x,0);

        sf::FloatRect visibleArea(
            0 - 30,
            0 - 30,
            window.getSize().x + 30,
            window.getSize().y + 30);

        for (int b = 0; b < 80000; b++)
        {
            sf::Vector2f objectPosition(blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y);

            if (blocks[b].id == 0)
            {
                continue;
            }
            if (!visibleArea.contains(objectPosition))
            {
                continue;
            }

            window.draw(getSprite(blocks[b].id, blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y, zoom));
            rendered++;
        }

        if ((0+(512*groundlc)+cameraPosition.x)+512<0) {
            groundlc+=1;
        }
        if ((1536+(512*groundlc)+cameraPosition.x)-512>720) {
            groundlc-=1;
        }

        ground.setPosition(0+(512*groundlc)+cameraPosition.x,690+cameraPosition.y);
        ground2.setPosition(512+(512*groundlc)+cameraPosition.x,690+cameraPosition.y);
        ground3.setPosition(1024+(512*groundlc)+cameraPosition.x,690+cameraPosition.y);
        ground4.setPosition(1024+512+(512*groundlc)+cameraPosition.x,690+cameraPosition.y);
        window.draw(ground);
        window.draw(ground2);
        window.draw(ground3);
        window.draw(ground4);

        float fps = 1 / fpsClock.getElapsedTime().asSeconds();

        fpsClock.restart().asSeconds();

        mousePosition = sf::Mouse::getPosition(window);

        window.draw(line);

        if (paused)
        {
            window.draw(pauseBG.draw());

            // display pause UI
            sf::Text title;
            title.setCharacterSize(36);
            title.setFont(roboto);
            title.setFillColor(sf::Color::White);
            title.setString("GDedit");
            sf::FloatRect titleRect = title.getLocalBounds();
            title.setOrigin(titleRect.left+titleRect.width/2,titleRect.top+titleRect.height/2);
            title.setPosition(640,80);

            window.draw(title);

            completeButton rc = Resume.draw();
            window.draw(rc.sprite);
            window.draw(rc.label);
            completeButton saq = SaveAndQuit.draw();
            window.draw(saq.sprite);
            window.draw(saq.label);            
            completeButton slc = SaveLevelBtn.draw();
            window.draw(slc.sprite);
            window.draw(slc.label);
            completeButton qc = Quit.draw();
            window.draw(qc.sprite);
            window.draw(qc.label);

            // check for button click
            if (clicked) {
                if (Resume.isMouseInside(mousePosition)) {
                    paused=false;
                }
                if (SaveAndQuit.isMouseInside(mousePosition)) {
                    saveLevel("test",blocks);
                    window.close();
                }
                if (SaveLevelBtn.isMouseInside(mousePosition)) {
                    saveLevel("test",blocks);
                }
                if (Quit.isMouseInside(mousePosition)) {
                    window.close();
                }
            }
        }

		if (reason!=std::string("")) {
			Canvas error(1280/2-(256*3/2), 720/2-(256*2/2),3,2,200);
			sf::Text warning;
			warning.setFont(roboto);
			warning.setCharacterSize(36);
			warning.setFillColor(sf::Color::Red);
			warning.setString(std::string("ERROR:"));
			sf::FloatRect bounds = warning.getLocalBounds();
			warning.setOrigin(bounds.left+bounds.width/2,bounds.top+bounds.height/2);
			warning.setPosition(1280/2,150);

			sf::Text msg;
			msg.setFont(roboto);
			msg.setCharacterSize(18);
			msg.setFillColor(sf::Color::White);
			msg.setString(reason);
			sf::FloatRect msgBounds = msg.getLocalBounds();
			msg.setOrigin(msgBounds.left+msgBounds.width/2,msgBounds.top+msgBounds.height/2);
			msg.setPosition(1280/2,720/2);

			Button btn(1280/2,(720/2)+225,1,0.2,std::string("OK"),18);

			window.draw(error.draw());
			window.draw(warning);
			window.draw(msg);
			window.draw(btn.drawButton());
			window.draw(btn.drawText());
			
			if (clicked) {
				if (btn.isMouseInside(mousePosition)) {
					reason=std::string("");
				}
			}
		}

        clicked=false;

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
        rText.setPosition(0, 20);
        window.draw(rText);

        sf::Text mpos;
        mpos.setCharacterSize(18);
        mpos.setString(std::string("nobjid: ").append(std::to_string(nobjid)));
        mpos.setFont(roboto);
        mpos.setPosition(0, 40);
        window.draw(mpos);

        window.display();
        rendered = 0;
    }

    return 0;
}
