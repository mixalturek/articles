TARGET		= sdl19a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_mixer

$(TARGET): sdl_19_a.cpp
	$(CC) -o $(TARGET) sdl_19_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core