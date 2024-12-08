build:
	gcc -c ./pack.s -o ./pack_sdom.o
	gcc -fPIC -o ./pack.o -c ./pack.c -I../../serpent -I../../include/
	gcc -Wall -fPIC -D SRPT_PACKAGED -o ./pack ../renderer.c ../../serpent/serpent.c ../../sdom.c ../../utils.c ../../include/glad/glad.c ./pack.o ../shaders.o ./pack_sdom.o  -lglfw -lwayland-client -ldl -rdynamic -I../../include/ -I../../serpent
