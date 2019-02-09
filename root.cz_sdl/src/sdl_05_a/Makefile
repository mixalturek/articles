TARGET		= sdl05a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image

$(TARGET): sdl_05_a.cpp functions.h functions.cpp
	$(CC) -o $(TARGET) sdl_05_a.cpp functions.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core