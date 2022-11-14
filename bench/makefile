.PHONY: help
help:
	@echo 'make all'
	@echo 'make hashes'
	@echo 'make hashes-opt'
	@echo 'make sets'
	@echo 'make sets-opt'
	@echo 'make help'

INCL_PATH := -I ../src
AUX_FLAGS :=
EXT := .no-opt
CMPL := $(CXX)
CXXFLAGS := -Wall -Wfatal-errors
LANG_CPP := -x c++
ALL_FLAGS =  $(CXXFLAGS) $(AUX_FLAGS)

.PHONY: all
all:
	$(MAKE) hashes
	$(MAKE) hashes-opt
	$(MAKE) sets
	$(MAKE) sets-opt

GTSC_HTBLS := ../src/oa_hash/oa_hash.c ../src/bit_set/bit_set.c ../src/ch_hash/ch_hash.c ../src/ch_hash/ch_hash_priv.c ../src/ptr_hash/ptr_hash.c ../src/ptr_hash/ptr_hash_priv.c
.PHONY: hashes-opt
hashes-opt: AUX_FLAGS := -O3
hashes-opt: EXT := .opt
hashes-opt: hashes

.PHONY: hashes
hashes: SRC_FILES := $(GTSC_HTBLS) hashes.cpp
hashes: TARGET = hashes$(EXT).bin
hashes: compile

GTSC_SETS := ../src/oa_set/oa_set.c ../src/bit_set/bit_set.c ../src/ch_set/ch_set.c ../src/ch_set/ch_set_priv.c ../src/ptr_set/ptr_set.c ../src/ptr_set/ptr_set_priv.c
.PHONY: sets-opt
sets-opt: AUX_FLAGS := -O3
sets-opt: EXT := .opt
sets-opt: sets

.PHONY: sets
sets: SRC_FILES := $(GTSC_SETS) sets.cpp
sets: TARGET = sets$(EXT).bin
sets: compile

.PHONY: compile
compile:
	$(CMPL) $(INCL_PATH) $(LANG_CPP) $(SRC_FILES) -o $(TARGET) $(ALL_FLAGS)

.PHONY: clean
clean:
	rm -f ./*.bin
