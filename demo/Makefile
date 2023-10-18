all:
	cd 00_SimpleBuild && make
	cd 00_SimpleBuild && bazel build ...
	cd 01_QuickstartWelcome && make
	cd 01_QuickstartWelcome && bazel build ...
	cd 02_QuickstartKP_SA && make
	cd 02_QuickstartKP_SA && bazel build ...
	cd 03_QuickstartTSP_VNS_BRKGA && make
	cd 03_QuickstartTSP_VNS_BRKGA && bazel build ...
	cd 05_Advanced_TSP_checkmodule && make
	cd 05_Advanced_TSP_checkmodule && bazel build ...

readme:
	cat 00_SimpleBuild/README.header.md README.bazel.md README.info.md > 00_SimpleBuild/README.md
	cat 01_QuickstartWelcome/README.header.md README.bazel.md README.info.md > 01_QuickstartWelcome/README.md
	cat 02_QuickstartKP_SA/README.header.md README.bazel.md README.info.md > 02_QuickstartKP_SA/README.md
	cat 03_QuickstartTSP_VNS_BRKGA/README.header.md README.bazel.md README.info.md > 03_QuickstartTSP_VNS_BRKGA/README.md
	cat 05_Advanced_TSP_checkmodule/README.header.md                README.info.md > 05_Advanced_TSP_checkmodule/README.md

clean:
	./clean_all.sh