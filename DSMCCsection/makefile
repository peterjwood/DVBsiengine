

%.o:%.c
	gcc -g -I../writer -c -MD $<

%.o:%.cpp
	g++ -g -I../writer -c -MD $<

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard *.c)
all: libDSMCCparser.a
	
OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

%.a: $(OBJECTS)
	ar -rsu $@ $(OBJECTS)

clean:
	rm -f *.o *.d libDSMCCparser.a

-include *.d
