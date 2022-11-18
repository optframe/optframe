## Development and Versioning Requirements 

### Versioning requirements

`pip install bumpver`

`bumpver update --patch`

### Docs Requirements

`python3 -m pip install -r requirements.txt`

TODO: use [exhale](https://exhale.readthedocs.io/en/latest/usage.html#usage-quickstart-guide)

Just type: `make docs`

Output is on: `xdg-open $(pwd)/docs/build/html/index.html`

## Build with Bazel

First, install [Bazel Build](https://bazel.build): `npm install -g @bazel/bazelisk`

Also, install bazel buildifier (recommended): `go get -v github.com/bazelbuild/buildtools/buildifier` (should appear on `$HOME/go/bin`) 

To build (currently a small test on `demo/00_SimpleBuild`):
`bazel build ...`

To run: `bazel run //demo/00_SimpleBuild:app_demo_opt`

Resulting binaries will be located at folder `./bazel-bin/demo/00_SimpleBuild/`

To clean:
`bazel clean` or `bazel clean --expunge`