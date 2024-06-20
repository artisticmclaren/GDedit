clear
g++ -std=c++17 main.cpp -o distro/linux/GDedit-linux -lsfml-graphics -lsfml-window -lsfml-system # linux compilation
cp -r ./assets ./distro/linux
cp -r ./include ./distro/linux
cp -r ./lib ./distro/linux

cd ./distro/linux
./GDedit-linux # run on linux
