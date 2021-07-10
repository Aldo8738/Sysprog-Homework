# Compile flags
CC = gcc
CFLAGS = -Wall -std=c11 -g
INCL_DIR = include
CPPFLAGS = -I$(INCL_DIR)

# Folders
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
TEST_OUTPUT_DIR = $(TEST_DIR)/output

SRC = filetree.c \
      os_malloc.c

EXEC = dos_os

TESTS_SRC = $(wildcard $(TEST_DIR)/test_*.c)
TESTS_ELF = $(basename $(notdir $(TESTS_SRC)))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

.PHONY: all run clean submission

all: $(EXEC)

$(EXEC): $(OBJ) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

run: all
	./$(EXEC)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

.PHONY: $(TESTS_ELF) tests_filetree_all tests_malloc_all tests_all
$(TESTS_ELF): %: $(TEST_DIR)/%.c $(OBJ)
	@mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $(TEST_OUTPUT_DIR)/$@
	./$(TEST_OUTPUT_DIR)/$@ > $(TEST_OUTPUT_DIR)/$@.txt
	@echo ""
	@echo ">>> Test output written to $(TEST_OUTPUT_DIR)/$@.txt"

tests_filetree_all: $(filter test_filetree_%, $(TESTS_ELF))
tests_malloc_all: $(filter test_malloc_%, $(TESTS_ELF))
tests_all: $(TESTS_ELF)

clean:
	rm -rf dos_os
	rm -rf abgabe.zip
	rm -rf $(OBJ_DIR)
	rm -rf $(TEST_OUTPUT_DIR)

submission:
	zip abgabe.zip $(addprefix $(SRC_DIR)/, $(SRC))

