CC = g++
CFLAGS = -Wall
PROG = octree

SRCS = main.cpp  vec3f.cpp cube.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT -l/home/Documents/AntTweakBar/lib
else
	LIBS = -lglut	-lGL	-lGLU	-lAntTweakBar
endif

INC = -I/home/Documents/AntTweakBar/include

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) $(INC)

clean:
	rm -f $(PROG)

