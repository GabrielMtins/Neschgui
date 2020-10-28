CC = gcc
EXEC ?= neschgui

CCFLAGS := -Wall -Wextra -O2

LIBFLAGS := -lSDL2

SOURCE_DIRECTORY := source

SOURCE := $(wildcard $(SOURCE_DIRECTORY)/*.c)

$(EXEC): $(SOURCE)
	$(CC) $(SOURCE) $(CCFLAGS) $(LIBFLAGS) -o $(EXEC)

install:
	cp neschgui /usr/bin/neschgui

clean:
	rm neschgui
