BUILD_DIR=_build
BIN_DIR=out

all:
	+make -C $(BUILD_DIR)

distclean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

%:
	+make -C $(BUILD_DIR) $@

.PHONY: $(DIRS)
