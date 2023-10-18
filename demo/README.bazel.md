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