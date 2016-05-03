
INCPATH=-I../Source -I../writer -I../DSMCC -I../DSMCCsection
LIBPATH=-Wl,-L../Source -Wl,-L../writer -Wl,-L../DSMCC -Wl,-L../DSMCCsection
LIBS=-Wl,-lz -Wl,-lsidecode -Wl,-lWriter -Wl,-lDSMCC -Wl,-lDSMCCparser
LIBDEPS=../Source/libsidecode.a ../writer/libWriter.a ../DSMCC/libDSMCC.a ../DSMCCsection/libDSMCCparser.a

%.o:%.c
	gcc -g $(INCPATH) -c -MD $<

%.o:%.cpp
	g++ -g $(INCPATH) -c -MD $<

all: fifoparser parser depacket extractPIDS extractDSMCC parseDSMCC decomp parseDSMCCOC sectionFilter DSMCCprocess

OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

parser: parser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

fifoparser: fifoparser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

decomp: decomp.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

depacket: depacket.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

extractPIDS: extractPIDS.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

extractDSMCC: extractDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

parseDSMCC: parseDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

parseDSMCCOC: parseDSMCCOC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

DSMCCprocess: DSMCCprocess.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

sectionFilter: sectionFilter.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)


clean:
	rm -f *.o *.d  fifoparser parser depacket extractPIDS extractDSMCC parseDSMCC decomp parseDSMCCOC DSMCCprocess sectionFilter

-include *.d
