CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = spyglass_log.c example.c

OBJ = $(SRC:.c=.o)

TARGET = example

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)