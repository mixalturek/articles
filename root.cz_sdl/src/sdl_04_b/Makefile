TARGET		= sdl04b
CC		= g++
CFLAGS		= $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_04_b.cpp
	$(CC) -o $(TARGET) sdl_04_b.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core