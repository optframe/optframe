all: clean build/liboptfxcore.so  appTestLib

appTestLib: build/liboptfxcore.so
#	g++ --std=c++20 -fno-rtti -fcoroutines -fsanitize=address -g -Wfatal-errors -I../ mainlib.cpp -Lbuild -loptfxcore -o appTestLib
	g++ --std=c++20 -fno-rtti -fcoroutines -g  -I../../src  mainlib.cpp -Lbuild -loptfxcore -o appTestLib

build/liboptfxcore.so:
	mkdir -p build/
#	g++ -std=c++20 -I../ -fno-rtti -fcoroutines  -Wfatal-errors -g -Ofast --shared optfxcore_lib.cpp -o build/liboptfxcore.so -fPIC
	g++ -std=c++20 -I../../src -fno-rtti -fcoroutines   -g -Ofast --shared ../../src/OptFCoreLib/optfxcore_lib.cpp -o build/liboptfxcore.so -fPIC

clean: 
	rm -f appTestLib
	rm -f build/liboptfxcore.so
