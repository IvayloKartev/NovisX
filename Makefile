# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LIBS = -lSDL2 -lSDL2_mixer -lSDL2_ttf

# Source and object files
SRCS = src/main.c src/core/graphics.c src/core/script.c src/core/engine.c src/ui/menu.c
OBJS = $(SRCS:src/%.c=obj/%.o)

# Executable name
TARGET = Game

# Default target
all: $(TARGET)

# Link objects to create final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile source files into object files
obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf obj $(TARGET)

