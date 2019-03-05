BUILD_DIR = _build
BIN_DIR   = out
PROJECT   = usbwall
VERSION  != git tag | tail -n 1 | cut -d'v' -f2-
FORMAT    = tar.gz

all:
	+$(MAKE) -C $(BUILD_DIR)

distclean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f $(PROJECT).$(FORMAT)
	rm -f $(PROJECT)-$(VERSION).$(FORMAT)

dist: distclean
	git archive --format $(FORMAT) -o $(PROJECT).$(FORMAT) --prefix $(PROJECT)/ HEAD

dist-stable: distclean
	git archive --format $(FORMAT) -o $(PROJECT)-$(VERSION).$(FORMAT) --prefix $(PROJECT)-$(VERSION)/ v$(VERSION)

test:
	+$(MAKE) -C $(BUILD_DIR) usbwall_test

doc:
	+$(MAKE) -C $(BUILD_DIR) $@

%:
	+$(MAKE) -C $(BUILD_DIR) $@

.PHONY: doc distclean dist dist-stable test
