CC = gcc
CXX = g++
LDFLAGS = 
CXXFLAGS = -O2 -Wall -pedantic -Wextra
CFLAGS = -O2 -Wall -pedantic -Wextra
INC = 

SRC_FILES := $(wildcard *.c)
TARGETS := $(basename $(notdir $(SRC_FILES)))
BUILD_DIR = build
TARGET_PATHS = $(addprefix $(BUILD_DIR)/,$(TARGETS))

all: prebuild $(TARGET_PATHS)

prebuild:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%: %.c
	$(CC) $(INC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all prebuild clean