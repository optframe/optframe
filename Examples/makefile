all: examples

examples: tsp etii pmedcap opths

tsp:
	make -f makeTSP.mk

opths:
	make -f makeOptHS.mk
	
etii:
	make -f makeEtII.mk
	
pmedcap:
	make -f makePMedCap.mk
	
clean:
	rm -f app_*
	rm -f *.o