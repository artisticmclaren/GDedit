#define RAYGUI_IMPLEMENTATION

#include <raylib.h>
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	InitWindow(1280,720,"GDedit");
	SetTargetFPS(60);
	Font main=LoadFontEx("assets/fonts/roboto.ttf",18,0,256);

	while (!WindowShouldClose()) {
		PollInputEvents();

		BeginDrawing();
		DrawTextEx(main,"Hello World!",(Vector2){0,0},(float)main.baseSize,2,RAYWHITE);
		EndDrawing();
	}

	UnloadFont(main);
	CloseWindow();
	return 0;
}
