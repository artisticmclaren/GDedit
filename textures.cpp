#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>

bool texLoaded=false;

bool fileExists(std::string path) {
    std::ifstream file;
    file.open(path);
    if (file) {return true; }
    return false;
}

sf::Texture textures[169];

int initializeTextures() {
    for (int i=0; i<169; i++) {
        sf::Texture tex;
        std::string path="assets/ids/";
        std::string ext = ".png";
        path.append(std::to_string(i));
        path.append(ext);

        if (!fileExists(path)) {
            i++;
            continue;
        }

        if (!tex.loadFromFile(path)) {
            tex.loadFromFile("assets/tnotfound.png");
        }
        tex.setSmooth(true);
        textures[i]=tex;
    }

    texLoaded=true;
    return 0;
}