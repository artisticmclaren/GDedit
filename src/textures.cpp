#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <raylib.h>
#include <algorithm>

struct objData {
	int id;
	Texture2D tex;
};

bool compareById(const objData &a, const objData &b) { return a.id < b.id; }

const int texCount=177;
Texture2D textures[texCount];
objData obj_data[texCount];

int initializeTextures() {
    std::filesystem::path pcwd = std::filesystem::current_path();
    std::filesystem::path ids = pcwd / "assets/ids";
    std::string cwd(pcwd.u8string());
    int i=0;
    for (const auto file : std::filesystem::directory_iterator(ids)) {
        std::string filename = file.path().filename().u8string();
        int len = filename.length();
        int id = std::stoi(filename.erase(len-4));

        Image img = LoadImage(file.path().u8string().c_str());
		Texture2D tex = LoadTextureFromImage(img);
		SetTextureFilter(tex,TEXTURE_FILTER_BILINEAR);
        obj_data[i].id = id;
        obj_data[i].tex = tex;
        i++;
    }
    std::sort(obj_data,obj_data+texCount,compareById);
    for (int i=0; i<texCount; i++) { textures[i] = obj_data[i].tex; }
    return 0;
}
