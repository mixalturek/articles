TARGET		= sdl08c
CC		= g++
CFLAGS		= -Wall $(shell sdl-config --cflags)
LDFLAGS		= -L/usr/X11R6/lib $(shell sdl-config --libs) -lGL -lGLU -lm

HEADERS_TO_ALL	= libquark.h sys_dependent.h paths.h qexceptions.h
OBJECTS		= main.o qapplication.o qgridapp.o qini.o qcamera.o qvector.o

# All object files
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# qgridapp
main.o: $(HEADERS_TO_ALL) main.cpp qgridapp.h qgridapp.cpp qapplication.h qapplication.cpp qini.h qini.cpp qvector.h qvector.cpp qcamera.h qcamera.cpp
	$(CC) -c main.cpp $(CFLAGS)

# QIni
qapplication.o: $(HEADERS_TO_ALL) qapplication.h qapplication.cpp qini.h qini.cpp
	$(CC) -c qapplication.cpp $(CFLAGS)

# QApplication
# QImage
# QFont
#qapplicationex.o: $(HEADERS_TO_ALL) qapplicationex.h qapplicationex.cpp qapplication.h qapplication.cpp qini.h qini.cpp qimage.h qimage.cpp qfont.h qfont.cpp
#	$(CC) -c qapplicationex.cpp $(CFLAGS)

# QApplicationEx
# QHeightmap
# QCamera
# QGrid
qgridapp.o: $(HEADERS_TO_ALL) qgridapp.h qgridapp.cpp qapplication.h qapplication.cpp qini.h qini.cpp qvector.h qvector.cpp qcamera.h qcamera.cpp
	$(CC) -c qgridapp.cpp $(CFLAGS)

# QVector
# QImage
# QIni
#qheightmap.o: $(HEADERS_TO_ALL) qheightmap.h qheightmap.cpp qvector.h qvector.cpp qimage.h qimage.cpp qini.h qini.cpp
#	$(CC) -c qheightmap.cpp $(CFLAGS)

# QVector
# QIni
qcamera.o: $(HEADERS_TO_ALL) qcamera.h qcamera.cpp qvector.h qvector.cpp qini.h qini.cpp
	$(CC) -c qcamera.cpp $(CFLAGS)

#
#qgrid.o: $(HEADERS_TO_ALL) qgrid.h qgrid.cpp
#	$(CC) -c qgrid.cpp $(CFLAGS)

#
qini.o: $(HEADERS_TO_ALL) qini.h qini.cpp
	$(CC) -c qini.cpp $(CFLAGS)

#
#qfont.o: $(HEADERS_TO_ALL) qfont.h qfont.cpp
#	$(CC) -c qfont.cpp $(CFLAGS)

#
#qimage.o: $(HEADERS_TO_ALL) qimage.h qimage.cpp
#	$(CC) -c qimage.cpp $(CFLAGS)

#
qvector.o: $(HEADERS_TO_ALL) qvector.h qvector.cpp
	$(CC) -c qvector.cpp $(CFLAGS)

#
#qviewport.o: $(HEADERS_TO_ALL) qviewport.h qviewport.cpp
#	$(CC) -c qviewport.cpp $(CFLAGS)

clean:
	rm -f $(TARGET) *.o core
