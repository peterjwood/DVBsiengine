BINPATH?=bin/
PKGNAME:=DVB
INCPATH=-I../Source -I../writer -I../DSMCC -I../DSMCCsection
LIBPATH=-Wl,-L../Source -Wl,-L../writer -Wl,-L../DSMCC -Wl,-L../DSMCCsection
LIBS=-Wl,-lz -Wl,-lsidecode -Wl,-lWriter -Wl,-lDSMCC -Wl,-lDSMCCparser
LIBDEPS=../Source/libsidecode.a ../writer/libWriter.a ../DSMCC/libDSMCC.a ../DSMCCsection/libDSMCCparser.a
DESTDIR?=/

%.o:%.c
	gcc -g $(INCPATH) -c -MD $<

%.o:%.cpp
	g++ -g $(INCPATH) -c -MD $<

all: $(BINPATH) $(BINPATH)$(PKGNAME)fifoparser $(BINPATH)$(PKGNAME)parser $(BINPATH)$(PKGNAME)depacket $(BINPATH)$(PKGNAME)extractPIDS $(BINPATH)$(PKGNAME)extractDSMCC $(BINPATH)$(PKGNAME)parseDSMCC $(BINPATH)$(PKGNAME)decomp $(BINPATH)$(PKGNAME)parseDSMCCOC $(BINPATH)$(PKGNAME)sectionFilter $(BINPATH)$(PKGNAME)DSMCCprocess

$(BINPATH):
	mkdir -p $(BINPATH)

OBJECTS=$(patsubst %.cpp,%.o, $(SOURCES))

$(BINPATH)$(PKGNAME)parser: parser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)$(PKGNAME)fifoparser: fifoparser.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)$(PKGNAME)decomp: decomp.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)$(PKGNAME)depacket: depacket.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)$(PKGNAME)extractPIDS: extractPIDS.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)$(PKGNAME)extractDSMCC: extractDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS) $(LIBPATH)

$(BINPATH)$(PKGNAME)parseDSMCC: parseDSMCC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)$(PKGNAME)parseDSMCCOC: parseDSMCCOC.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)$(PKGNAME)DSMCCprocess: DSMCCprocess.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)

$(BINPATH)$(PKGNAME)sectionFilter: sectionFilter.o $(LIBDEPS)
	g++ -o $@ $< $(LIBS)  $(LIBPATH)


clean:
	rm -f *.o *.d  $(BINPATH)$(PKGNAME)fifoparser $(BINPATH)$(PKGNAME)parser $(BINPATH)$(PKGNAME)depacket $(BINPATH)$(PKGNAME)extractPIDS $(BINPATH)$(PKGNAME)extractDSMCC $(BINPATH)$(PKGNAME)parseDSMCC $(BINPATH)$(PKGNAME)decomp $(BINPATH)$(PKGNAME)parseDSMCCOC $(BINPATH)$(PKGNAME)DSMCCprocess $(BINPATH)$(PKGNAME)sectionFilter 
	if [ -d $(BINPATH) ]; then rmdir -p --ignore-fail-on-non-empty $(BINPATH); fi

install:
	if [ ! -d $(DESTDIR)/usr/bin ]; then mkdir -p $(DESTDIR)/usr/bin; fi
	cp $(BINPATH)/* $(DESTDIR)/usr/bin

-include *.d
