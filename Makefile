user="elijah"

linux:
	$(info Compiling for Linux...)
	g++ -std=c++17 src/main.cpp src/network.cpp -o distro/linux/GDedit -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wno-narrowing
	cp ./assets ./distro/linux -r
windows:
	$(info Compiling for Windows...)
	i686-w64-mingw32-g++ -std=c++17 -o distro/win/GDedit.exe src/main.cpp src/network.cpp -I/home/$(user)/w32-raylib/include -L/home/$(user)/w32-raylib/lib -lraylib -lopengl32 -lgdi32 -lws2_32 -lwinmm -lstdc++fs -D_GLIBCXX_USE_CXX11_ABI=0 -Wno-narrowing -static
	cp ./assets ./distro/win -r
all:
	linux
	windows