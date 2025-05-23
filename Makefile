# Compiler and flags
CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags sdl2 libxml-2.0)
LIBS = $(shell pkg-config --libs sdl2 SDL2_mixer SDL2_ttf libxml-2.0)

# Source and object files
SRCS = src/main.c src/core/graphics.c src/core/parser.c src/core/engine.c src/ui/menu.c src/ui/map.c src/entity/player.c src/screen/page.c src/ui/components/button.c src/ui/components/label.c src/ui/components/dropdown.c
OBJS = $(SRCS:src/%.c=obj/%.o)

# Executable name
TARGET = Game

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile source files into object files
obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf obj $(TARGET)

