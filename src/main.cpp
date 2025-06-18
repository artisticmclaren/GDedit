#define RAYGUI_IMPLEMENTATION

#include <raylib.h>
#include "raygui.h"
#include "textures.cpp"
#include "network.hpp"
#include <stdio.h>
#include <raymath.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <array>
#include <unistd.h>
#include <thread>
#include <mutex>

std::array<int,2> roundPositions(std::array<int,2> pos)
{
    int xr = (pos[0]/30)*30;
    int yr = (pos[1]/30)*30;
    std::array<int,2> result = {xr,yr};
    return result;
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

        Color toRaylibColor() {
            Color output = {this->red,this->green,this->blue,this->opacity};
            return output;
        }
};

bool inEditMenu = false;

Rectangle getCameraView(float screenWidth, float screenHeight, Camera2D camera) {
    float vw = screenWidth / camera.zoom;
    float vh = screenHeight / camera.zoom;

    Vector2 topLeft = {
        camera.target.x - (vw * 0.5f),
        camera.target.y - (vh * 0.5f)
    };

    Rectangle cameraView = {
        topLeft.x,
        topLeft.y,
        vw,
        vh
    };

    return cameraView;
}

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

    std::string convertToNetwork() {
        std::string output="0 ";
        output.append(std::to_string(id)).append(" ");
        output.append(std::to_string(x)).append(" ");
        output.append(std::to_string(y)).append(" ");
        output.append(std::to_string(rotation)).append(" ");
        output.append(std::to_string(layer)).append(" ");
        output.append(std::to_string(mainColor)).append(" ");
        output.append(std::to_string(secondaryColor)).append(" ");
        output.append(std::to_string(touchTriggered)).append(" ");
        output.append(std::to_string(red)).append(" ");
        output.append(std::to_string(green)).append(" ");
        output.append(std::to_string(blue));

        return output;
    }

	bool isMouseInside() {
		return false;
	}
};

std::vector<block> blocks;
color colors[1014];

Camera2D camera;

std::array<int,2> mousePosition = {0,0};

int nobjid=1;

float mwheel=0;

std::array<int,2> lrp={0,0};

bool running = true;
std::string message = "Waiting...";
std::mutex msg_mutex;

block NetworkToGame(std::vector<std::string> data) {
    block output;

    output.id=stoi(data[1]);
    output.x=stoi(data[2]);
    output.y=stoi(data[3]);
    output.rotation=stof(data[4]);
    output.layer=stoi(data[5]);
    output.mainColor=stoi(data[6]);
    output.secondaryColor=stoi(data[7]);
    output.touchTriggered=stoi(data[8]);
    output.red=stoi(data[9]);
    output.green=stoi(data[10]);
    output.blue=stoi(data[11]);

    return output;
}

bool networking = true;

std::vector<std::string> splitBy(char delimiter, std::string input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> splitBySpace(std::string input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void listenForData(int sockfd) {
    char buf[1024];
    while (running) {
        memset(buf,0,sizeof(buf));
        int valread = receive_message(sockfd,buf,sizeof(buf));
        if (valread > 0) {
            std::lock_guard<std::mutex> lock(msg_mutex);
            std::cout << buf << std::endl;
            message = buf;
            std::string op = buf;

            std::vector<std::string> tokens;
            std::istringstream iss(op);
            std::string token;
            while (iss >> token) {
                tokens.push_back(token);
            }

            int type = std::stoi(tokens[0]);

            if (type==0) {
                block netb = NetworkToGame(tokens);
                blocks.push_back(netb);
            } else if (type==1) {
                int delid = std::stoi(tokens[1]);
                blocks.erase(blocks.begin()+delid);
            }

        } else if (valread==0) {
            std::cout << "Disconnected from server." << std::endl;
            break;
        }
    }

    close(sockfd);
}

void sendBlockData(int sockfd, block block) {
    std::string msg = block.convertToNetwork();
    send_message(sockfd, msg.c_str());
}

void sendData(int sockfd, std::string data) {
    send_message(sockfd,data.c_str());
}

bool blockInput = false;
bool DoButton(Rectangle bounds, const char *text, const char *tooltip,Font font) {

    bool clicked = GuiButton(bounds, text);

    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, bounds)) {
        blockInput = true;

        Vector2 textSize = MeasureTextEx(GuiGetFont(), tooltip, 18, 1);
        float tooltipX = mouse.x;
        float tooltipY = mouse.y - textSize.y - 8; // Draw *above* the mouse

        if (tooltipX+textSize.x>1280) {
            tooltipX=tooltipX-textSize.x;
        }

        if (tooltipY+textSize.y>720) {
            tooltipY=720;
        }

        if (tooltip!="") {
            DrawRectangle(tooltipX - 4, tooltipY - 4, textSize.x + 8, textSize.y + 8, Fade(DARKGRAY, 0.85f));
            DrawRectangleLines(tooltipX - 4, tooltipY - 4, textSize.x + 8, textSize.y + 8, GRAY);
    
            //DrawText(tooltip,tooltipX,tooltipY,18,WHITE);
            DrawTextEx(font,tooltip,{tooltipX,tooltipY},18,1,WHITE);
        }
    }

    return clicked;
}

int mode=0;

// edit mode
int objectSelected=-1;
int objectCopied=-1;

int keybindEditCheck(int socket) {
    if (IsKeyPressed(KEY_W)) {
        if (objectSelected!=-1 && mode==1) {
            blocks[objectSelected].y-=30;
            std::string netb = std::string("2 ").append(std::to_string(objectSelected)).append(" 0");
            if (networking) send_message(socket,netb.c_str());
        }
    }


    if (IsKeyPressed(KEY_S)) {
        if (objectSelected!=-1 && mode==1) {
            blocks[objectSelected].y+=30;
            std::string netb = std::string("2 ").append(std::to_string(objectSelected)).append(" 1");
            if (networking) send_message(socket,netb.c_str());
        }
    }


    if (IsKeyPressed(KEY_A)) {
        if (objectSelected!=-1 && mode==1) {
            blocks[objectSelected].x-=30;
            std::string netb = std::string("2 ").append(std::to_string(objectSelected)).append(" 2");
            if (networking) send_message(socket,netb.c_str());
        }
    }


    if (IsKeyPressed(KEY_D)) {
        if (objectSelected!=-1 && mode==1) {
            blocks[objectSelected].x+=30;
            std::string netb = std::string("2 ").append(std::to_string(objectSelected)).append(" 3");
            if (networking) send_message(socket,netb.c_str());
        }
    }

    return 0;
}

int pasteBlock(int pbp, int sock) {
    block nb = block(blocks[pbp].id,15,15,blocks[pbp].layer,blocks[pbp].rotation,blocks[pbp].mainColor,blocks[pbp].secondaryColor);

    blocks.push_back(nb);
    if (networking) {
        sendBlockData(sock,nb);
    }

    return 0;
}

void GuiWindowFloating(Vector2 *position, Vector2 *size, bool *minimized, bool *moving, bool *resizing, void (*draw_content)(Vector2, Vector2), Vector2 content_size, Vector2 *scroll, const char* title) {
    #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
        #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
    #endif

    #if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
        #define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 18
    #endif

    int close_title_size_delta_half = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

    // window movement and resize input and collision check
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*moving && !*resizing) {
        Vector2 mouse_position = GetMousePosition();

        Rectangle title_collision_rect = { position->x, position->y, size->x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + close_title_size_delta_half), RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
        Rectangle resize_collision_rect = { position->x + size->x - 20, position->y + size->y - 20, 20, 20 };

        if(CheckCollisionPointRec(mouse_position, title_collision_rect)) {
            *moving = true;
        } else if(!*minimized && CheckCollisionPointRec(mouse_position, resize_collision_rect)) {
            *resizing = true;
        }
    }

    // window movement and resize update
    if(*moving) {
        Vector2 mouse_delta = GetMouseDelta();
        position->x += mouse_delta.x;
        position->y += mouse_delta.y;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *moving = false;

            // clamp window position keep it inside the application area
            if(position->x < 0) position->x = 0;
            else if(position->x > GetScreenWidth() - size->x) position->x = GetScreenWidth() - size->x;
            if(position->y < 0) position->y = 0;
            else if(position->y > GetScreenHeight()) position->y = GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
        }

    } else if(*resizing) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x > position->x)
            size->x = mouse.x - position->x;
        if (mouse.y > position->y)
            size->y = mouse.y - position->y;

        // clamp window size to an arbitrary minimum value and the window size as the maximum
        if(size->x < 100) size->x = 100;
        else if(size->x > GetScreenWidth()) size->x = GetScreenWidth();
        if(size->y < 100) size->y = 100;
        else if(size->y > GetScreenHeight()) size->y = GetScreenHeight();

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *resizing = false;
        }
    }

    // window and content drawing with scissor and scroll area
    if(*minimized) {
        GuiStatusBar((Rectangle){ position->x, position->y, size->x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, title);

        if (GuiButton((Rectangle){ position->x + size->x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - close_title_size_delta_half,
                                   position->y + close_title_size_delta_half,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE },
                                   "#120#")) {
            *minimized = false;
        }

    } else {
        if (GuiWindowBox((Rectangle) { position->x, position->y, size->x, size->y }, title)) {
            inEditMenu=false;
            return;
        }

        // scissor and draw content within a scroll panel
        if(draw_content != NULL) {
            Rectangle scissor = { 0 };
            GuiScrollPanel((Rectangle) { position->x, position->y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, size->x, size->y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT },
                                         NULL,
                                         (Rectangle) { position->x, position->y, content_size.x, content_size.y },
                                         scroll,
                                         &scissor);

            bool require_scissor = size->x < content_size.x || size->y < content_size.y;

            if(require_scissor) {
                BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
            }

            draw_content(*position, *scroll);

            if(require_scissor) {
                EndScissorMode();
            }
        }

        // draw the resize button/icon
        GuiDrawIcon(71, position->x + size->x - 20, position->y + size->y - 20, 1, WHITE);
    }
}

int SaveLevel() {

    std::string saveFile = "";
    std::string gmdSaveFile = "<?xml version='1.0'?><plist version='1.0' gjver='2.0'><dict><k>kCEK</k><i>4</i><k>k2</k><s>GDedit latest</s><k>k4</k><s>b'kS38,1_40_2_125_3_255_11_255_12_255_13_255_4_-1_6_1000_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1001_7_1_15_1_18_0_8_1|1_0_2_102_3_255_11_255_12_255_13_255_4_-1_6_1009_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1002_5_1_7_1_15_1_18_0_8_1|1_40_2_125_3_255_11_255_12_255_13_255_4_-1_6_1013_7_1_15_1_18_0_8_1|1_40_2_125_3_255_11_255_12_255_13_255_4_-1_6_1014_7_1_15_1_18_0_8_1|1_125_2_255_3_0_11_255_12_255_13_255_4_-1_6_1005_5_1_7_1_15_1_18_0_8_1|1_0_2_255_3_255_11_255_12_255_13_255_4_-1_6_1006_5_1_7_1_15_1_18_0_8_1|1_255_2_255_3_255_11_255_12_255_13_255_4_-1_6_1004_7_1_15_1_18_0_8_1|,kA13,0,kA15,0,kA16,0,kA14,,kA6,0,kA7,0,kA25,0,kA17,0,kA18,0,kS39,0,kA2,0,kA3,0,kA8,0,kA4,0,kA9,0,kA10,0,kA22,0,kA23,0,kA24,0,kA27,1,kA40,1,kA41,1,kA42,1,kA28,0,kA29,0,kA31,1,kA32,1,kA36,0,kA43,0,kA44,0,kA45,1,kA33,1,kA34,1,kA35,0,kA37,1,kA38,1,kA39,1,kA19,0,kA26,0,kA20,0,kA21,0,kA11,0;";

    for (int b=0;b<blocks.size();b++) { // TODO: fix gmd file saving
        std::string serial = "";
        std::string gmdSerial = "";
        
        serial.append(std::to_string(blocks[b].id)).append(" ");
        serial.append(std::to_string(blocks[b].x)).append(" ");
        serial.append(std::to_string(blocks[b].y)).append(" ");
        serial.append(std::to_string(blocks[b].layer)).append(" ");
        serial.append(std::to_string(blocks[b].rotation)).append(" ");
        serial.append(std::to_string(blocks[b].mainColor)).append(" ");
        serial.append(std::to_string(blocks[b].secondaryColor)).append(" ");
        serial.append(std::to_string(blocks[b].touchTriggered)).append(" ");
        serial.append(std::to_string(blocks[b].red)).append(" ");
        serial.append(std::to_string(blocks[b].green)).append(" ");
        serial.append(std::to_string(blocks[b].blue)).append(" ");
        serial.append(std::to_string(blocks[b].durration)).append(";");

        if (obj_data[b].id==0) {
            obj_data[b].id=1;
        }

        gmdSerial.append("1,").append(std::to_string(obj_data[b].id)).append(",");
        gmdSerial.append("2,").append(std::to_string(1280-blocks[b].x)).append(",");
        gmdSerial.append("3,").append(std::to_string(720-blocks[b].y)).append(",");
        gmdSerial.append("20,").append(std::to_string(blocks[b].layer)).append(",");
        gmdSerial.append("6,").append(std::to_string(blocks[b].rotation)).append(",");
        gmdSerial.append("21,").append(std::to_string(blocks[b].mainColor)).append(",");
        gmdSerial.append("22,").append(std::to_string(blocks[b].secondaryColor)).append(",");
        gmdSerial.append("11,").append(std::to_string(blocks[b].touchTriggered)).append(",");
        gmdSerial.append("7,").append(std::to_string(blocks[b].red)).append(",");
        gmdSerial.append("8,").append(std::to_string(blocks[b].green)).append(",");
        gmdSerial.append("9,").append(std::to_string(blocks[b].blue)).append(",");
        gmdSerial.append("10,").append(std::to_string(blocks[b].durration)).append(";");

        saveFile.append(serial);
        gmdSaveFile.append(gmdSerial);
        gmdSaveFile.append("'</s><k>k5</k><s>GDedit</s><k>k101</k><s>0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0</s><k>k13</k><t /><k>k21</k><i>2</i><k>k16</k><i>1</i><k>k80</k><i>84</i><k>k83</k><i>611</i><k>k50</k><i>42</i><k>k47</k><t /><k>k48</k><i>4</i><k>kI1</k><r>198.2</r><k>kI2</k><r>94.5185</r><k>kI3</k><r>0.6</r><k>kI6</k><d><k>0</k><s>0</s><k>1</k><s>0</s><k>2</k><s>0</s><k>3</k><s>0</s><k>4</k><s>0</s><k>5</k><s>0</s><k>6</k><s>0</s><k>7</k><s>0</s><k>8</k><s>0</s><k>9</k><s>0</s><k>10</k><s>0</s><k>11</k><s>0</s><k>12</k><s>0</s><k>13</k><s>0</s></d></dict></plist>");
    }

    std::ofstream opfile;
    std::ofstream gmdopfile;
    opfile.open("latest.gds");

    if (opfile.is_open()) {
        opfile << saveFile;
        opfile.close();
    } else {
        std::cout << "Save failed." << std::endl;
        return 1;
    }

    gmdopfile.open("latest.gmd");
    
    if (gmdopfile.is_open()) {
        gmdopfile << gmdSaveFile;
        gmdopfile.close();
    } else {
        std::cout << "Save failed." << std::endl;
        return 1;
    }

    return 0;
}

static Vector2 pos = {1280-250,720-250};
static Vector2 size = {500,500};
static bool minimized = false;
static bool moving = false;
static bool resizing = false;
static Vector2 scroll;

static void EditMenuDrawContent(Vector2 position, Vector2 scroll) {
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 50  + scroll.y, 100, 25 }, "Button 1");
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 100 + scroll.y, 100, 25 }, "Button 2");
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 150 + scroll.y, 100, 25 }, "Button 3");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 200 + scroll.y, 250, 25 }, "A Label");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 250 + scroll.y, 250, 25 }, "Another Label");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 300 + scroll.y, 250, 25 }, "Yet Another Label");
}

int main() {

    std::string ans;
    std::string ip;

    std::cout << "Sinpleplayer (s) / Multiplayer (m): ";
    std::cin >> ans;
    if (ans=="s") {
        networking=false;
    } else {
        std::cout << "IP: ";
        std::cin >> ip;
    }

    if (!init_networking()) return 1;

/*     int sockfd = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET,ip.c_str(),&serv_addr.sin_addr);

    if (connect(sockfd,(sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed, starting singleplayer...\n";
        networking=false;
    }
    
    std::thread netThread(listenForData,sockfd); */

    socket_t sock = connect_to_server(ip.c_str(),8080);
    std::thread netThread(listenForData,sock);

	InitWindow(1280,720,"GDedit");
	SetTargetFPS(60);

    Font mainf=LoadFontEx("assets/fonts/roboto.ttf",18,0,256);

	// initialize textures
    initializeTextures();

    // Set default colors
    colors[1000] = color(40,125,255,0,255); // BG
    colors[1001] = color{0,102,255,0,255}; // G1
    colors[1009] = color{0,102,255,0,255}; // G2
    colors[1010] = color(0,0,0,0,255); // BLACK
    colors[1013] = color(40,125,255,0,255); // MG1
    colors[1014] = color(40,125,255,0,255); // MG2

    // Set default background
    ClearBackground(colors[1000].toRaylibColor());

    GuiLoadStyle("assets/rgui/def.rgs");
    GuiLoadIcons("assets/rgui/def.rgi",true);

    Texture2D ground = LoadTexture("assets/grounds/groundSquare_01_001-uhd.png");

	while (!WindowShouldClose()) {
		ClearBackground(colors[1000].toRaylibColor());

		BeginDrawing();
		mousePosition[0]=GetMouseX();
		mousePosition[1]=GetMouseY();

        mwheel=GetMouseWheelMoveV().y;
        nobjid+=mwheel;

        int active = 1;

        for (int i=0;i<4;i++) {
            DrawTexture(ground,ground.width*i,720-30,colors[1001].toRaylibColor());
        }


        for (int b=0;b<blocks.size();b++) {
            Rectangle dest = {(float)blocks[b].x,(float)blocks[b].y,(float)textures[blocks[b].id].width*0.6,(float)textures[blocks[b].id].height*0.6};
            Vector2 origin = {(float)textures[blocks[b].id].width*0.6/2,(float)textures[blocks[b].id].height*0.6/2};

            if (b==objectSelected) {
                DrawTexturePro(textures[blocks[b].id],{0,0,(float)textures[blocks[b].id].width,(float)textures[blocks[b].id].height},dest,origin,blocks[b].rotation,GREEN);
            } else {
                DrawTexturePro(textures[blocks[b].id],{0,0,(float)textures[blocks[b].id].width,(float)textures[blocks[b].id].height},dest,origin,blocks[b].rotation,WHITE);
            }
        }

        if (DoButton({15,720-65,105,55},"Build","",mainf)) {
            mode=0;
            objectSelected=-1;
        }

        if (DoButton({15+120,720-65,105,55},"Edit","",mainf)) {
            mode=1;
        }

        if (DoButton({15+240,720-65,105,55},"Delete","",mainf)) {
            mode=2;
            objectSelected=-1;
        }

        // edit mode keyboard checking
        
        // keybindEditCheck(sock);

        if (mode==1) {
            if (DoButton({1280-45,10,35,35},"#16#","Copy",mainf)) {
                objectCopied=objectSelected;
            }
            if (DoButton({1280-45,55,35,35},"#17#","Cut",mainf)) {
                objectCopied=objectSelected;
                blocks.erase(blocks.begin()-objectSelected);
                std::string netdata = std::string("1 ").append(std::to_string(objectSelected));
                if (networking) sendData(sock,netdata);
            }
            if (DoButton({1280-45,100,35,35},"#18#","Paste",mainf)) {
                pasteBlock(objectCopied,sock);
            }
            if (DoButton({1280-45,145,35,35},"#220#","Copy + Paste",mainf)) {
                blocks.push_back(blocks[objectSelected]);
                if (networking) sendBlockData(sock,blocks[objectSelected]);
            }
            if (DoButton({1280-45,190,35,35},"#22#","Edit Object",mainf)) {
                inEditMenu=true;
            }
            if (DoButton({1280-45,235,35,35},"#187#","Deselect",mainf)) {
                objectSelected=-1;
            }
        }

        if (DoButton({1280/2-35,15,35,35},"#2#","Save",mainf)) {
            if (SaveLevel()==0) {
                // none
            }
        }

        if (DoButton({1280/2-35+50,15,35,35},"#1#","Load",mainf)) {
            // load
        }
        

        DrawTextEx(mainf,TextFormat("mpos: %d, %d\ncurrent obj id:%d\nobj count: %d",mousePosition[0],mousePosition[1],nobjid,blocks.size()),{0,26},18,2,WHITE);
        DrawFPS(0,0);

        if (!blockInput && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            std::array<int,2> rp = roundPositions(mousePosition);

            if (mode==0) {
                if (nobjid>0 && nobjid<texCount && rp!=lrp) {
                    block nb(nobjid,rp[0]+15,rp[1]+15,0,0,1004,1004);
                    if (networking) sendBlockData(sock,nb);
                    blocks.push_back(nb);
                    lrp=rp;
                }
            } else if (mode==1) {
                for (int b=0;b<blocks.size();b++) {
                    int bx = blocks[b].x;
                    int by = blocks[b].y;

                    if (rp[0]+15==bx && rp[1]+15==by) {
                        objectSelected=b;
                        break;
                    }
                }
            } else if (mode==2) {
                for (int b=0;b<blocks.size();b++) {
                    int bx = blocks[b].x;
                    int by = blocks[b].y;

                    if (rp[0]+15==bx && rp[1]+15==by) {
                        blocks.erase(blocks.begin()+b);
                        if (networking) {
                            std::string netdata = std::string("1 ").append(std::to_string(b));
                            sendData(sock,netdata);

                        }
                    }
                }
            }
        }

        if (inEditMenu) {
            GuiWindowFloating(&pos,&size,&minimized,&moving,&resizing,EditMenuDrawContent,size,&scroll,"Edit Menu");
        }

		EndDrawing();
        if (!inEditMenu) blockInput=false;
	}

    std::cout << "Unloading resources...\n";

	UnloadFont(mainf);
    deinitializeTextures();
	CloseWindow();

    return 0;
}
