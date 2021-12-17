all: clean liboptfxcore  appTestLib

appTestLib:
	g++ --std=c++20 -fno-rtti -fcoroutines -fsanitize=address -g -Wfatal-errors -I../ mainlib.cpp -Lbuild -loptfxcore -o appTestLib

liboptfxcore:
	g++ -std=c++20 -I../ -fno-rtti -fcoroutines  -Wfatal-errors -g -Ofast --shared optfxcore_lib.cpp -o build/liboptfxcore.so -fPIC

clean: 
	rm -f appTestLib