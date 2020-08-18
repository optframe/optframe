all: test


test:
	cd tests && make test

test-install:
	cd tests && make test-install

install:
	echo "installing OptFrame headers in /usr/local/include/"
	./install.sh && echo "Finished Installation"

.PHONY: docs

docs:
	cd docs && make clean && make html
