## OptFrame Demo 01 - Quickstart Welcome

![C++17](https://img.shields.io/badge/std-c%2B%2B17-blue)

This demo is intended to work with bazel remote OptFrame repository, instead of local.

Note that [WORKSPACE.bazel](./WORKSPACE.bazel) includes `git_repository` pointing to `master` branch of
`https://github.com/optframe/optframe.git`.

Other demos do not have this behavior, as they are intended to run on local copies of OptFrame,
related as `../../`

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

### Learning more

For more information, check [OptFrame Quickstart on ReadTheDocs](https://optframe.readthedocs.io/en/latest/quickstart.html).

### License

This is dual-licensed LGPLv3 and MIT.

Please use the following SPDX identifier:
`SPDX-License-Identifier: MIT OR LGPL-3.0-or-later`

Copyleft 2023 - OptFrame
