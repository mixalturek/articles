TARGET		= sdl17a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_17_a.cpp
	$(CC) -o $(TARGET) sdl_17_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core