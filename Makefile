# Configuration

CC			= gcc
CFLAGS		= -g -std=gnu99 -Wall -Iinclude -fPIC -pthread
TARGET		= client/proj1client server/proj1server

# Rules
make: $(TARGET)

$(TARGET): client/client.c server/server.c
	$(CC) $(CFLAGS) -o client/proj1client client/client.c
	$(CC) $(CFLAGS) -o server/proj1server server/server.c 

clean:
	@echo "Removing  executables"
	@rm -f client/proj1client server/proj1server
