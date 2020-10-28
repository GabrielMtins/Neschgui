#    Copyright (C) 2020  Gabriel Martins

#    This file is part of Neschgui.

#    Neschgui is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.

#    Neschgui is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.

#    You should have received a copy of the GNU General Public License
#    along with Neschgui.  If not, see <https://www.gnu.org/licenses/>.


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
