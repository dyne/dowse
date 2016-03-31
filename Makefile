DESTDIR?=
PREFIX?=/usr/local/dowse
CONFDIR?=/etc/dowse

all: sources
	@echo
	@echo "Dowse is compiled and ready to run"
	@echo "setup conf/settings and launch ./start.sh"
	@echo "or use interactive zsh mode:"
	@echo "source dowse"
	@echo "dowse-start"
	@echo "dowse-[tab] for completion of other commands"
	@echo

sources:
	@mkdir -p build
	make -C src

clean:
	@rm -rf build
	make -C src clean

install:
	install -d ${DESTDIR}${PREFIX}
	install -d ${DESTDIR}${PREFIX}/bin
	install -p -m 644 dowse ${DESTDIR}${PREFIX}/.zshrc
	make -C src     install
	make -C zlibs   install
	make -C daemons install
	install -d ${CONFDIR}
	install -d ${CONFDIR}/blocklists
	install -p -m 644 conf/settings     ${CONFDIR}/settings
	install -p -m 644 conf/network      ${CONFDIR}/network
	install -p -m 644 conf/blocklists/* ${CONFDIR}/blocklists
	@modules/install.sh ${CONFDIR}
	install -d ${DESTDIR}${PREFIX}/db
	install    -p -m 644 build/*.zkv         ${DESTDIR}${PREFIX}/db
	install -s -p -m 755 src/dowse-to-osc    ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 src/dowse-to-gource ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/dnscap        ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/dowse.so      ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/dnscrypt-proxy ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/dnsmasq      ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/redis-server ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/redis-cli    ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/tor          ${DESTDIR}${PREFIX}/bin
	install    -p -m 755 build/pglcmd       ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755 build/pgld         ${DESTDIR}${PREFIX}/bin
	install -s -p -m 6755 build/sup         ${DESTDIR}${PREFIX}/bin
# here sup is installed with suid bit. sup is a secure application we
# use for privilege escalation when needed. sup executes only certain
# binaries on a fixed path and they must match a sha256 hash which is
# compiled in. for more information see: https://github.com/dyne/sup
# to see what binaries are compiled in, do `sup -l`
