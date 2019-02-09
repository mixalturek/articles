TARGET		= sdl05b
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image

$(TARGET): sdl_05_b.cpp functions.h functions.cpp
	$(CC) -o $(TARGET) sdl_05_b.cpp functions.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core