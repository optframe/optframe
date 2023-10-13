all: test


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
	cd docs && python3 -m pip install -r requirements.txt && make clean && make html

clean:
	cd Examples && make clean
	bazel clean --expunge

