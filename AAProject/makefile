CC = g++
INCDIR = -II:/YingYin/Research/Projects/EclipseProjects/ColorCalib/lib/yingyin/Time/inc
CFLAGS = -O2

default: main

graph.o: graph.cpp graph.h
	$(CC) $(CFALGS) -c graph.cpp
	
main.o: main.cpp graph.h
	$(CC) $(CFALGS) $(INCDIR) -c main.cpp

main: main.o graph.o
	$(CC) $(CFLAGS) -o main main.o graph.o

clean:
	rm *.o *
