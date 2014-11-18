all: mainOptFrame  examples
	@echo "BUILT SUCCESSFULLY!"

mainOptFrame: main.cpp
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples:
	make -C ./Examples/
	
clean:
	#make clean -C ./Examples/
	rm -f mainOptFrame