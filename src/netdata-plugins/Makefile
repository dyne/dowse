DESTDIR ?=
PREFIX ?= /usr/local/dowse

BIN = dowse.plugin

all:
	@echo nothing to do

install:
	@echo installing netdata plugins to ${DESTDIR}${PREFIX}/netdata/libexec/netdata/plugins.d/
	@cp -f ${BIN} ${DESTDIR}${PREFIX}/netdata/libexec/netdata/plugins.d/
	@cd ${DESTDIR}${PREFIX}/netdata/libexec/netdata/plugins.d && chmod 755 ${BIN}

uninstall:
	@echo uninstalling netdata plugins
	@cd ${DESTDIR}${PREFIX}/netdata/libexec/netdata/plugins.d && rm -f ${BIN}

clean:
	@echo cleaning

.PHONY: all install clean
