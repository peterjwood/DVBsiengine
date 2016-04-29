

%.o:%.c
	gcc -g -I../writer -c -MD $<

%.o:%.cpp
	g++ -g -I../writer -c -MD $<

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard *.c)
all: libDSMCC.a
	
OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

%.a: $(OBJECTS)
	ar -rsu $@ $(OBJECTS)

clean:
	rm -f *.o *.d libDSMCC.a

-include *.d
