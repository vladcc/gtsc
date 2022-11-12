# RUN_PREF=valgrind
# AUX_FLAGS=-fsanitize=address

.PHONY: help
help:
	@echo 'make help'
	@echo 'make all'
	@echo 'make all_test'
	@echo 'make gen_doc'
	@echo 'make misc_test'
	@echo 'make arr_algo_test'
	@echo 'make vect_test'
	@echo 'make vset_test'
	@echo 'make bit_set_test'
	@echo 'make cstr_test'
	@echo 'make nslist_test'
	@echo 'make pool_test'
	@echo 'make one_way_pool_test'
	@echo 'make grow_pool_test'
	@echo 'make nbtree_test'
	@echo 'make oa_hash_test'
	@echo 'make ch_hash_test'
	@echo 'make ptr_hash_test'
	@echo 'make oa_set_test'
	@echo 'make ch_set_test'
	@echo 'make ptr_set_test'
	@echo 'make clean'

OBJ_DIR := ./obj
SRC_DIR := ./src
DOC_DIR := ./doc
INCL_DIR := $(SRC_DIR)
TEST_DIR := $(SRC_DIR)/tests

CC := gcc
CFLAGS := -I$(INCL_DIR) -Wall -Wfatal-errors -Wpedantic -Wextra -Werror -g $(AUX_FLAGS)

CMPL := $(CC)

.PHONY: all
all: all_test gen_doc

.PHOY: all_test
all_test: misc_test arr_algo_test vect_test vset_test bit_set_test cstr_test \
nslist_test pool_test one_way_pool_test grow_pool_test nbtree_test \
oa_hash_test ch_hash_test ptr_hash_test oa_set_test ch_set_test ptr_set_test

# <misc>
MISC_BASE := misc
MISC_DIR := $(SRC_DIR)/misc

MISC_H := $(MISC_DIR)/$(MISC_BASE).h

TEST_MISC_C := $(TEST_DIR)/test_$(MISC_BASE).c
TEST_MISC_BIN := $(OBJ_DIR)/test_$(MISC_BASE).bin
.PHONY: misc_test_compile
misc_test_compile: $(TEST_MISC_BIN)

TEST_MISC_DEP = $(TEST_MISC_C)
$(TEST_MISC_BIN): $(TEST_MISC_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: misc_test
misc_test: misc_test_compile misc_test_run

.PHONY: misc_test_run
misc_test_run:
	$(RUN_PREF) $(TEST_MISC_BIN)
# </misc>

# <arr_algo>
ARR_ALGO_BASE := arr_algo
ARR_ALGO_DIR := $(SRC_DIR)/$(ARR_ALGO_BASE)

ARR_ALGO_C := $(ARR_ALGO_DIR)/$(ARR_ALGO_BASE).c
ARR_ALGO_H := $(ARR_ALGO_DIR)/$(ARR_ALGO_BASE).h
ARR_ALGO_O := $(OBJ_DIR)/$(ARR_ALGO_BASE).o

.PHONY: arr_algo_compile
arr_algo_compile: $(ARR_ALGO_O)

$(ARR_ALGO_O): $(ARR_ALGO_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_ARR_ALGO_C := $(TEST_DIR)/test_$(ARR_ALGO_BASE).c
TEST_ARR_ALGO_BIN := $(OBJ_DIR)/test_$(ARR_ALGO_BASE).bin
.PHONY: arr_algo_test_compile
arr_algo_test_compile: $(TEST_ARR_ALGO_BIN)

TEST_ARR_ALGO_DEP := $(ARR_ALGO_O) $(TEST_ARR_ALGO_C)
$(TEST_ARR_ALGO_BIN): $(TEST_ARR_ALGO_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: arr_algo_test
arr_algo_test: arr_algo_test_compile arr_algo_test_run

.PHONY: arr_algo_test_run
arr_algo_test_run:
	$(RUN_PREF) $(TEST_ARR_ALGO_BIN)
# </arr_algo>

# <vect>
VECT_BASE := vect
VECT_DIR := $(SRC_DIR)/vect

VECT_C := $(VECT_DIR)/$(VECT_BASE).c
VECT_H := $(VECT_DIR)/$(VECT_BASE).h
VECT_O := $(OBJ_DIR)/$(VECT_BASE).o

.PHONY: vect_compile
vect_compile: $(VECT_O)

$(VECT_O): $(VECT_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_VECT_C := $(TEST_DIR)/test_$(VECT_BASE).c
TEST_VECT_BIN := $(OBJ_DIR)/test_$(VECT_BASE).bin
.PHONY: vect_test_compile
vect_test_compile: $(TEST_VECT_BIN)

TEST_VECT_DEP := $(ARR_ALGO_O) $(VECT_O) $(TEST_VECT_C)
$(TEST_VECT_BIN): $(TEST_VECT_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: vect_test
vect_test: vect_test_compile vect_test_run

.PHONY: vect_test_run
vect_test_run:
	$(RUN_PREF) $(TEST_VECT_BIN)
# </vect>

# <vset>
VSET_BASE := vset
VSET_DIR := $(SRC_DIR)/vset

VSET_C := $(VSET_DIR)/$(VSET_BASE).c
VSET_H := $(VSET_DIR)/$(VSET_BASE).h
VSET_O := $(OBJ_DIR)/$(VSET_BASE).o

.PHONY: vset_compile
vset_compile: $(VSET_O)

$(VSET_O): $(VSET_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_VSET_C := $(TEST_DIR)/test_$(VSET_BASE).c
TEST_VSET_BIN := $(OBJ_DIR)/test_$(VSET_BASE).bin
.PHONY: vset_test_compile
vset_test_compile: $(TEST_VSET_BIN)

TEST_VSET_DEP := $(ARR_ALGO_O) $(VECT_O) $(VSET_O)
TEST_VSET_DEP += $(TEST_VSET_C)
$(TEST_VSET_BIN): $(TEST_VSET_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: vset_test
vset_test: vset_test_compile vset_test_run

.PHONY: vset_test_run
vset_test_run:
	$(RUN_PREF) $(TEST_VSET_BIN)
# </vset>

# <bit_set>
BIT_SET_BASE := bit_set
BIT_SET_DIR := $(SRC_DIR)/bit_set

BIT_SET_C := $(BIT_SET_DIR)/$(BIT_SET_BASE).c
BIT_SET_H := $(BIT_SET_DIR)/$(BIT_SET_BASE).h
BIT_SET_O := $(OBJ_DIR)/$(BIT_SET_BASE).o

.PHONY: bit_set_compile
bit_set_compile: $(BIT_SET_O)

$(BIT_SET_O): $(BIT_SET_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_BIT_SET_C := $(TEST_DIR)/test_$(BIT_SET_BASE).c
TEST_BIT_SET_BIN := $(OBJ_DIR)/test_$(BIT_SET_BASE).bin
.PHONY: bit_set_test_compile
bit_set_test_compile: $(TEST_BIT_SET_BIN)

TEST_BIT_SET_DEP := $(BIT_SET_O)
TEST_BIT_SET_DEP += $(TEST_BIT_SET_C)
$(TEST_BIT_SET_BIN): $(TEST_BIT_SET_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: bit_set_test
bit_set_test: bit_set_test_compile bit_set_test_run

.PHONY: bit_set_test_run
bit_set_test_run:
	$(RUN_PREF) $(TEST_BIT_SET_BIN)
# </bit_set>

# <cstr>
CSTR_BASE := cstr
CSTR_DIR := $(SRC_DIR)/cstr

CSTR_C := $(CSTR_DIR)/$(CSTR_BASE).c
CSTR_H := $(CSTR_DIR)/$(CSTR_BASE).h
CSTR_O := $(OBJ_DIR)/$(CSTR_BASE).o

.PHONY: cstr_compile
cstr_compile: $(CSTR_O)

$(CSTR_O): $(CSTR_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_CSTR_C := $(TEST_DIR)/test_$(CSTR_BASE).c
TEST_CSTR_BIN := $(OBJ_DIR)/test_$(CSTR_BASE).bin
.PHONY: cstr_test_compile
cstr_test_compile: $(TEST_CSTR_BIN)

TEST_CSTR_DEP := $(CSTR_O)
TEST_CSTR_DEP += $(TEST_CSTR_C)
$(TEST_CSTR_BIN): $(TEST_CSTR_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: cstr_test
cstr_test: cstr_test_compile cstr_test_run

.PHONY: cstr_test_run
cstr_test_run:
	$(RUN_PREF) $(TEST_CSTR_BIN)
# </cstr>

# <nslist>
NSLIST_BASE := nslist
NSLIST_DIR := $(SRC_DIR)/nslist

NSLIST_H := $(NSLIST_DIR)/$(NSLIST_BASE).h

TEST_NSLIST_C := $(TEST_DIR)/test_$(NSLIST_BASE).c
TEST_NSLIST_BIN := $(OBJ_DIR)/test_$(NSLIST_BASE).bin
.PHONY: nslist_test_compile
nslist_test_compile: $(TEST_NSLIST_BIN)

TEST_NSLIST_DEP = $(TEST_NSLIST_C)
$(TEST_NSLIST_BIN): $(TEST_NSLIST_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: nslist_test
nslist_test: nslist_test_compile nslist_test_run

.PHONY: nslist_test_run
nslist_test_run:
	$(RUN_PREF) $(TEST_NSLIST_BIN)
# </nslist>

# <pool>
POOL_BASE := pool
POOL_DIR := $(SRC_DIR)/pool

POOL_C := $(POOL_DIR)/$(POOL_BASE).c
POOL_H := $(POOL_DIR)/$(POOL_BASE).h
POOL_O := $(OBJ_DIR)/$(POOL_BASE).o

.PHONY: pool_compile
pool_compile: $(POOL_O)

$(POOL_O): $(POOL_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_POOL_C := $(TEST_DIR)/test_$(POOL_BASE).c
TEST_POOL_BIN := $(OBJ_DIR)/test_$(POOL_BASE).bin
.PHONY: pool_test_compile
pool_test_compile: $(TEST_POOL_BIN)

TEST_POOL_DEP := $(POOL_O)
TEST_POOL_DEP += $(TEST_POOL_C)
$(TEST_POOL_BIN): $(TEST_POOL_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: pool_test
pool_test: pool_test_compile pool_test_run

.PHONY: pool_test_run
pool_test_run:
	$(RUN_PREF) $(TEST_POOL_BIN)
# </pool>

# <one_way_pool>
ONE_WAY_POOL_BASE := one_way_pool
ONE_WAY_POOL_DIR := $(SRC_DIR)/one_way_pool

ONE_WAY_POOL_C := $(ONE_WAY_POOL_DIR)/$(ONE_WAY_POOL_BASE).c
ONE_WAY_POOL_H := $(ONE_WAY_POOL_DIR)/$(ONE_WAY_POOL_BASE).h
ONE_WAY_POOL_O := $(OBJ_DIR)/$(ONE_WAY_POOL_BASE).o

.PHONY: one_way_pool_compile
one_way_pool_compile: $(ONE_WAY_POOL_O)

$(ONE_WAY_POOL_O): $(ONE_WAY_POOL_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_ONE_WAY_POOL_C := $(TEST_DIR)/test_$(ONE_WAY_POOL_BASE).c
TEST_ONE_WAY_POOL_BIN := $(OBJ_DIR)/test_$(ONE_WAY_POOL_BASE).bin
.PHONY: one_way_pool_test_compile
one_way_pool_test_compile: $(TEST_ONE_WAY_POOL_BIN)

TEST_ONE_WAY_POOL_DEP := $(ONE_WAY_POOL_O)
TEST_ONE_WAY_POOL_DEP += $(TEST_ONE_WAY_POOL_C)
$(TEST_ONE_WAY_POOL_BIN): $(TEST_ONE_WAY_POOL_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: one_way_pool_test
one_way_pool_test: one_way_pool_test_compile one_way_pool_test_run

.PHONY: one_way_pool_test_run
one_way_pool_test_run:
	$(RUN_PREF) $(TEST_ONE_WAY_POOL_BIN)
# </one_way_pool>

# <grow_pool>
GROW_POOL_BASE := grow_pool
GROW_POOL_DIR := $(SRC_DIR)/grow_pool

GROW_POOL_C := $(GROW_POOL_DIR)/$(GROW_POOL_BASE).c
GROW_POOL_H := $(GROW_POOL_DIR)/$(GROW_POOL_BASE).h
GROW_POOL_O := $(OBJ_DIR)/$(GROW_POOL_BASE).o

.PHONY: grow_pool_compile
grow_pool_compile: $(GROW_POOL_O)

$(GROW_POOL_O): $(GROW_POOL_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_GROW_POOL_C := $(TEST_DIR)/test_$(GROW_POOL_BASE).c
TEST_GROW_POOL_BIN := $(OBJ_DIR)/test_$(GROW_POOL_BASE).bin
.PHONY: grow_pool_test_compile
grow_pool_test_compile: $(TEST_GROW_POOL_BIN)

TEST_GROW_POOL_DEP := $(ARR_ALGO_O) $(VECT_O) $(GROW_POOL_O)
TEST_GROW_POOL_DEP += $(TEST_GROW_POOL_C)
$(TEST_GROW_POOL_BIN): $(TEST_GROW_POOL_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: grow_pool_test
grow_pool_test: grow_pool_test_compile grow_pool_test_run

.PHONY: grow_pool_test_run
grow_pool_test_run:
	$(RUN_PREF) $(TEST_GROW_POOL_BIN)
# </grow_pool>

# <nbtree>
NBTREE_BASE := nbtree
NBTREE_DIR := $(SRC_DIR)/nbtree

NBTREE_C := $(NBTREE_DIR)/$(NBTREE_BASE).c
NBTREE_H := $(NBTREE_DIR)/$(NBTREE_BASE).h
NBTREE_O := $(OBJ_DIR)/$(NBTREE_BASE).o

.PHONY: nbtree_compile
nbtree_compile: $(NBTREE_O)

$(NBTREE_O): $(NBTREE_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_NBTREE_C := $(TEST_DIR)/test_$(NBTREE_BASE).c
TEST_NBTREE_BIN := $(OBJ_DIR)/test_$(NBTREE_BASE).bin
.PHONY: nbtree_test_compile
nbtree_test_compile: $(TEST_NBTREE_BIN)

TEST_NBTREE_DEP := $(NBTREE_O)
TEST_NBTREE_DEP += $(TEST_NBTREE_C)
$(TEST_NBTREE_BIN): $(TEST_NBTREE_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: nbtree_test
nbtree_test: nbtree_test_compile nbtree_test_run

.PHONY: nbtree_test_run
nbtree_test_run:
	$(RUN_PREF) $(TEST_NBTREE_BIN)
# </nbtree>

# <oa_hash>
OA_HASH_BASE := oa_hash
OA_HASH_DIR := $(SRC_DIR)/oa_hash

OA_HASH_C := $(OA_HASH_DIR)/$(OA_HASH_BASE).c
OA_HASH_H := $(OA_HASH_DIR)/$(OA_HASH_BASE).h
OA_HASH_O := $(OBJ_DIR)/$(OA_HASH_BASE).o

.PHONY: oa_hash_compile
oa_hash_compile: $(OA_HASH_O)

$(OA_HASH_O): $(OA_HASH_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_OA_HASH_C := $(TEST_DIR)/test_$(OA_HASH_BASE).c
TEST_OA_HASH_BIN := $(OBJ_DIR)/test_$(OA_HASH_BASE).bin
.PHONY: oa_hash_test_compile
oa_hash_test_compile: $(TEST_OA_HASH_BIN)

TEST_OA_HASH_DEP := $(ARR_ALGO_O) $(BIT_SET_O) $(OA_HASH_O)
TEST_OA_HASH_DEP += $(TEST_OA_HASH_C)
$(TEST_OA_HASH_BIN): $(TEST_OA_HASH_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: oa_hash_test
oa_hash_test: oa_hash_test_compile oa_hash_test_run

.PHONY: oa_hash_test_run
oa_hash_test_run:
	$(RUN_PREF) $(TEST_OA_HASH_BIN)
# </oa_hash>

# <ch_hash>
CH_HASH_BASE := ch_hash
CH_HASH_DIR := $(SRC_DIR)/ch_hash

CH_HASH_C := $(CH_HASH_DIR)/$(CH_HASH_BASE).c
CH_HASH_H := $(CH_HASH_DIR)/$(CH_HASH_BASE).h
CH_HASH_O := $(OBJ_DIR)/$(CH_HASH_BASE).o

CH_HASH_PRIV_BASE := $(CH_HASH_BASE)_priv
CH_HASH_PRIV_C := $(CH_HASH_DIR)/$(CH_HASH_PRIV_BASE).c
CH_HASH_PRIV_O := $(OBJ_DIR)/$(CH_HASH_PRIV_BASE).o

.PHONY: ch_hash_compile
ch_hash_compile: $(CH_HASH_O)

$(CH_HASH_O): $(CH_HASH_C) $(CH_HASH_H)
	$(CMPL) -c $< -o $@ $(CFLAGS)

$(CH_HASH_PRIV_O): $(CH_HASH_PRIV_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)
	
TEST_CH_HASH_C := $(TEST_DIR)/test_$(CH_HASH_BASE).c
TEST_CH_HASH_BIN := $(OBJ_DIR)/test_$(CH_HASH_BASE).bin
.PHONY: ch_hash_test_compile
ch_hash_test_compile: $(TEST_CH_HASH_BIN)

TEST_CH_HASH_DEP := $(CH_HASH_O) $(CH_HASH_PRIV_O)
TEST_CH_HASH_DEP += $(TEST_CH_HASH_C)
$(TEST_CH_HASH_BIN): $(TEST_CH_HASH_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: ch_hash_test
ch_hash_test: ch_hash_test_compile ch_hash_test_run

.PHONY: ch_hash_test_run
ch_hash_test_run:
	$(RUN_PREF) $(TEST_CH_HASH_BIN)
# </ch_hash>

# <ptr_hash>
PTR_HASH_BASE := ptr_hash
PTR_HASH_DIR := $(SRC_DIR)/ptr_hash

PTR_HASH_C := $(PTR_HASH_DIR)/$(PTR_HASH_BASE).c
PTR_HASH_H := $(PTR_HASH_DIR)/$(PTR_HASH_BASE).h
PTR_HASH_O := $(OBJ_DIR)/$(PTR_HASH_BASE).o

PTR_HASH_PRIV_BASE := $(PTR_HASH_BASE)_priv
PTR_HASH_PRIV_C := $(PTR_HASH_DIR)/$(PTR_HASH_PRIV_BASE).c
PTR_HASH_PRIV_O := $(OBJ_DIR)/$(PTR_HASH_PRIV_BASE).o

.PHONY: ptr_hash_compile
ptr_hash_compile: $(PTR_HASH_O)

$(PTR_HASH_O): $(PTR_HASH_C) $(PTR_HASH_H)
	$(CMPL) -c $< -o $@ $(CFLAGS)

$(PTR_HASH_PRIV_O): $(PTR_HASH_PRIV_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)
	
TEST_PTR_HASH_C := $(TEST_DIR)/test_$(PTR_HASH_BASE).c
TEST_PTR_HASH_BIN := $(OBJ_DIR)/test_$(PTR_HASH_BASE).bin
.PHONY: ptr_hash_test_compile
ptr_hash_test_compile: $(TEST_PTR_HASH_BIN)

TEST_PTR_HASH_DEP := $(PTR_HASH_O) $(PTR_HASH_PRIV_O)
TEST_PTR_HASH_DEP += $(TEST_PTR_HASH_C)
$(TEST_PTR_HASH_BIN): $(TEST_PTR_HASH_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: ptr_hash_test
ptr_hash_test: ptr_hash_test_compile ptr_hash_test_run

.PHONY: ptr_hash_test_run
ptr_hash_test_run:
	$(RUN_PREF) $(TEST_PTR_HASH_BIN)
# </ptr_hash>

# <oa_set>
OA_SET_BASE := oa_set
OA_SET_DIR := $(SRC_DIR)/oa_set

OA_SET_C := $(OA_SET_DIR)/$(OA_SET_BASE).c
OA_SET_H := $(OA_SET_DIR)/$(OA_SET_BASE).h
OA_SET_O := $(OBJ_DIR)/$(OA_SET_BASE).o

.PHONY: oa_set_compile
oa_set_compile: $(OA_SET_O)

$(OA_SET_O): $(OA_SET_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)

TEST_OA_SET_C := $(TEST_DIR)/test_$(OA_SET_BASE).c
TEST_OA_SET_BIN := $(OBJ_DIR)/test_$(OA_SET_BASE).bin
.PHONY: oa_set_test_compile
oa_set_test_compile: $(TEST_OA_SET_BIN)

TEST_OA_SET_DEP := $(ARR_ALGO_O) $(BIT_SET_O) $(OA_SET_O)
TEST_OA_SET_DEP += $(TEST_OA_SET_C)
$(TEST_OA_SET_BIN): $(TEST_OA_SET_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: oa_set_test
oa_set_test: oa_set_test_compile oa_set_test_run

.PHONY: oa_set_test_run
oa_set_test_run:
	$(RUN_PREF) $(TEST_OA_SET_BIN)
# </oa_set>

# <ch_set>
CH_SET_BASE := ch_set
CH_SET_DIR := $(SRC_DIR)/ch_set

CH_SET_C := $(CH_SET_DIR)/$(CH_SET_BASE).c
CH_SET_H := $(CH_SET_DIR)/$(CH_SET_BASE).h
CH_SET_O := $(OBJ_DIR)/$(CH_SET_BASE).o

CH_SET_PRIV_BASE := $(CH_SET_BASE)_priv
CH_SET_PRIV_C := $(CH_SET_DIR)/$(CH_SET_PRIV_BASE).c
CH_SET_PRIV_O := $(OBJ_DIR)/$(CH_SET_PRIV_BASE).o

.PHONY: ch_set_compile
ch_set_compile: $(CH_SET_O)

$(CH_SET_O): $(CH_SET_C) $(CH_SET_H)
	$(CMPL) -c $< -o $@ $(CFLAGS)

$(CH_SET_PRIV_O): $(CH_SET_PRIV_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)
	
TEST_CH_SET_C := $(TEST_DIR)/test_$(CH_SET_BASE).c
TEST_CH_SET_BIN := $(OBJ_DIR)/test_$(CH_SET_BASE).bin
.PHONY: ch_set_test_compile
ch_set_test_compile: $(TEST_CH_SET_BIN)

TEST_CH_SET_DEP := $(CH_SET_O) $(CH_SET_PRIV_O)
TEST_CH_SET_DEP += $(TEST_CH_SET_C)
$(TEST_CH_SET_BIN): $(TEST_CH_SET_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: ch_set_test
ch_set_test: ch_set_test_compile ch_set_test_run

.PHONY: ch_set_test_run
ch_set_test_run:
	$(RUN_PREF) $(TEST_CH_SET_BIN)
# </ch_set>

# <ptr_set>
PTR_SET_BASE := ptr_set
PTR_SET_DIR := $(SRC_DIR)/ptr_set

PTR_SET_C := $(PTR_SET_DIR)/$(PTR_SET_BASE).c
PTR_SET_H := $(PTR_SET_DIR)/$(PTR_SET_BASE).h
PTR_SET_O := $(OBJ_DIR)/$(PTR_SET_BASE).o

PTR_SET_PRIV_BASE := $(PTR_SET_BASE)_priv
PTR_SET_PRIV_C := $(PTR_SET_DIR)/$(PTR_SET_PRIV_BASE).c
PTR_SET_PRIV_O := $(OBJ_DIR)/$(PTR_SET_PRIV_BASE).o

.PHONY: ptr_set_compile
ptr_set_compile: $(PTR_SET_O)

$(PTR_SET_O): $(PTR_SET_C) $(PTR_SET_H)
	$(CMPL) -c $< -o $@ $(CFLAGS)

$(PTR_SET_PRIV_O): $(PTR_SET_PRIV_C)
	$(CMPL) -c $^ -o $@ $(CFLAGS)
	
TEST_PTR_SET_C := $(TEST_DIR)/test_$(PTR_SET_BASE).c
TEST_PTR_SET_BIN := $(OBJ_DIR)/test_$(PTR_SET_BASE).bin
.PHONY: ptr_set_test_compile
ptr_set_test_compile: $(TEST_PTR_SET_BIN)

TEST_PTR_SET_DEP := $(PTR_SET_O) $(PTR_SET_PRIV_O)
TEST_PTR_SET_DEP += $(TEST_PTR_SET_C)
$(TEST_PTR_SET_BIN): $(TEST_PTR_SET_DEP)
	$(CMPL) $^ -o $@ $(CFLAGS)

.PHONY: ptr_set_test
ptr_set_test: ptr_set_test_compile ptr_set_test_run

.PHONY: ptr_set_test_run
ptr_set_test_run:
	$(RUN_PREF) $(TEST_PTR_SET_BIN)
# </ptr_set>

CATCH := Module:
ALL_DOC_H := $(shell find $(SRC_DIR)/ -name '*.h' | xargs grep -l '$(CATCH)')
.PHONY: gen_doc
gen_doc: 
	@for file in $(ALL_DOC_H); do \
		base=$$(basename -s '.h' $${file}); \
		targ=$(DOC_DIR)/$${base}.txt; \
		if [ ! -f $$targ ] || [ $$file -nt $$targ ]; then \
			cmd="awk -f tools/make-doc.awk $$file > $$targ"; \
			echo "$$cmd"; \
			eval "$$cmd"; \
		fi \
	done

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*
