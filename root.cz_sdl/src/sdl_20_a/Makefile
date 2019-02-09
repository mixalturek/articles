TARGET		= sdl20a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_mixer

$(TARGET): sdl_20_a.cpp
	$(CC) -o $(TARGET) sdl_20_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core