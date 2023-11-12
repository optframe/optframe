## OptFrame C Library

This project puts OptFrame/OptFCore project into a shared library.

This is quite experimental, trying to adapt coroutines and other newest features into optframe lib.
Python project already has its own implementation of OptFrame library, without any C++20 feature (limited to C++17).

### Who uses this project?

This is already used by [pyoptframe](https://github.com/optframe/pyoptframe-dev) project (try `pip install optframe`).

Anyone that wants to use general optframe support directly without embedded compiling all .hpp files.

Python implementation of OptFrame will use this library to provide direct support of OptFrame internals.

For now, due to Python inspiration, main `void*` type is `FakePythonObjPtr`, 
but it could be renamed to `FakeExternalObjPtr`, as soon as other language projects are available.

### License

MIT License