# Install GCC-10

## OptFrame FCore dependencies

OptFrame Functional Core (OptFCore) depends on GCC-10 features (`c++20`).

To install it on your system (if it's not present already) follow these steps (for ubuntu/Linux).

Install dependencies:

- `sudo apt install build-essential wget m4 flex bison`

Download and configure gcc-10 from mirror (http://gcc.gnu.org/mirrors.html):

- `cd /tmp`
- `wget https://ftpmirror.gnu.org/gcc/gcc-10.1.0/gcc-10.1.0.tar.xz`
- `tar xf gcc-10.1.0.tar.xz`
- `cd gcc-10.1.0`
- `contrib/download_prerequisites`


Install gcc on `/usr/local/gcc-10`:

- `cd /tmp`
- `mkdir build && cd build`
- `../gcc-10.1.0/configure -v --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --prefix=/usr/local/gcc-10.1.0 --enable-checking=release --enable-languages=c,c++,fortran --disable-multilib --program-suffix=-10.1`
- `make -j 8`
- `sudo make install-strip`

Add these two lines on user `~/.bashrc`:

- `export export PATH=/usr/local/gcc-10.1.0/bin:$PATH`
- `export LD_LIBRARY_PATH=/usr/local/gcc-10.1.0/lib64:$LD_LIBRARY_PATH`

Then, load user exports: `source ~/.bashrc`

## More Information

Tutorial: https://solarianprogrammer.com/2016/10/07/building-gcc-ubuntu-linux/
