#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>

sf::Texture ui_textures[1];
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
        // variables
        int x;
        int y;
        float width;
        float height;
        std::string title;

        Button(int x, int y,float width, float height,std::string title) {
            this->x=x;
            this->y=y;
            this->width=width;
            this->height=height;
            this->title=title;
        }

        // functions
        bool isTouching(sf::Vector2i pos) {
            int _x=pos.x;
            int _y=pos.y;

            if (_x < x+width && _x > x && _y < y+height && _y > y) {
                return true;
            }
            return false;
        }

        sf::Sprite drawButton() {
            sf::Sprite output;
            output.setTexture(ui_textures[0]);
            output.setPosition(x,y);
            output.setScale(sf::Vector2f(width,height));
            return output;
        }
        
        sf::Text drawText() {
            sf::Text output;
            return output;
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
        int width;
        int height;

        Canvas(int x, int y, int width, int height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }

        sf::Sprite draw() {
            sf::Sprite output;
            output.setPosition(x,y);
            output.setScale(sf::Vector2f(width,height));
            return output;
        }
};