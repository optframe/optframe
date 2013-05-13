all: build

build:
	echo Building OptFrame Script Language Interpreter
	g++ opti.cpp -O2 ../OptFrame/Scanner++/Scanner.cpp -o opti
	
install:
	echo Moving 'opti' to '/usr/bin/'
	sudo cp opti /usr/bin/
	echo Installation successful! type: 'opti' to run

uninstall:
	echo Removing 'opti' from '/usr/bin/'
	sudo rm /usr/bin/opti
	echo Uninstallation successful!
