CC = g++
CFLAGS = -Wall
PROG = collision_detection

SRCS = main.cpp  vec3f.cpp text3d.cpp

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

