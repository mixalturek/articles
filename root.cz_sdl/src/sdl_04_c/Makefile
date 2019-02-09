TARGET		= sdl04c
CC		= g++
CFLAGS		= $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_04_c.cpp
	$(CC) -o $(TARGET) sdl_04_c.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core