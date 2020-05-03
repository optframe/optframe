all: pmedcap examples kp tsp hfvrpmt

examples: mitsp kp tsp etii  pmedcap opths  gfp hfvrpmt hfm modm

mitsp:
	g++ --std=c++17 -fconcepts mainMImpTSP.cpp -o app_MiTSP 2> error-gcc.err 

fix_examples: svrpdsp modm

tsp:
	make -f makeTSP.mk

kp:
	make -f makeKP.mk

kp2:
	make -f makeKP2.mk

hfm:
	make -f makeHFM.mk

#pn:
#	make -f makePN.mk

opths:
	make -f makeOptHS.mk

etii:
	make -f makeEtII.mk

pmedcap:
	cd PMedCap && make -f makePMedCap.mk

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
