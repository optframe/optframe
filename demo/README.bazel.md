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
