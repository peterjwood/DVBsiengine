all:Apps


Apps: Source/libsidecode.a writer/libWriter.a DSMCC/libDSMCC.a DSMCCsection/libDSMCCparser.a
	make -C apps

writer/libWriter.a:
	make -C writer

Source/libsidecode.a:
	make -C Source

DSMCC/libDSMCC.a:
	make -C DSMCC

DSMCCsection/libDSMCCparser.a:
	make -C DSMCCsection

clean:
	make -C apps clean
	make -C writer clean
	make -C Source clean
	make -C DSMCC clean
	make -C DSMCCsection clean

