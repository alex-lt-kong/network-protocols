CC = gcc
INCS = -I/usr/include/librdkafka
CFLAGS = -O2 -Wall -pedantic -Wextra -Wc++-compat
LDFLAGS = -lrdkafka
#ASAN = -fsanitize=address -g
UBSAN = -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-divide-by-zero -g

TARGETS = forwarder
# Directory to store the built targets
BUILD_DIR = build
# The paths to the built targets
TARGET_PATHS = $(addprefix $(BUILD_DIR)/,$(TARGETS))

#forwarder.out: forwarder.c#
# $(CC) forwarder.c -o forwarder.out $(INCS) $(LDFLAGS) $(CFLAGS) $(ASAN) $(UBSAN)

all: $(TARGET_PATHS)

$(BUILD_DIR)/%: %.c
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $< $(LDFLAGS) $(CFLAGS) $(ASAN) $(UBSAN) $(INCS)

.PHONY: clean
clean:
	rm  $(TARGET_PATHS)