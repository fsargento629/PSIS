output: main.c client.c
	gcc -W -g main.c client.c UI/UI_library.c  -o client  -pthread -lSDL2 -lSDL2_image
