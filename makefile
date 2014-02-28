all: mainOptFrame  examples
	@echo "BUILT SUCCESSFULLY!"

mainOptFrame:
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples:
	make -C ./Examples/
	
clean:
	#make clean -C ./Examples/
	rm -f mainOptFrame