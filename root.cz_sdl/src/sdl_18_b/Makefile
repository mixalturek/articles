TARGET		= sdl18b
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_sound

$(TARGET): sdl_18_b.cpp
	$(CC) -o $(TARGET) sdl_18_b.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core