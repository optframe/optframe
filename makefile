all: test


test:
	cd tests && make test

install:
	echo "installing OptFrame headers in /usr/local/include/"
	./install.sh && echo "Finished Installation"

