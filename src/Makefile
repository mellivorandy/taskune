CC = gcc
CFLAGS = -Wall -pthread
OBJS = node.c dispatcher.c server.c task.c cJSON.c

all:
	$(CC) $(OBJS) -o taskune $(CFLAGS)

clean:
	rm -f taskune
