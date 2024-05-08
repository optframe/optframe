## OptFrame Demo 03 - Traveling Salesman Problem with ILS/VNS and BRKGA

![C++17](https://img.shields.io/badge/std-c%2B%2B17-blue)

This demo is intended to work with bazel local OptFrame repository `../..`.
### Installing Bazel Build

- For windows, you can use Chocolatey: `choco install bazelisk`
   * Best solution is to install WSL2 and use GNU Linux instructions!

- For mac, you can use Brew: `brew install bazelisk`

- For linux, you can use bazelisk from NPM (that can be installed by NVM):

```
$ curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.5/install.sh | bash
$ bash -i -c "nvm install --lts"
$ bash -i -c "sudo npm install -g @bazel/bazelisk"
```

### Building with cxxbuild

`cxxbuild` is a utilitary tool to automatically generate CMake or Bazel build files.
To use `cxxbuild`, just install it with pip: `pip install cxxbuild`

Then, create a `cxxdeps.txt` file with the following content (choose desired OptFrame version):

```
OptFrame == "5.0.18" [ OptFrameAll ]  git *  https://github.com/optframe/optframe.git
```

Finally, run `cxxbuild` with the desired c++ standard: `cxxbuild . --c++17`
### Learning more

For more information, check [OptFrame Quickstart on ReadTheDocs](https://optframe.readthedocs.io/en/latest/quickstart.html).

### License

This is dual-licensed LGPLv3 and MIT.

Please use the following SPDX identifier:
`SPDX-License-Identifier: MIT OR LGPL-3.0-or-later`

Copyleft 2023 - OptFrame
