all: mainOptFrame  examples

mainOptFrame:
	g++ -Wall main.cpp ./OptFrame/Scanner++/Scanner.cpp -o mainOptFrame

examples:
	@make -C ./Examples/
	
clean:
	@make clean -C ./Examples/
	rm -f mainOptFrame