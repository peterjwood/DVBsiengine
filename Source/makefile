

%.o:%.c
	gcc -g -c -I../writer -MD $<

%.o:%.cpp
	g++ -g -c -I../writer -MD $<

SOURCES=$(wildcard *.cpp)
SOURCES+=$(wildcard *.c)
all: libsidecode.a
	
OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

%.a: $(OBJECTS)
	ar -rsu $@ $(OBJECTS)

clean:
	rm -f *.o *.d libsidecode.a

-include *.d
