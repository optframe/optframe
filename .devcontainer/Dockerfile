FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# REFRESH!
RUN apt-get update && apt-get upgrade -y && apt-get autoremove -y

# install pip (for cpplint and cmake)
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends python3-pip

# clang-tidy, clangd, clang-tools
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends clang-19 clang-tidy-19 clang-format-19 clangd-19 clang-tools-19 libc++-19-dev

# install nvm (for npm)
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.5/install.sh | bash
RUN bash /root/.nvm/install.sh

# install npm (using nvm)
RUN bash -i -c "nvm install --lts"

# bazel (using npm)
RUN bash -i -c "npm install -g @bazel/bazelisk"

# install cpplint into /usr/local/bin/cpplint
RUN python3 -m pip install cpplint --break-system-packages

# bumpver
RUN python3 -m pip install bumpver --break-system-packages

# TEMPORARY FIX TO PYTHON BABEL: https://github.com/python-babel/babel/issues/990
# ValueError: ZoneInfo keys may not be absolute paths, got: /UTC
#RUN rm -f /etc/localtime
#RUN ln -s /usr/share/zoneinfo/UTC /etc/localtime

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends texlive texlive-latex-extra latexmk texlive-science

# docs requiring tgtermes.sty
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends tex-gyre


# command 'add-apt-repository'
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends software-properties-common


RUN sudo add-apt-repository "deb http://br.archive.ubuntu.com/ubuntu plucky main universe"
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends g++-15 gcc-15

# FORCE UPGRADE (NEVER ON PLUCKY!)
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y upgrade

# default gcc-15
RUN sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 10
RUN sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 10

# default clang-19
RUN sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 10
RUN sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 10
RUN sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-19 10
RUN sudo update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-19 10
RUN sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-19 10

RUN pip install --upgrade cmake --break-system-packages
