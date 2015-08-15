all: examples

examples: tsp etii pmedcap opths svrpdsp

tsp:
	make -f makeTSP.mk

opths:
	make -f makeOptHS.mk
	
etii:
	make -f makeEtII.mk
	
pmedcap:
	make -f makePMedCap.mk
	
svrpdsp:
	make -f makeSVRPDSP.mk
	
clean:
	rm -f app_*
	rm -f *.o