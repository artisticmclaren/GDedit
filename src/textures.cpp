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

void DrawLoadingScreen(int loadedTextures,int totalTextures)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Loading Textures...", GetScreenWidth()/2 - MeasureText("Loading Textures...", 20)/2, GetScreenHeight()/2 - 10, 20, WHITE);
    DrawText(TextFormat("%d/%d",loadedTextures,totalTextures), GetScreenWidth()/2 - MeasureText(TextFormat("%d/%d",loadedTextures,totalTextures), 20)/2, GetScreenHeight()/2 + 25, 20, WHITE);
    EndDrawing();
}

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
        // DrawLoadingScreen(i,texCount); // sucks for now, might readd later
    }
    std::sort(obj_data,obj_data+texCount,compareById);
    for (int i=0; i<texCount; i++) { textures[i] = obj_data[i].tex; }
    return 0;
}
