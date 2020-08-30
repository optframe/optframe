Installation
=============

OptFrame is a standalone C++ project, such that users can simply download a `.zip` from
the `GitHub repository <https://github.com/optframe/optframe>`_ and use contents from `src/`
folder.
Instead of downloading a .zip, we recommend users to use git and clone the project (so that 
they can keep updated with latest fixes).

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

Once you have downloaded the OptFrame package (or cloned repository),
create a folder on your PC which is readable by all users,
for example "C:\Program Files\Common Files\MinGW\OptFrame\". 
Copy the "libs\" and "src\" folders from the zip archive to that location.

When compiling with MinGW, remember to add *-I* option, passing include directories::

   g++ -o myExample example.c -I"C:\Program Files\Common Files\MinGW\OptFrame\src" -I"C:\Program Files\Common Files\MinGW\OptFrame\libs" 

Remember that *src* contains all OptFrame files, and *libs* contains all dependencies.