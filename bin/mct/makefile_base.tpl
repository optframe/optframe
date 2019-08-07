
GCC_FLAGS=-O3 --std=c++17 -fconcepts

examples:
	make -C ../Examples/
	
clean: 
	make clean -C ../Examples/
	rm -f app_*
	rm -f *.o
