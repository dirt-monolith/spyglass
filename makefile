CC = gcc
CFLAGS = -Wall -Wextra -std=c99

MAYDAY    = SPYGLASS_MAYDAY
SIGHTING  = SPYGLASS_SIGHT
MARK      = SPYGLASS_MARK

CFG_COLOR = SPYGLASS_LENS_COLOR
CFG_TIME  = SPYGLASS_LENS_TIME
CFG_FILE  = SPYGLASS_LENS_FILE
CFG_FUNC  = SPYGLASS_LENS_FUNC
CFG_LINE  = SPYGLASS_LENS_LINE
CFG_FILE  = SPYGLASS_LENS_AIM
CFG_ERR   = SPYGLASS_LENS_AIM_STDERR
CFG_OUT   = SPYGLASS_LENS_AIM_STDOUT

DEBUG_OBSERVATIONS = "($(MAYDAY)|$(SIGHTING)|$(MARK))"
DEBUG_LENS = "($(CFG_COLOR)|$(CFG_TIME)|$(CFG_FILE)|$(CFG_FUNC)|$(CFG_LINE)|$(CFG_FILE))"

TEST_OBSERVATIONS = "($(MAYDAY)|$(SIGHTING))"
TEST_LENS = "($(CFG_TIME)|$(CFG_FILE)|$(CFG_FUNC)|$(CFG_LINE)|$(CFG_FILE))"

SRC = spyglass_log.c example.c
OBJ = $(SRC:.c=.o)
TARGET = example

all: release
debug: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(DEBUG_LENS) \
                 -D SPYGLASS_CONFIG_OBSERVATIONS=$(DEBUG_OBSERVATIONS)
debug: $(TARGET)

test: CFLAGS += -D SPYGLASS_CONFIG_LENS=$(TEST_LENS) \
                 -D SPYGLASS_CONFIG_OBSERVATIONS=$(TEST_OBSERVATIONS)
test: $(TARGET)
release: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all debug test release clean