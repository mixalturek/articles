TARGET		= sdl15a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image -lSDL_ttf

$(TARGET): sdl_15_a.cpp qmenu.h qmenu.cpp qsdlmenu.h qsdlmenu.cpp
	$(CC) -o $(TARGET) sdl_15_a.cpp qmenu.cpp qsdlmenu.cpp $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core