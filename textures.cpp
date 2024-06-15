#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

bool texLoaded=false;
bool startLoad=false;

struct objData {
    int id;
    sf::Texture tex;
};

bool compareById(const objData &a, const objData &b) { return a.id < b.id; }

const int texCount=169;
sf::Texture textures[texCount];
objData obj_data[texCount];

int getTextureID(int r_id) {
    for (int i=0; i< texCount; i++) {
        if (obj_data[i].id = r_id) {
            return i;
        }
    }
    return -1;
}

int initializeTextures() {
    startLoad=true;
    std::filesystem::path pcwd = std::filesystem::current_path();
    std::filesystem::path ids = pcwd / "assets/ids";
    std::string cwd(pcwd.u8string());
    int i=0;
    for (const auto file : std::filesystem::directory_iterator(ids)) {
        std::string filename = file.path().filename().u8string();
        int len = filename.length();
        int id = std::stoi(filename.erase(len-4));

        sf::Texture tex;
        tex.loadFromFile(file.path().u8string());
        obj_data[i].id = id;
        obj_data[i].tex = tex;
        i++;
    }
    std::sort(obj_data,obj_data+texCount,compareById);
    for (int i=0; i<texCount; i++) { textures[i] = obj_data[i].tex; }

    texLoaded=true;
    return 0;
}