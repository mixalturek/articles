TARGET		= sdl09a
CC		= g++
CFLAGS		= $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_09_a.cpp
	$(CC) -o $(TARGET) sdl_09_a.cpp $(CFLAGS) $(LDFLAGS) -lSDL_ttf

clean:
	rm -f $(TARGET) core