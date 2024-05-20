Installation
=============

OptFrame is a standalone C++ project, such that users can simply download a `.zip` from
the `GitHub repository <https://github.com/optframe/optframe>`_ and use contents from `include/`
folder.
Instead of downloading a .zip, we recommend users to use git and clone the project (so that 
they can keep updated with latest fixes).

Building with cxxbuild
----------------------

To use it on a CMake or Bazel project, we recommend installing cxxbuild: `pipx install cxxbuild`

Create the following `cxxdeps.txt` file and run `cxxbuild`::

   OptFrame == "master" [ OptFrame::OptFrameAll ]  git *  https://github.com/optframe/optframe.git
   Catch2 == "v3.5.1" [ Catch2::Catch2WithMain ] cmake+git test https://github.com/catchorg/Catch2.git
   !define LEGACY_ADS
   !std c++20
   !version 1.6.2
   !build cmake

Building with CMake
-------------------

To add OptFrame in your C++ project with CMake, just add the following to your `CMakeLists.txt`::

   Include(FetchContent)
   FetchContent_Declare(OptFrame GIT_REPOSITORY https://github.com/optframe/optframe.git GIT_TAG master)
   FetchContent_MakeAvailable(OptFrame)
   # ...
   target_link_libraries(your_target PRIVATE OptFrame::OptFrameAll)

Building with Bazel
-------------------

To add OptFrame in your C++ project with Bazel, just add the following to your `WORKSPACE.bazel`::

   load('@bazel_tools//tools/build_defs/repo:git.bzl', 'git_repository')
   git_repository(
      name='OptFrame',
      remote='https://github.com/optframe/optframe.git',
      branch='master'
   )

Then, use the desired `deps`, such as `"@OptFrame//include:OptFrame"` and `"@OptFrame//include:OptFCore"`.

Cloning from GitHub
-------------------

To clone OptFrame repository from GitHub:

.. code-block:: shell

   git clone https://github.com/optframe/optframe.git


Then you can run basic tests::

   make test

Output should be: `All tests passed`.


Linux Installation
-------------------

By default, OptFrame installer will put headers on `/usr/local/include/`,
and examples/tests on `/usr/local/optframe/`.

You need sudo to execute the script::

   sudo ./install.sh

You need to accept the installation path::

   Installing OptFrame (with OptFCore) into /usr/local/include and /usr/local/optframe
   Continue? y/n

Message `Finished installation` will confirm that process has finished.


Windows Installation
---------------------

For Windows users it is recommended to follow the steps from `Cloning from GitHub`_ and
use the project locally.

If you want system wide installation, we assume you already have MinGW installed on your PC.
If not, this is covered in the `MinGW Getting Started Wiki <http://www.mingw.org/wiki/Getting_Started>`_.
You'll need both C and C++ compilers (latest MinGW already supports C++17).


Alternative One
^^^^^^^^^^^^^^^

Once you have downloaded the OptFrame package (or cloned repository),
create a folder on your PC which is readable by all users,
for example "C:\Program Files\Common Files\MinGW\OptFrame\". 
Copy the directories "libs/" and "src/" from the zip archive to that location.

When compiling with MinGW, remember to add *-I* option, passing include directories::

   g++ -o myExample example.c -I"C:\Program Files\Common Files\MinGW\OptFrame\src" -I"C:\Program Files\Common Files\MinGW\OptFrame\libs" 

.. hint::
   Remember that *src* contains all OptFrame files, and *libs* contains all its dependencies.

Alternative Two
^^^^^^^^^^^^^^^

A somewhat easier approach is to simply copy content from *libs/* and *src/* to MinGW
default include folder. *Note that this may vary on user installation*, but let's assume *C:\mingw\include* to exist.

Files could be placed as:
- C:\mingw\include\OptFrame
- C:\mingw\include\OptFCore
- C:\mingw\include\catch2
- C:\mingw\include\scannerpplib

If everything works fine, user would be able to use OptFrame without explicit *-I* option::

   g++ -o myExample example.cpp 
