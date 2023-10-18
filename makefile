all: test

check: test demos

demos:
	cd demo && make

test:
	cd tests && make test && ./build/OptFrame_test

test-install:
	cd tests && make test-install

install:
	echo "installing OptFrame headers in /usr/local/include/"
	./install.sh && echo "Finished Installation"

lint:
	bazel run @hedron_compile_commands//:refresh_all
	bazel build //... --config clang-tidy

.PHONY: docs

docs:
	# apt-get install python3-sphinx
	# apt-get install texlive texlive-latex-extra latexmk texlive-science
	python3 -m pip install -r docs/requirements.txt
	cd docs && make clean && make html && make latexpdf

clean:
	cd Examples && make clean
	bazel clean --expunge

