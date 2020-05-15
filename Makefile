client: client_main.c client.h drawing.h drawing.c client_communication.c client_communication.h Server/common.h Server/common.c
	gcc -W -g client_main.c client_communication.c drawing.c UI_library.c Server/common.c -o client -pthread -lSDL2 -lSDL2_image
