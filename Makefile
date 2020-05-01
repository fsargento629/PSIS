output: client.c
	gcc -W -g client.c UI/UI_library.c  -o client -lSDL2 -lSDL2_image
