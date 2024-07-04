#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>

sf::Texture ui_textures[2];
sf::Font uifnt;

int initializeUI() {
    ui_textures[0].loadFromFile("assets/ui/ui_bg.png");
    ui_textures[0].setSmooth(true);

    uifnt.loadFromFile("assets/fonts/roboto.ttf");
    return 0;
}

struct completeButton {
    sf::Sprite sprite;
    sf::Text label;
};

class Button {
    public:
        // button variables
        int x;
        int y;
        float width;
        float height;
        int alpha;

        // text variables
        std::string title;
        int textSize;

        Button() {
            this->x=0;
            this->y=0;
            this->width=0;
            this->height=0;
            this->alpha=100;
            this->title="";
            this->textSize=0;
        }

        Button(int x, int y,float width, float height,int alpha,std::string title, int textSize) {
            this->x=x;
            this->y=y;
            this->width=width;
            this->height=height;
            this->title=title;
            this->alpha=alpha;
            this->textSize = textSize;
        }

        sf::Sprite drawButton() {
            sf::Sprite output;
            output.setTexture(ui_textures[0]);
            output.setPosition(x,y);
            output.setOrigin(sf::Vector2f(ui_textures[0].getSize().x/2,ui_textures[0].getSize().y/2));
            output.setScale(sf::Vector2f(width,height));
            output.setColor(sf::Color(255,255,255,alpha));
            return output;
        }
        
        sf::Text drawText() {
            sf::Text output;
            output.setFont(uifnt);
            output.setCharacterSize(textSize);
            output.setFillColor(sf::Color::White);
            output.setString(title);

            sf::FloatRect textRect;
            textRect = output.getLocalBounds();
            output.setOrigin(textRect.left+textRect.width/2,textRect.top+textRect.height/2);
            output.setPosition(x,y);
            return output;
        }

        bool isMouseInside(sf::Vector2i mousePosition) {
            sf::FloatRect bounds = drawButton().getGlobalBounds();

            if (bounds.contains(mousePosition.x,mousePosition.y)) {
                return true;
            }
            return false;
        }

        completeButton draw() {
            completeButton output;
            output.sprite = drawButton();
            output.label = drawText();
            return output;
        }
};

class Canvas {
    public:
        int x;
        int y;
        float width;
        float height;
        int alpha;

        Canvas(int x, int y, float width, float height, int alpha) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            this->alpha = alpha;
        }

        sf::Sprite draw() {
            sf::Sprite output;
            output.setTexture(ui_textures[0]);
            output.setPosition(x,y);
            output.setColor(sf::Color(255,255,255,alpha));
            output.setScale(sf::Vector2f(width,height));
            return output;
        }
};

class TextInput {
    public:
        int x;
        int y;
        int id;
        int textSize;
        float width;
        float height;
        std::string value;

        TextInput() {
            this->x = 0;
            this->y = 0;
            this->width = 0;
            this->height = 0;
            this->textSize = -1;
            this->id=-1;
        }

        TextInput(int x, int y, float width,float height,int textSize,int id) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
            this->textSize=textSize;
        }

        sf::Sprite draw() {
            sf::Sprite output;
            output.setTexture(ui_textures[0]);
            output.setPosition(x,y);
            output.setOrigin(sf::Vector2f(ui_textures[0].getSize().x/2,ui_textures[0].getSize().y/2));
            output.setScale(sf::Vector2f(width,height));
            return output;
        }

        bool isMouseInside(sf::Vector2i mousePosition) {
            sf::FloatRect bounds = draw().getGlobalBounds();

            if (bounds.contains(mousePosition.x,mousePosition.y)) {
                return true;
            }
            return false;
        }

        sf::Text drawInput() {
            sf::Text output;
            output.setFont(uifnt);
            output.setCharacterSize(textSize);
            output.setFillColor(sf::Color::White);
            output.setString(value);

            sf::FloatRect textRect;
            textRect = output.getLocalBounds();
            output.setOrigin(textRect.left+textRect.width/2,textRect.top+textRect.height/2);
            output.setPosition(x,y);
            return output;
        }

        void set_default() { // set to default constructor
            this->x = 0;
            this->y = 0;
            this->width = 0;
            this->height = 0;
            this->textSize = -1;
        }
};