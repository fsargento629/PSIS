output: client_main.c client.c
	gcc -W -g client_main.c client.c UI_library.c Server/common.c  -o client  -pthread -lSDL2 -lSDL2_image
