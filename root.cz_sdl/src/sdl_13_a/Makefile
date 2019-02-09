TARGET		= sdl13a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image

$(TARGET): sdl_13_a.cpp
	$(CC) -o $(TARGET) sdl_13_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core