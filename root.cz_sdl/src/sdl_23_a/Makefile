TARGET		= sdl23a
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= $(shell sdl-config --libs) -lSDL_image -lzzip

$(TARGET): sdl_23_a.cpp SDL_rwops_zzip.h SDL_rwops_zzip.c
	$(CC) -o $(TARGET) sdl_23_a.cpp SDL_rwops_zzip.c $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET) core