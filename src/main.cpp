#define RAYGUI_IMPLEMENTATION

#include <raylib.h>
#include "textures.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <array>

std::array<int,2> roundPositions(std::array<int,2> pos)
{
    int xr = pos[0] % 30;
    int yr = pos[1] % 30;

	std::array<int,2> output;

    if (xr == 0)
    {
        output[0] = pos[0];
    }
    else
    {
        output[0] = pos[0] - xr;
    }

    if (yr == 0)
    {
        output[1] = pos[1];
    }
    else
    {
        output[1] = pos[1] - yr;
    }

    return output;
}

struct color {
    public:
        int red;
        int green;
        int blue;
        int blending;
        float opacity;

        color() {
            this->red=255; // 1
            this->green=255; // 2
            this->blue=255; // 3
            this->blending=0; // 5
            this->opacity=1; // 7
        }

        color(int red, int green, int blue, int blending, float opacity) {
            this->red=red;
            this->blue=blue;
            this->green=green;
            this->blending=blending;
            this->opacity=opacity;
        }

        std::string toColorString(int channelID) {
            std::string output="";
            output.append("6_").append(std::to_string(channelID)).append("_");;
            output.append("1_").append(std::to_string(red)).append("_");
            output.append("2_").append(std::to_string(green)).append("_");
            output.append("3_").append(std::to_string(blue)).append("_");
            output.append("5_").append(std::to_string(blending)).append("_");
            output.append("7_").append(std::to_string(opacity)).append("_");;
            output.append("11_255_12_255_13_255");
            output.append("|");
            return output;
        }
};

struct block
{
    // ALL OBJECTS //
    int id; // 1
    int x; // 2
    int y; // 3
    int layer; // 20
    float rotation; // 6
    int mainColor = 1004; // 21
    int secondaryColor = 1004; // 22

    // ALL TRIGGERS //
    int touchTriggered=0;

    /// COLOR TRIGGER //
    int red=255; // 7
    int green=255; // 8
    int blue=255; // 9
    float durration=0.5; // 10

    block() {
        this->id = 0;
        this->x = 0;
        this->y= 0;
        this->layer = 0;
        this->rotation = 0;
        this->touchTriggered = 0;
        this->red = 0;
        this->green = 0;
        this->blue = 0;
        this->durration = 0;
    }

    block(int id,int x, int y, int layer, float rotation, int mainColor, int secondaryColor) {
        this->id = id;
        this->x = x;
        this->y= y;
        this->layer = layer;
        this->rotation = rotation;
        this->touchTriggered = touchTriggered;
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->durration = durration;
    }

    void setEqual(block nb) {
        this->id = nb.id;
        this->x = nb.x;
        this->y= nb.y;
        this->layer = nb.layer;
        this->rotation = nb.rotation;
        this->touchTriggered = nb.touchTriggered;
        this->red = nb.red;
        this->green = nb.green;
        this->blue = nb.blue;
        this->durration = nb.durration;
    }

	bool isMouseInside() {
		return false;
	}
};

std::vector<block> blocks;
color colors[1010];

int mousePosition[2] = {0,0};
int nobjid=1;

int main() {
	InitWindow(1280,720,"GDedit");
	SetTargetFPS(60);
	Font main=LoadFontEx("assets/fonts/roboto.ttf",18,0,256);

	// Show loading screen while initializing textures
    initializeTextures();

    // Clear the loading screen
    ClearBackground({40, 125, 255});

	while (!WindowShouldClose()) {
		ClearBackground({40,125,255});

		BeginDrawing();
		mousePosition[0]=GetMouseX();
		mousePosition[1]=GetMouseY();
		DrawTextEx(main,TextFormat("mpos: %d, %d\ncurrent obj id:%d\nobj count: %d",mousePosition[0],mousePosition[1],nobjid,),{0,0},18,2,WHITE);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			block nb(nobjid,mousePosition[0],mousePosition[1],0,0,1004,1004);
			blocks.push_back(nb);
		}
		if (nobjid<texCount) {
			nobjid++;
		} else {
			nobjid=1;
		}

        for (int b=0;b<blocks.size();b++) {
            DrawTexturePro(textures[blocks[b].id],{0,0,textures[blocks[b].id].width,textures[blocks[b].id].height},{blocks[b].x,blocks[b].y,textures[blocks[b].id].width*0.6,textures[blocks[b].id].height*0.6},{textures[blocks[b].id].width*0.6/2,textures[blocks[b].id].height*0.6/2},blocks[b].rotation,WHITE);
        }

		EndDrawing();
	}

	UnloadFont(main);
	CloseWindow();
	return 0;
}
