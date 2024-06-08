clear
g++ -std=c++17 main.cpp -o distro/linux/GDedit-linux -lsfml-graphics -lsfml-window -lsfml-system # linux compilation
echo "Linux successful."
x86_64-w64-mingw32-g++ -std=c++17 main.cpp -o distro/win/GDedit-win -static-libgcc -static-libstdc++ -L/home/elijah/cpp/GDedit/lib -I/home/elijah/cpp/GDedit/include -lsfml-graphics -lsfml-window -lsfml-system # windows compilation
echo "Windows successful."

./distro/linux/GDedit-linux # run on linux