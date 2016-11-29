BINPATH?=bin/
INCPATH=-I../Source -I../writer -I../DSMCC -I../DSMCCsection
LIBPATH=-Wl,-L../Source -Wl,-L../writer -Wl,-L../DSMCC -Wl,-L../DSMCCsection
LIBS=-Wl,-lz -Wl,-lsidecode -Wl,-lWriter -Wl,-lDSMCC -Wl,-lDSMCCparser
LIBDEPS=../Source/libsidecode.a ../writer/libWriter.a ../DSMCC/libDSMCC.a ../DSMCCsection/libDSMCCparser.a
DESTDIR?=/

%.o:%.c
	gcc -g $(INCPATH) -c -MD $<

%.o:%.cpp
	g++ -g $(INCPATH) -c -MD $<

all: $(BINPATH) $(BINPATH)fifoparser $(BINPATH)parser $(BINPATH)depacket $(BINPATH)extractPIDS $(BINPATH)extractDSMCC $(BINPATH)parseDSMCC $(BINPATH)decomp $(BINPATH)parseDSMCCOC $(BINPATH)sectionFilter $(BINPATH)DSMCCprocess

$(BINPATH):
	mkdir -p $(BINPATH)

OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

$(BINPATH)parser: parser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)fifoparser: fifoparser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)decomp: decomp.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)depacket: depacket.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)extractPIDS: extractPIDS.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)extractDSMCC: extractDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)parseDSMCC: parseDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)parseDSMCCOC: parseDSMCCOC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)DSMCCprocess: DSMCCprocess.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)sectionFilter: sectionFilter.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)


clean:
	rm -f *.o *.d  $(BINPATH)fifoparser $(BINPATH)parser $(BINPATH)depacket $(BINPATH)extractPIDS $(BINPATH)extractDSMCC $(BINPATH)parseDSMCC $(BINPATH)decomp $(BINPATH)parseDSMCCOC $(BINPATH)DSMCCprocess $(BINPATH)sectionFilter 
	if [ -d $(BINPATH) ]; then rmdir -p --ignore-fail-on-non-empty $(BINPATH); fi

install:
	if [ ! -d $(DESTDIR)/usr/bin ]; then mkdir -p $(DESTDIR)/usr/bin; fi
	cp $(BINPATH)/* $(DESTDIR)/usr/bin

-include *.d
