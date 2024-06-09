clear
g++ -std=c++17 main.cpp -o distro/linux/GDedit-linux -lsfml-graphics -lsfml-window -lsfml-system # linux compilation
cp -r ./assets ./distro/linux
cp -r ./include ./distro/linux
cp -r ./lib ./distro/linux
echo "Linux successful."
x86_64-w64-mingw32-g++ -std=c++17 main.cpp -o distro/win/GDedit-win.exe -static -L/home/elijah/cpp/GDedit/lib -I/home/elijah/cpp/GDedit/include -lsfml-graphics -lsfml-window -lsfml-system # windows compilation
cp -r ./assets ./distro/win
cp -r ./include ./distro/win
cp -r ./lib ./distro/win
echo "Windows successful."

./distro/linux/GDedit-linux # run on linux