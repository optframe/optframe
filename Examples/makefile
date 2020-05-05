all: pmedcap examples kp tsp hfvrpmt

examples: mitsp kp tsp etii  pmedcap opths  gfp hfvrpmt hfm modm

mitsp:
	#g++ --std=c++17 -fconcepts mainMImpTSP.cpp -o app_MiTSP 2> error-gcc.err 
	cd MITSP && make

fix_examples: svrpdsp modm

tsp:
	cd TSP && make

kp:
	cd KP && make

kp2:
	make -f makeKP2.mk

hfm:
	make -f makeHFM.mk

#pn:
#	make -f makePN.mk

opths:
	cd OptHScheduling && make

etii:
	cd EternityII && make

pmedcap:
	cd PMedCap && make

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
