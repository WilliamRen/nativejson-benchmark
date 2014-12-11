ifeq ($(CONFIG),)
	CONFIG = release64
endif

ifeq ($(VERBOSE),)
	VERBOSE=1
endif


nativejson_$(CONFIG)_gmake : build/gmake/nativejson.make bin/nativejson_$(CONFIG)_gmake.a
	cd build/gmake && make -f nativejson.make config=$(CONFIG) verbose=$(VERBOSE)

clean : 
	rm -rf build/gmake
	rm -rf build/vs2005
	rm -rf build/vs2008
	rm -rf build/vs2010
	rm -rf intermediate
	rm -rf src/machine.h
	rm -rf bin
	rm -rf result/*.csv
	rm -rf result/*.html

setup :
	cd build && ./premake.sh
	
	
build/gmake/nativejson.make : setup
build/gmake/benchmark.make : setup

	
	
bin/nativejson_$(CONFIG)_gmake.a : build/gmake/benchmark.make
	cd build/gmake && make -f benchmark.make config=$(CONFIG) verbose=$(VERBOSE)
	
	
all : bin/nativejson_$(CONFIG)_gmake
	cd bin && ./nativejson_$(CONFIG)_gmake
	cd result && make -f makefile
	
clean_status :
	@echo "Filesystem status according to GIT"
	@git clean -dfxn