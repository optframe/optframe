## Development and Versioning Requirements 

### Versioning requirements

`pip install bumpver`

`bumpver update --patch`

### Docs Requirements

`python3 -m pip install -r docs/requirements.txt`

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

### VSCode Setup

We recommend the following setup on Dev Containers (see `.devcontainer` folder).

#### Extensions

We recommend the following vscode extensions:

```{.json}
"extensions": [
    "mine.cpplint",                           // for C++ linting with CPPLINT
    "DevonDCarew.bazel-code",                 // for Bazel files
    "llvm-vs-code-extensions.vscode-clangd",  // for C++ linting with clangd
    "matepek.vscode-catch2-test-adapter"      // for C++ tests
],
```

#### Configurations

On `settings.json` for vscode, we recommend the following:

```{.json}
{
    // ... setttings.json
    "editor.formatOnSave": true,
    "cpplint.cpplintPath": "/usr/local/bin/cpplint",
    "cpplint.root": "${workspaceFolder}/include",
    "C_Cpp.codeAnalysis.clangTidy.useBuildPath": true,
    "C_Cpp.default.compileCommands": "/workspaces/optframe/compile_commands.json",
    "C_Cpp.intelliSenseEngine": "Disabled",
    "clangd.arguments": [
        "-log=verbose",
        "-pretty",
        "--background-index",
        "--compile-commands-dir=${workspaceFolder}/",
    ],
    "testMate.cpp.test.advancedExecutables": [
        {
            "pattern": "{bazel-bin}/**/*{test}*"
        }
    ],
    // ...
}
```

#### Refreshing `compile_commands.json` file and building with bazel

Static checkers require a `compile_commands.json` file.

To build it using bazel, execute: `bazel run @hedron_compile_commands//:refresh_all`

To build with clang-tidy (**very strict mode**), execute: `bazel build //... --config clang-tidy`

To run tests: `bazel test ...`