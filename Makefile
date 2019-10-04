DESTDIR?=
CONFDIR?=/etc/dowse

include config.mk

all: config sources
	@echo
	@echo "Dowse is compiled and ready to run"
	@echo "setup conf/settings and launch ./start.sh"
	@echo "or use interactive zsh mode:"
	@echo "source dowse"
	@echo "dowse-start"
	@echo "dowse-[tab] for completion of other commands"
	@echo

sources:
	make -C src

clean:
	@rm -rf build
	make -C src clean

install:
	install -d ${DESTDIR}${CONFDIR}
	install -d ${DESTDIR}${CONFDIR}/blocklists
	install -p -m 644 conf/settings.dist     ${DESTDIR}${CONFDIR}/settings
	install -p -m 644 conf/network.dist      ${DESTDIR}${CONFDIR}/network.dist
	install -p -m 644 conf/blocklists/*      ${DESTDIR}${CONFDIR}/blocklists
	@modules/install.sh ${DESTDIR}${CONFDIR}
	install -d -o dowse -g dowse ${DESTDIR}/var/lib/dowse
	install -d -o dowse -g dowse ${DESTDIR}/var/log/dowse
	install -d -o dowse -g dowse ${DESTDIR}/var/log/dowse/supervisor

uninstall:
	rm -rf ${DESTDIR}${CONFDIR}

.PHONY: all config sources clean install uninstall
