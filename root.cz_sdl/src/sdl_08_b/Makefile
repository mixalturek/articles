TARGET		= sdl08b
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image -L/usr/X11R6/lib -lGL -lGLU

$(TARGET): sdl_08_b.cpp
	$(CC) -o $(TARGET) sdl_08_b.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core