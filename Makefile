server:  server.c list.c server_client.c
	gcc server.c server_client.c list.c room.c -lpthread -Wformat -Wall -o server