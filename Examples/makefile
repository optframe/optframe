all: hfvrpmt #examples

examples: tsp etii pmedcap opths  gfp hfvrpmt

fix_examples: svrpdsp modm

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
	
opmop:
	make -f makeOPMOP.mk
	
gfp:
	make -f makeGFP.mk
	
hfvrpmt:
	make -f makeHFVRPMT.mk
	
modm:
	make -f makeMODM.mk
		
clean:
	rm -f app_*
	rm -f *.o
