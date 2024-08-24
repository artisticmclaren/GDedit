echo "Compiling for Linux..."
g++ -std=c++17 src/main.cpp -I./include -o GDedit -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
echo "Compiling for Windows..."
i686-w64-mingw32-gcc -std=c++17 -o GDedit.exe src/main.cpp -I/win32-raylib/include -L/win32-raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
./GDedit
