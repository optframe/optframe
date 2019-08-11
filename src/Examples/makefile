all: pmedcap examples kp tsp hfvrpmt 

examples: kp tsp etii  pmedcap opths  gfp hfvrpmt hfm modm

fix_examples: svrpdsp modm

tsp:
	make -f makeTSP.mk

kp:
	make -f makeKP.mk

hfm:
	make -f makeHFM.mk

#pn:
#	make -f makePN.mk

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
