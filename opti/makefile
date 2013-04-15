all: build install

build:
	echo Building OptFrame Script Language Interpreter
	g++ opti.cpp ../OptFrame/Scanner++/Scanner.cpp -o opti
	
install:
	echo Moving 'opti' to '/usr/bin/'
	sudo cp opti /usr/bin/
	echo Installation successful! type: 'opti' to run
	