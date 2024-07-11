clear
echo "Compiling for Linux..."
g++ -std=c++17 main.cpp -o distro/linux/GDedit-linux -lsfml-graphics -lsfml-window -lsfml-system 
cp -r ./assets ./distro/linux
cp -r ./lib ./distro/linux
echo "Done."

cd ./distro/linux
./GDedit-linux # run on linux
