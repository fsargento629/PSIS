output: main_client.c client.c
	gcc -W -g main_client.c client.c UI/UI_library.c  -o client -lSDL2 -lSDL2_image
