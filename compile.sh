echo "Compiling for Linux..."
cp -r ./assets ./distro/linux
g++ -std=c++17 src/main.cpp -o distro/linux/GDedit -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
echo "Compiling for Windows..."
cp -r ./assets ./distro/win
i686-w64-mingw32-g++ -std=c++17 -o distro/win/GDedit.exe src/main.cpp -I/win32-raylib/include -L/win32-raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lstdc++fs -D_GLIBCXX_USE_CXX11_ABI=0
./distro/linux/GDedit
