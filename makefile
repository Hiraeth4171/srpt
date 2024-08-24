debug:
	gcc -Wall -ggdb3 main.c utils.c compile.c render.c sdom.c -o test

clean: 
	rm -f vgcore.*

with-renderer:
	gcc -Wall -ggdb3 main.c utils.c compile.c render.c package.c sdom.c ./renderer/renderer.c ./include/glad/glad.c ./include/utils/io.c -o test `pkg-config --libs --cflags glfw3 wayland-client` -lm -I ./include/


dcomp:
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test compile -o ./test.sdom ./test.srpt 

drender:
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./test render ./test.sdom 

dd:
	make dcomp drender

rrun: 
	./test.sh

