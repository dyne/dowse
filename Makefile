DESTDIR?=
PREFIX?=/usr/local/dowse
CONFDIR?=/etc/dowse
THREADS?=1

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

config:
	@echo "configuring environment for the calling user: ${USER} ${DOWSE_HOME}"
	@sed -i -e "s:DOWSE_USER .*:DOWSE_USER = ${USER}:" config.mk
	@sed -i -e "s:DOWSE_HOME .*:DOWSE_HOME = ${HOME}:" config.mk
	@mkdir -p build/bin
	@mkdir -p build/db

sources:
	THREADS=${THREADS} make -C src

clean:
	@rm -rf build
	make -C src clean

install-zlibs:
	make -C zlibs   install
	make -C daemons install
	make -C zuper   install

install: install-zlibs
	install -d ${DESTDIR}${PREFIX}
	install -d ${DESTDIR}${PREFIX}/bin
	install -p -m 644 dowse ${DESTDIR}${PREFIX}/zshrc
	install -p -m 755 pendulum ${DESTDIR}${PREFIX}/bin/pendulum
	#install -d ${DESTDIR}${PREFIX}/node-red
	#cp -ra build/node-red/* ${DESTDIR}${PREFIX}/node-red
	#install -d ${DESTDIR}${PREFIX}/nodejs
	#cp -ra build/nodejs/* ${DESTDIR}${PREFIX}/nodejs
	make -C src install
	install -d ${CONFDIR}
	install -d ${CONFDIR}/blocklists
	install -p -m 644 conf/settings.dist     ${CONFDIR}/settings.dist
	install -p -m 644 conf/network.dist      ${CONFDIR}/network.dist
	install -p -m 644 conf/blocklists/*      ${CONFDIR}/blocklists
	@modules/install.sh ${CONFDIR}
	install -d ${DESTDIR}${PREFIX}/db
	install    -p -m 644 build/db/*.zkv   ${DESTDIR}${PREFIX}/db
	install    -p -m 644 build/db/*.idx   ${DESTDIR}${PREFIX}/db
	install -s -p -m 755 build/bin/*      ${DESTDIR}${PREFIX}/bin
	install    -p -m 644 build/nmap-mac   ${DESTDIR}/usr/share/nmap/nmap-mac-prefixes
	./set_privileges.sh                     ${DESTDIR}${PREFIX}
	install -s -p -m 6755 build/sup         ${DESTDIR}${PREFIX}/bin
	install -d ${DESTDIR}${PREFIX}/lib/dnscrypt-proxy
	install -s -p -m 644 src/dnscrypt-plugin/.libs/dnscrypt_dowse.so ${DESTDIR}${PREFIX}/lib/dnscrypt-proxy
	chown -R ${DOWSE_USER}:${DOWSE_USER} ${DESTDIR}${DOWSE_HOME}/.dowse


# here sup is installed with suid bit. sup is a secure application we
# use for privilege escalation when needed. sup executes only certain
# binaries on a fixed path and they must match a sha256 hash which is
# compiled in. for more information see: https://github.com/dyne/sup
# to see what binaries are compiled in, do `sup -l`

uninstall:
	rm -rf ${DESTDIR}${CONFDIR}
	rm -rf ${DESTDIR}${PREFIX}
	rm -rf ${DESTDIR}${DOWSE_HOME}/.dowse

.PHONY: all config sources clean install uninstall
