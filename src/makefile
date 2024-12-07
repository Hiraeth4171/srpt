OBJECTS=main.c utils.c compile.c render.c package.c sdom.c ./renderer/shaders.o ./renderer/renderer.c ./serpent/serpent.c ./include/glad/glad.c ./include/utils/io.c
FLAGS=-Wall -ggdb3
build:
	gcc $(FLAGS) $(OBJECTS) -o ../bin/srpt `pkg-config --libs --cflags glfw3 wayland-client` -lm -I./include/

clean: 
	rm -f vgcore.*

build-x:
	mkdir -p ../bin/x11
	gcc $(FLAGS) $(OBJECTS) -o ../bin/x11/srptx `pkg-config --libs --cflags glfw3 x11` -lm -I./include/

build-windows-cross-compile:
	x86_64-w64-mingw32-gcc  $(FLAGS) $(OBJECTS) -o ../bin/srpt.exe -L../vendor/windows/glfw3/lib-mingw-w64 -lglfw3  -lopengl32 -lgdi32 -lm -I./include/ -I../vendor/windows/glfw3/include

build-mac-native:
	# TODO: but also maybe just don't i mean who cares really i don't know any 
	# 		mac users


MODULE_DIR=../vendor
setup:
	mkdir -p $(MODULE_DIR)
	mkdir -p ./include/SDT
	mkdir -p ./include/inip
	mkdir -p ./include/fs
	git submodule update --init --recursive --remote
	cp ../vendor/sdt/sdt.h ./include/SDT/
	cp ../vendor/inip/src/include/inip/ini.h ./include/inip/
	cp ../vendor/fs/src/include/fs/fs.h ./include/fs/
