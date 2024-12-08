build:
	gcc -c ./pack.s -o ./pack.o
	gcc -Wall -ggdb -D SRPT_PACKAGED -o ./pack ../renderer.c ../../serpent/serpent.c ../../sdom.c ../../utils.c ../../include/glad/glad.c ./pack.c ../shaders.o ./pack.o  -lglfw -lwayland-client -ldl -rdynamic -I../../include/ -I../../serpent
