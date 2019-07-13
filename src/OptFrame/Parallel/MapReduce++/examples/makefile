all:	MRITest SeqEx01 SeqEx02 SeqWordCounter MapMPEx01 MapMPEx02 MapMPWordCounter MaPIEx01 MaPIEx02 MaPIWordCounter MaPIFSEx01 MaPIFSEx02 MaPIFSEx03

MaPIFSEx03:	MaPIFSEx03.cpp ../MaPI/*
	mpiCC MaPIFSEx03.cpp -o MaPIFSEx03

MaPIFSEx02:	MaPIFSEx02.cpp ../MaPI/*
	mpiCC MaPIFSEx02.cpp -o MaPIFSEx02

MaPIFSEx01:	MaPIFSEx01.cpp ../MaPI/*
	mpiCC MaPIFSEx01.cpp -o MaPIFSEx01

MaPIWordCounter:	MaPIWordCounter.cpp ../MaPI/*
	mpiCC MaPIWordCounter.cpp -o MaPIWordCounter ../util/Scanner++/Scanner.cpp

MaPIEx02:	MaPIEx02.cpp ../MaPI/*
	mpiCC MaPIEx02.cpp -o MaPIEx02 ../util/Scanner++/Scanner.cpp -fopenmp

MaPIEx01:	MaPIEx01.cpp ../MaPI/*
	mpiCC MaPIEx01.cpp -o MaPIEx01 ../util/Scanner++/Scanner.cpp -fopenmp

MapMPWordCounter:	MapMPWordCounter.cpp ../MapMP/*
	g++ MapMPWordCounter.cpp -o MapMPWordCounter ../util/Scanner++/Scanner.cpp -fopenmp

MapMPEx02:	MapMPEx02.cpp ../MapMP/*
	g++ MapMPEx02.cpp -o MapMPEx02 -fopenmp

MapMPEx01:	MapMPEx01.cpp ../MapMP/*
	g++ MapMPEx01.cpp -o MapMPEx01 -fopenmp

SeqWordCounter:	SeqWordCounter.cpp ../SeqMR/* ../mri/*
	g++ SeqWordCounter.cpp -o SeqWordCounter ../util/Scanner++/Scanner.cpp

SeqEx02:	SeqEx02.cpp ../SeqMR/*
	g++ SeqEx02.cpp -o SeqEx02

SeqEx01:	SeqEx01.cpp ../SeqMR/*
	g++ SeqEx01.cpp -o SeqEx01

MRITest:	MRITest.cpp ../mri/*
	g++ MRITest.cpp -o MRITest

clean:	
	rm *~ MRITest SeqEx01 SeqEx02 SeqWordCounter MapMPEx01 MapMPEx02 MapMPWordCounter MaPIEx01 MaPIEx02 MaPIWordCounter MaPIFSEx01 MaPIFSEx02 MaPIFSEx03
