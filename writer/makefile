

%.o:%.c
	gcc -g -c -MD $<

%.o:%.cpp
	g++ -g -c -MD $<

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard *.c)
all: libWriter.a
	
OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

%.a: $(OBJECTS)
	ar -rsu $@ $(OBJECTS)

clean:
	rm -f *.o *.d libWriter.a

-include *.d
