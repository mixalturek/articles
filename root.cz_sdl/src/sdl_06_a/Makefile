TARGET		= sdl06a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image

$(TARGET): sdl_06_a.cpp functions.h functions.cpp
	$(CC) -o $(TARGET) sdl_06_a.cpp functions.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core