CC = g++
CFLAGS = -Wall
PROG = collisions

SRCS = main.cpp  vec3f.cpp cube.cpp

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut	-lGL	-lGLU 
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)
