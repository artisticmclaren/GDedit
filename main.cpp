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

sf::Transformable setOriginAndReadjust(sf::Transformable object, const sf::Vector2f &newOrigin)
{
        auto offset = newOrigin - object.getOrigin();
        object.setOrigin(newOrigin);
        object.move(offset);
        return object;
}

sf::Sprite getSprite(int id, int x, int y, float rot, float z)
{
    sf::Sprite output;
    output.rotate(rot);
    output.setTexture(textures[id]);
    output.setPosition(sf::Vector2f(x, y));
    output.setScale(sf::Vector2f(0.6 * z, 0.6 * z));
    sf::Transformable transform;
    transform.setPosition(output.getPosition());
    transform.setOrigin(output.getOrigin());
    transform.setScale(output.getScale());
    sf::Transformable newTransform = setOriginAndReadjust(transform,sf::Vector2f(textures[id].getSize().x/2,textures[id].getSize().y/2));
    output.setPosition(newTransform.getPosition());
    output.setOrigin(newTransform.getOrigin());

    return output;
}

bool stopHold = false;

struct block
{
    int x;
    int y;
    int id;
    float rotation;
};

bool paused = false;
bool exploring = false;

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
    std::string l_data = "";

	if (std::filesystem::is_directory(std::string("saves"))==false) {
		reason=std::string("directory 'saves' does not exist.");
		return false;
	}

    for (int b = 0; b < 80000; b++)
    {
        if (blocks[b].id > 0)
        {
            std::string all = "";
            int r_id = obj_data[blocks[b].id].id;
            std::string id = std::string(":").append(std::to_string(r_id));
            std::string x = std::string(";").append(std::to_string(blocks[b].x));
            std::string y = std::string(";").append(std::to_string(blocks[b].y));
            std::string r = std::string(";").append(std::to_string(blocks[b].rotation));
            all.append(id).append(x).append(y).append(r);
            l_data.append(all);
        }
        else
        {
            continue;
        }
    }

    save << l_data;
    return true;
}

int totalPlaced=0;

void deleteObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, block blocks[80000]) {
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;

    for (int b = 0; b<totalPlaced; b++) {
        if (blocks[b].x == rounded.x && blocks[b].y == rounded.y) {
            blocks[b].id=0;
            blocks[b].x=0;
            blocks[b].y=0;
            blocks[b].rotation=0;
            objectCount--;
        }
    }
}

void placeObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, int nobjid, float rotation, block blocks[80000])
{
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;
    blocks[totalPlaced].id = nobjid;
    blocks[totalPlaced].x = rounded.x;
    blocks[totalPlaced].y = rounded.y;
    blocks[totalPlaced].rotation=rotation;
    objectCount++;
    totalPlaced++;
}

bool clicked;
bool loading=false;

int main()
{

    sf::RenderWindow window(sf::VideoMode(1280, 720), "GDedit");

    auto icon = sf::Image();
    icon.loadFromFile("assets/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // clocks
    sf::Clock fpsClock;

    // init
    initializeUI();
    initializeTextures();

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

    sf::RectangleShape abyss;
    abyss.setSize(sf::Vector2f(1280,720));
    abyss.setPosition(0,720);
    abyss.setFillColor(sf::Color(0,0,0));

    // define pause ui
    Button Resume(640,50+150,1,0.25,"Resume",18);
    Button SaveLevelBtn(640, 150+150, 1, 0.25, "Save", 18);
    Button SaveAndQuit(640,250+150,1,0.25,"Save and Quit",18);
    Button Quit(640,350+150,1,0.25,"Quit",18);

    Canvas pauseBG(0, 0, 5, 5,200);
    Canvas exploreBG(1280/2-(256*4)/2,10,4,2.75,200);

    sf::Vector2i cameraPosition;
    sf::Vector2i mousePosition;

    // camera
    sf::Vector2i oldmp;
    sf::Vector2i cpOnClick;
    bool dragging = false;
    float zoom = 1;
    float rotation=0;
    int nobjid = 1;


    sf::Sprite preview;
    preview.setTexture(textures[nobjid]);
    preview.setScale(0.6,0.6);
    preview.setOrigin(textures[nobjid].getSize().x/2,textures[nobjid].getSize().y/2);
    preview.setPosition(1280-30,30);
    preview.setRotation(rotation);

    block blocks[80000];

    for (int b = 0; b < 80000; b++)
    {
        blocks[b].id = 0;
        blocks[b].x = 0;
        blocks[b].y = 0;
        blocks[b].rotation=0;
    }

    int rendered = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                // breaks some stuff but at least it doesnt stretch everything anymore
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Button::Right)
                {
                    if (!paused && !exploring) {
                        oldmp = sf::Mouse::getPosition();
                        cpOnClick = cameraPosition;
                        dragging = true;
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    clicked=true;

                    if (!paused && !exploring)
                    {
                        placeObject(mousePosition, cameraPosition, nobjid,rotation, blocks);
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
                if (event.key.code==sf::Keyboard::Key::Q) {
                    rotation-=90;
                    preview.setRotation(rotation);
                }
                if (event.key.code==sf::Keyboard::Key::E) {
                    rotation+=90;
                    preview.setRotation(rotation);
                }
                if (event.key.code==sf::Keyboard::Key::O) {
                    exploring=!exploring;
                }
                if (event.key.code == sf::Keyboard::Key::Delete || event.key.code == sf::Keyboard::Key::Backspace) {
                    deleteObject(mousePosition,cameraPosition,blocks);
                }
            }
        }

        if (dragging && !paused)
        {
            sf::Vector2i cmp = sf::Mouse::getPosition();
            cameraPosition.x = cpOnClick.x + (cmp.x - oldmp.x);
            cameraPosition.y = cpOnClick.y + (cmp.y - oldmp.y);
        }

        window.clear(sf::Color(40,105,235,235));
        window.draw(preview);
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

            window.draw(getSprite(blocks[b].id, blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y, blocks[b].rotation, zoom));
            rendered++;
        }

        if ((0+(512*groundlc)+cameraPosition.x)+512<0) {
            groundlc+=1;
        }
        if ((1536+(512*groundlc)+cameraPosition.x)-512>720) {
            groundlc-=1;
        }

        abyss.setPosition(0,946+cameraPosition.y);
        abyss.setScale(720,1280+cameraPosition.y);
        window.draw(abyss);
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

        if (exploring) {
            window.draw(exploreBG.draw());

            sf::Text label;
            label.setCharacterSize(24);
            label.setFont(roboto);
            label.setFillColor(sf::Color::White);
            label.setString("Objects");
            sf::FloatRect labelRect = label.getLocalBounds();
            label.setOrigin(labelRect.left+labelRect.width/2,labelRect.top+labelRect.height/2);
            label.setPosition((int)640,(int)25);
            window.draw(label);
            Button expBtn[texCount];

            int x=0;
            int y=0;

            for (int i=1; i<texCount; i++) {
                //          ↓ took me ages to find this number
                Button btn(149+(35*x),55+(35*y),0.1172,0.1172,std::string(""),0);
                sf::Sprite obj;
                obj.setTexture(textures[i]);
                sf::Vector2u texSize = textures[i].getSize();
                float sfx=0;
                float sfy=0;
                if (texSize.x == texSize.y) {
                    sfx = 30.0f/texSize.x;
                    sfy = 30.0f/texSize.y;
                } else {
                    sfy = 30.0f/texSize.y;
                    sfx=sfy;
                }
                obj.setScale(sfx,sfy);
                obj.setPosition((149+(35*x))-15,(55+(35*y))-15);
                int r_id = obj_data[i].id;
                btn.title=std::to_string(r_id);
                completeButton cbtn = btn.draw();
                cbtn.sprite.setColor(sf::Color(255,255,255,0));
                window.draw(cbtn.sprite);
                expBtn[i]=btn;
                window.draw(obj);
                if (x==28) {
                    x=0;
                    y++;
                } else {
                    x++;
                }
            }

            if (clicked) {
                for (int i=0; i<texCount; i++) {
                    if (expBtn[i].isMouseInside(mousePosition)) {
                        nobjid=i;
                        preview.setTexture(textures[nobjid]);
                    }
                }
            }

        }

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

            sf::Text objcount;
            objcount.setCharacterSize(18);
            objcount.setFont(roboto);
            objcount.setFillColor(sf::Color::White);
            sf::FloatRect objRect = objcount.getLocalBounds();
            objcount.setOrigin(objRect.left+objRect.width/2,objRect.top+objRect.height/2);
            objcount.setPosition(5,697);
            objcount.setString(std::string("Objects: ").append(std::to_string(objectCount)));
            window.draw(objcount);

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
        mpos.setString(std::string("tex list id: ").append(std::to_string(nobjid)));
        mpos.setFont(roboto);
        mpos.setPosition(0, 40);
        window.draw(mpos);

        sf::Text rot;
        rot.setCharacterSize(18);
        rot.setString(std::string("rot: ").append(std::to_string(rotation)));
        rot.setFont(roboto);
        rot.setPosition(0,60);
        window.draw(rot);

        window.display();
        rendered = 0;
    }

    return 0;
}
