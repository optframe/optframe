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
	cd docs && make clean && make html

clean:
	cd Examples && make clean
	bazel clean --expunge
