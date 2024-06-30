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

std::string debugText="";
int d_msg=0;

void LogDebug(std::string msg) {
    if (d_msg==32) {
        d_msg=0;
        debugText=std::string("");
    }
    debugText.append(msg.append("\n"));
    d_msg++;
}

sf::Transformable setOriginAndReadjust(sf::Transformable object, const sf::Vector2f &newOrigin)
{
        auto offset = newOrigin - object.getOrigin();
        object.setOrigin(newOrigin);
        object.move(offset);
        return object;
}

sf::Sprite getSprite(int id, int x, int y, float rot, float z, bool isBeingEdited,int cLayer,int layer)
{
    sf::Sprite output;
    output.rotate(rot);
    output.setTexture(textures[id]);
    output.setPosition(sf::Vector2f(x, y));
    sf::Vector2u texSize = textures[id].getSize();
    float scalex=0.6;
    float scaley=0.6;
    output.setScale(sf::Vector2f(scalex * z, scaley * z));
    if (isBeingEdited) { output.setColor(sf::Color::Green); }
    sf::Transformable transform;
    transform.setPosition(output.getPosition());
    transform.setOrigin(output.getOrigin());
    transform.setScale(output.getScale());
    sf::Transformable newTransform = setOriginAndReadjust(transform,sf::Vector2f(textures[id].getSize().x/2,textures[id].getSize().y/2));
    output.setPosition(newTransform.getPosition());
    output.setOrigin(newTransform.getOrigin());
    if (cLayer>-1) {
        if (layer!=cLayer) {
            output.setColor(sf::Color(255,255,255,100));
        }
    }

    return output;
}

struct block
{
    int x;
    int y;
    int id;
    int layer;
    float rotation;
};

bool paused = false;
bool exploring = false;
bool debug=false;

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

int cLayer=0;

bool saveLevel(std::string levelName, block blocks[80000])
{
    std::ofstream save(std::string("saves/").append(levelName));
    std::ofstream save_gd(std::string("saves/").append(levelName).append(".gd"));
    std::string l_data = "";
    std::string ld_gd = "";

	if (std::filesystem::is_directory(std::string("saves"))==false) {
		reason=std::string("directory 'saves' does not exist.");
        LogDebug("[ERROR] Directory 'saves' does not exist.");
		return false;
	}

    for (int b = 0; b < 80000; b++)
    {
        if (blocks[b].id > 0)
        {
            std::string all = "";
            std::string gd_all = "";

            int r_id = obj_data[blocks[b].id].id;
            std::string id = std::to_string(blocks[b].id).append("\n");
            std::string x = std::to_string(blocks[b].x).append("\n");
            std::string y = std::to_string(blocks[b].y).append("\n");
            std::string r = std::to_string(blocks[b].rotation).append("\n");
            std::string l =std::to_string(blocks[b].layer);

            std::string gd_id = std::to_string(r_id).append("\n");

            all.append(id).append(x).append(y).append(r).append(l);
            gd_all.append(gd_id).append(x).append(y).append(r).append(l);
            gd_all.append(std::string("\nnew\n"));
            all.append(std::string("\nnew\n"));
            l_data.append(all);
            ld_gd.append(gd_all);            
        }
        else
        {
            continue;
        }
    }

    save << l_data;
    save_gd << ld_gd;
    return true;
}

int count_occurences(std::string input,char f) {
  int count = 0;

  for (int i = 0; i < input.size(); i++)
    if (input[i] == f) count++;

  return count;
}

int totalPlaced=0;

bool LoadSave(std::string levelName,block blocks[80000]) {
    std::filesystem::path pcwd = std::filesystem::current_path();
    std::string saveFile(pcwd.u8string());
    saveFile.append("/saves/").append(levelName);
    ifstream save(saveFile);
    std::stringstream buf;
    buf << save.rdbuf();
    std::string saveString = buf.str();
    std::vector<std::string> lines;
    std::stringstream ss(saveString);
    std::string line;
    objectCount=0;
    for (int b=0;b<totalPlaced;b++) {
        blocks[b].id=0;
        blocks[b].x=0;
        blocks[b].y=0;
        blocks[b].layer=0;
        blocks[b].rotation=0;
    }

    totalPlaced=0;

    int c=1;
    int total=0;
    block nb;

    while (std::getline(ss,line,'\n')) {
        lines.push_back(line);
        if (line=="") {
            continue;
        }
        switch (c) {
            case 1:
                nb.id = stoi(line);
                break;
            case 2:
                nb.x = stoi(line);
                break;
            case 3:
                nb.y = stoi(line);
                break;
            case 4:
                nb.rotation = stof(line);
                break;
            case 5:
                nb.layer = stoi(line);
                break;
            case 6:
                c=0;
                std::cout << "id " << nb.id << " x " << nb.x << " y " << nb.y << " rot " << nb.rotation << " lay " << nb.layer << std::endl;
                blocks[total] = nb;
                total++;
                objectCount++;
                totalPlaced++;
        }

        c++;
    }

    save.close();
    return true;
}

void deleteObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, block blocks[80000]) {
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;

    for (int b = 0; b<totalPlaced; b++) {
        if (blocks[b].x == rounded.x && blocks[b].y == rounded.y) {
            if (cLayer==-1 || blocks[b].layer==cLayer) {
                blocks[b].id=0;
                blocks[b].x=0;
                blocks[b].y=0;
                blocks[b].rotation=0;
                objectCount--;
            }
        }
    }
}

void placeObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, int nobjid, float rotation, int layer, block blocks[80000])
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
    if (layer==-1) {
        blocks[totalPlaced].layer=0;
    } else {
        blocks[totalPlaced].layer=layer;
    }
    objectCount++;
    totalPlaced++;
}

int editSelected=-1;

int pickObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, block blocks[80000]) {
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;

    for (int b=0;b<totalPlaced;b++) {
        if (blocks[b].id==0) {continue;}
        if (rounded.x==blocks[b].x && rounded.y==blocks[b].y) {
            return blocks[b].id;
        }
    }
    return -1;
}

void editObject(sf::Vector2i mousePosition, sf::Vector2i cameraPosition, block blocks[80000]) {
    sf::Vector2i mpcp;
    mpcp.x = mousePosition.x - cameraPosition.x;
    mpcp.y = mousePosition.y - cameraPosition.y;

    sf::Vector2i rounded = roundPositions(mpcp);
    if (mpcp.x<0) rounded.x-=30;
    if (mpcp.y<0) rounded.y-=30;


    for (int b = 0; b<totalPlaced; b++) {
        if (blocks[b].id==0) {continue;}
        if (rounded.x==blocks[b].x && rounded.y==blocks[b].y) {
            editSelected=b;
        }
    }
}


bool clicked;
bool ctrlClicked=false;
bool shiftClicked=false;
bool loading=false;
bool levelLoaded=false;
std::string levelName="null";

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

    sf::Texture objSelectTex;
    sf::Sprite objSelect;

    objSelectTex.loadFromFile("assets/objects.png");
    objSelect.setTexture(objSelectTex);
    objSelect.setPosition(135,30);

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
    line.setFillColor(sf::Color(255,255,255,100));

    sf::RectangleShape abyss;
    abyss.setSize(sf::Vector2f(1280,720));
    abyss.setPosition(0,720);
    abyss.setFillColor(sf::Color(0,0,0));

    // define pause ui
    Button Resume(640,50+150,1,0.25,255,"Resume",18);
    Button Load(640,150+150,1,0.25,255,"Load",18);
    Button SaveLevelBtn(640, 250+150, 1, 0.25,255,"Save", 18);
    Button SaveAndQuit(640,350+150,1,0.25,255,"Save and Quit",18);
    Button Quit(640,450+150,1,0.25,255,"Quit",18);

    // other
    Button objectM(90+20,720-42,0.7,0.25,255,"Object",18);
    Button edit(90+20+189,720-42,0.7,0.25,255,"Edit",18);
    Button objectC(1280-30,30,0.17578125,0.17578125,100,"",0);

    Button layerLeft(1050,650,0.25,0.25,255,"",0);
    Button layerRight(1220,650,0.25,0.25,255,"",0);
    sf::Text cLayerText;
    cLayerText.setFont(roboto);
    cLayerText.setFillColor(sf::Color::White);
    cLayerText.setString(std::string(std::to_string(cLayer)));
    cLayerText.setPosition(1114,650);

    int mode = 0; // 0: object mode, 1: edit mode

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
        blocks[b].layer=0;
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

                if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                    int nid = pickObject(mousePosition,cameraPosition,blocks);
                    if (nid>0) {
                        nobjid=nid;
                        preview.setTexture(textures[nobjid]);
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Button::Left)
                {
                    clicked=true;
                    bool clickedButton=false;

                    if (objectM.isMouseInside(mousePosition)) {
                        mode=0;
                        clickedButton=true;
                    } else if (edit.isMouseInside(mousePosition)) {
                        mode=1;
                        clickedButton=true;
                    }
                    if (objectC.isMouseInside(mousePosition)) {
                        exploring=true;
                        clickedButton=true;
                    }

                    if (layerLeft.isMouseInside(mousePosition)) {
                        if (cLayer>0) {
                            cLayer--;
                        } else {
                            cLayer=-1;
                        }

                        clickedButton=true;
                    }
                    if (layerRight.isMouseInside(mousePosition)) { cLayer++; clickedButton=true; }
                    
                    if (!paused && !exploring && !clickedButton)
                    {
                        if (mode==0) {
                            placeObject(mousePosition, cameraPosition, nobjid,rotation, cLayer, blocks);
                        } else if (mode==1) {
                            if (editSelected==-1) {
                                editObject(mousePosition,cameraPosition,blocks);
                            } else if (editSelected>-1) {
                                editSelected=-1;
                            }

                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Button::Right)
            {
                dragging = false;
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Key::LControl && !ctrlClicked) {
                  ctrlClicked=true;
                }
                if (event.key.code==sf::Keyboard::Key::LShift && !shiftClicked) {
                  shiftClicked=true;
                }
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
                if (event.key.code==sf::Keyboard::Key::Num1) {
                    mode=0;
                }
                if (event.key.code==sf::Keyboard::Key::Num2) {
                    mode=1;
                }
                if (event.key.code==sf::Keyboard::Left) {
                    if (cLayer>-1) {
                        cLayer--;
                    }
                }
                if (event.key.code==sf::Keyboard::Right) {
                    cLayer++;
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

        if (editSelected>-1 && mode==1) {
            sf::Vector2i mpcp = sf::Vector2i(mousePosition.x-cameraPosition.x,mousePosition.y-cameraPosition.y);
            sf::Vector2i rounded = roundPositions(mpcp);
            if (mpcp.x<0) rounded.x-=30;
            if (mpcp.y<0) rounded.y-=30;

            blocks[editSelected].x = rounded.x;
            blocks[editSelected].y = rounded.y;
        }

        window.draw(objectC.drawButton());
        window.draw(preview);
        line.setPosition(cameraPosition.x,0);

        sf::FloatRect visibleArea(
            0 - 30,
            0 - 30,
            window.getSize().x + 30,
            window.getSize().y + 30);

        for (int b = 0; b < totalPlaced; b++)
        {
            sf::Vector2f objectPosition(blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y);

            if (blocks[b].id == 0 || !visibleArea.contains(objectPosition)) { continue; }

            bool edited;
            if (b==editSelected) {
                edited=true;
            } else {
                edited=false;
            }
            window.draw(getSprite(blocks[b].id, blocks[b].x + cameraPosition.x, blocks[b].y + cameraPosition.y, blocks[b].rotation, zoom,edited,cLayer,blocks[b].layer));
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
        ground.setPosition(0+(512*groundlc)+cameraPosition.x,670+cameraPosition.y);
        ground2.setPosition(512+(512*groundlc)+cameraPosition.x,670+cameraPosition.y);
        ground3.setPosition(1024+(512*groundlc)+cameraPosition.x,670+cameraPosition.y);
        ground4.setPosition(1024+512+(512*groundlc)+cameraPosition.x,670+cameraPosition.y);
        window.draw(ground);
        window.draw(ground2);
        window.draw(ground3);
        window.draw(ground4);

        float fps = 1 / fpsClock.getElapsedTime().asSeconds();

        fpsClock.restart().asSeconds();

        mousePosition = sf::Mouse::getPosition(window);

        window.draw(line);

        // menu
        completeButton ec = edit.draw();
        window.draw(ec.sprite);
        window.draw(ec.label);
        completeButton oc = objectM.draw();
        window.draw(oc.sprite);
        window.draw(oc.label);

        // layer controls
        sf::Texture LLtex;
        LLtex.loadFromFile("assets/ui/arrowLeft.png");
        LLtex.setSmooth(true);
        sf::Texture LRtex;
        LRtex.loadFromFile("assets/ui/arrowRight.png");

        sf::Sprite LLsprite = layerLeft.drawButton();
        LLsprite.setTexture(LLtex);
        sf::Sprite LRsprite = layerRight.drawButton();
        LRsprite.setTexture(LRtex);

        window.draw(LLsprite);
        sf::FloatRect textRect;
        textRect = cLayerText.getLocalBounds();
        if (cLayer==-1) {
            cLayerText.setString("ALL");
        } else {
            cLayerText.setString(std::to_string(cLayer));
        }
        cLayerText.setOrigin(textRect.left+textRect.width/2,textRect.top+textRect.height/2);
        cLayerText.setPosition(1114+(42/2),650);
        window.draw(cLayerText);
        window.draw(LRsprite);

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

            window.draw(objSelect);

            Button close(640,670,0.7,0.25,255,"Close",18);
            completeButton closeDraw = close.draw();
            window.draw(closeDraw.sprite);
            window.draw(closeDraw.label);

            if (clicked) {
                for (int i=0; i<texCount; i++) {
                    if (expBtn[i].isMouseInside(mousePosition)) {
                        nobjid=i;
                        preview.setTexture(textures[nobjid]);
                    }
                }

                if (close.isMouseInside(mousePosition)) {
                    exploring=false;
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
            completeButton lc = Load.draw();
            window.draw(lc.sprite);
            window.draw(lc.label);
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
            if (clicked && reason==std::string("")) {
                if (Resume.isMouseInside(mousePosition)) {
                    paused=false;
                }
                if (Load.isMouseInside(mousePosition)) {
                    LoadSave("test",blocks);
                }
                if (SaveAndQuit.isMouseInside(mousePosition)) {
                    saveLevel(levelName,blocks);
                    window.close();
                }
                if (SaveLevelBtn.isMouseInside(mousePosition)) {
                    saveLevel(levelName,blocks);
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

			Button btn(1280/2,(720/2)+225,1,0.2,255,std::string("OK"),18);

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
        rot.setString(std::string("mode: ").append(std::to_string(mode)));
        rot.setFont(roboto);
        rot.setPosition(0,60);
        window.draw(rot);

        if (mode==1) { 
            edit.textSize=24;
            objectM.textSize=18; 
        } else if (mode==0) {
            edit.textSize=18;
            objectM.textSize=24;
        }

        if (debug==true) {
            sf::Text d_text;
            d_text.setCharacterSize(20);
            d_text.setFont(roboto);
            d_text.setString(debugText);
            d_text.setPosition(0,80);
            window.draw(d_text);
        }

        window.display();
        rendered = 0;
    }

    return 0;
}
