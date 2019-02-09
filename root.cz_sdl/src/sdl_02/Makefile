TARGET		= sdl02
CC		= g++
CFLAGS		= $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_02.cpp
	$(CC) -o $(TARGET) sdl_02.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core