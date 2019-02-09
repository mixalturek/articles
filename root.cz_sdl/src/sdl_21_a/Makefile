TARGET		= sdl21a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs)

$(TARGET): sdl_21_a.cpp
	$(CC) -o $(TARGET) sdl_21_a.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core