build:
	gcc -Wall -ggdb3 main.c utils.c compile.c render.c package.c sdom.c ./renderer/shaders.o ./renderer/renderer.c ./serpent/serpent.c ./include/glad/glad.c ./include/utils/io.c -o ../bin/srpt `pkg-config --libs --cflags glfw3 wayland-client` -lm -I/usr/lib/include -I./include/

clean: 
	rm -f vgcore.*

build-x:
	gcc -Wall -ggdb3 main.c utils.c compile.c render.c package.c sdom.c ./renderer/shaders.o ./renderer/renderer.c ./serpent/serpent.c ./include/glad/glad.c ./include/utils/io.c -o ../bin/testx `pkg-config --libs --cflags glfw3 x11` -lm -I/usr/lib/include -I./include/

build-windows-cross-compile:
	x86_64-w64-mingw32-gcc -Wall -ggdb3 main.c utils.c compile.c render.c package.c sdom.c ./renderer/shaders.o ./renderer/renderer.c ./serpent/serpent.c ./include/glad/glad.c ./include/utils/io.c -o ../bin/windows -L../vendor/windows/glfw3/lib-mingw-w64 -lglfw3  -lopengl32 -lgdi32 -lm -I/usr/lib/include -I./include/ -I../vendor/windows/glfw3/include

build-mac-native:
	# TODO: but also maybe just don't i mean who cares really i don't know any 
	# 		mac users

dcomp:
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test compile -o ./test.sdom ./test.srpt 

drender:
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test render ./test.sdom 

dd:
	make dcomp drender

rrun: 
	./test.sh

