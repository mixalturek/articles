TARGET		= sdl20b
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image -lSDL_mixer

$(TARGET): sdl_20_b.cpp
	$(CC) -o $(TARGET) sdl_20_b.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core