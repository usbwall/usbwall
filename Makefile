BUILD_DIR = _build
BIN_DIR   = out
PROJECT   = usbwall
VERSION  != git tag | tail -n 1 | cut -d'v' -f2-
FORMAT    = tar.gz

all:
	+make -C $(BUILD_DIR)

distclean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f $(PROJECT)-$(VERSION).$(FORMAT)

dist: distclean
	git archive --format $(FORMAT) -o $(PROJECT)-$(VERSION).$(FORMAT) --prefix $(PROJECT)-$(VERSION)/ origin

doc:
	+make -C $(BUILD_DIR) $@

%:
	+make -C $(BUILD_DIR) $@

.PHONY: doc distclean dist
