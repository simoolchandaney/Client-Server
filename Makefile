# Configuration

CC			= gcc
CFLAGS		= -g -std=gnu99 -Wall -Iinclude -fPIC -pthread
TARGET		= proj1client proj1server

# Rules
make: $(TARGET)

$(TARGET): client/client.c server/server.c
	$(CC) $(CFLAGS) -o proj1client client/client.c
	$(CC) $(CFLAGS) -o proj1server server/server.c 

clean:
	@echo "Removing  executables"
	@rm -f proj1client proj1server
