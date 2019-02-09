TARGET		= sdl08a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -L/usr/X11R6/lib -lGL -lGLU

$(TARGET): sdl_08_a.cpp
	$(CC) -o $(TARGET) sdl_08_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core