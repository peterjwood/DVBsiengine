

all:Apps


Apps: Source/libsidecode.a writer/libWriter.a DSMCC/libDSMCC.a DSMCCsection/libDSMCCparser.a $(wildcard apps/*.cpp)
	make -C apps

writer/libWriter.a:$(wildcard writer/*.cpp) $(wildcard writer/*.h) writer/makefile
	make -C writer

Source/libsidecode.a:$(wildcard Source/*.cpp) $(wildcard Source/*.h) Source/makefile
	make -C Source

DSMCC/libDSMCC.a:$(wildcard DSMCC/*.cpp) $(wildcard DSMCC/*.h) DSMCC/makefile
	make -C DSMCC

DSMCCsection/libDSMCCparser.a:$(wildcard DSMCCsection/*.cpp) $(wildcard DSMCCsection/*.h) DSMCCsection/makefile
	make -C DSMCCsection

clean:
	make -C apps clean
	make -C writer clean
	make -C Source clean
	make -C DSMCC clean
	make -C DSMCCsection clean

