PIDDIR = @PIDDIR@
LOGDIR = @LOGDIR@
TMPDIR = @TMPDIR@
BLOCKLISTS_DIR = @BLOCKLISTS_DIR@
LOCAL_BLOCKLIST_DIR = @LOCAL_BLOCKLIST_DIR@
MASTER_BLOCKLIST_DIR = @MASTER_BLOCKLIST_DIR@
LSB = @LSBFILE@

if DBUSMAKE
DBUS_SET = 1
else
DBUS_SET = 0
endif

if LOGROTATEMAKE
logdir = $(sysconfdir)/logrotate.d
dist_log_DATA = logrotate/pglcmd
endif

if CRONMAKE
crondir = $(sysconfdir)/cron.daily
cron_SCRIPTS = cron.daily/pglcmd
endif

if NETWORKMANAGERMAKE
netdir = $(sysconfdir)/NetworkManager/dispatcher.d
net_SCRIPTS = networkmanager/20pglcmd
endif

initddir = @INITDDIR@
initd_SCRIPTS = init/pgl

pglcmddir = $(bindir)
pglcmd_SCRIPTS = pglcmd

pglcmdwddir = $(sbindir)
pglcmdwd_SCRIPTS = pglcmd.wd

pgletcdir = $(sysconfdir)/$(PACKAGE_NAME)
dist_pgletc_DATA = \
	allow.p2p \
	blocklists.list \
	pglcmd.conf

pgllibdir = $(libdir)/$(PACKAGE_NAME)
pgllib_DATA = \
	pglcmd.defaults \
	pglcmd.lib \
	pglcmd.lsb.lib \
	pglcmd.main

if HAVE_SYSTEMD
systemdsystemunit_DATA = \
	init/pgl.service \
	init/pgl-update.service \
	init/pgl-update.timer

systemdsystemunitdir = @SYSTEMDUNITDIR@
endif

# Don't update PATH here anymore, because on user's make it doesn't contain
# [/usr]/sbin
pglcmd.defaults:
	sed \
	-e 's|^BINDIR=.*|BINDIR=\"$(bindir)\"|g' \
	-e 's|^SBINDIR=.*|SBINDIR=\"$(sbindir)\"|g' \
	-e 's|^PIDDIR=.*|PIDDIR=\"$(PIDDIR)\"|g' \
	-e 's|^LOGDIR=.*|LOGDIR=\"$(LOGDIR)\"|g' \
	-e 's|^CONFDIR=.*|CONFDIR=\"$(pgletcdir)\"|g' \
	-e 's|^LIBDIR=.*|LIBDIR=\"$(pgllibdir)\"|g' \
	-e 's|^TMPDIR=.*|TMPDIR=\"$(TMPDIR)\"|g' \
	-e 's|^BLOCKLISTS_DIR=.*|BLOCKLISTS_DIR=\"$(BLOCKLISTS_DIR)\"|g' \
	-e 's|^LOCAL_BLOCKLIST_DIR=.*|LOCAL_BLOCKLIST_DIR=\"$(LOCAL_BLOCKLIST_DIR)\"|g' \
	-e 's|^MASTER_BLOCKLIST_DIR=.*|MASTER_BLOCKLIST_DIR=\"$(MASTER_BLOCKLIST_DIR)\"|g' \
	-e 's|^LSB=.*|LSB=\"$(LSB)\"|g' \
	-e 's|^DBUS=.*|DBUS=\"$(DBUS_SET)\"|g' \
	-e 's|^VERSION=.*|VERSION=\"$(VERSION)\"|g' \
	< pglcmd.defaults.in > pglcmd.defaults

pglcmd.main:
	sed \
	-e 's|^CMD_DEFAULTS=.*|CMD_DEFAULTS=\"$(pgllibdir)/pglcmd.defaults\"|g' \
	< pglcmd.main.in > pglcmd.main

do_subst = sed -e 's|^CONTROL_MAIN=.*|CONTROL_MAIN=\"$(pgllibdir)/pglcmd.main\"|g'

pglcmd:
	$(do_subst) < pglcmd.in > pglcmd
	chmod +x pglcmd

pglcmd.wd:
	$(do_subst) < pglcmd.wd.in > pglcmd.wd
	chmod +x pglcmd.wd

cron.daily/pglcmd:
	$(do_subst) < cron.daily/pglcmd.in > cron.daily/pglcmd
	chmod +x cron.daily/pglcmd

if GENTOOINIT
init/pgl:
	sed \
	-e 's|@PID_DIR@|$(PIDDIR)|' \
	-e 's|@PGLCMDPATH@|$(bindir)/pglcmd|' \
	< init/pgl.gentoo.in > init/pgl
	chmod +x init/pgl
else
init/pgl:
	$(do_subst) < init/pgl.in > init/pgl
	chmod +x init/pgl
endif

init/pgl.service:
	sed \
	-e 's|@data_root_dir@|$(datarootdir)|g' \
	-e 's|@CONF_DIR@|$(sysconfdir)|g' \
	-e 's|@LIB_DIR@|$(libdir)|g' \
	-e 's|@BIN_DIR@|$(bindir)|g' \
	-e 's|@PID_DIR@|$(PIDDIR)|g' \
	-e 's|@TMP_DIR@|$(TMPDIR)|g' \
	-e 's|@LOG_DIR@|$(LOGDIR)|g' \
	-e 's|@MASTERBLOCKLISTDIR@|$(MASTER_BLOCKLIST_DIR)|g' \
	-e 's|@BLOCKLISTSDIR@|$(BLOCKLISTS_DIR)|g' \
	< init/pgl.service.in > init/pgl.service

init/pgl-update.service:
	sed \
	-e 's|@data_root_dir@|$(datarootdir)|g' \
	-e 's|@BIN_DIR@|$(bindir)|g' \
	-e 's|@LOG_DIR@|$(LOGDIR)|g' \
	-e 's|@MASTERBLOCKLISTDIR@|$(MASTER_BLOCKLIST_DIR)|g' \
	-e 's|@BLOCKLISTSDIR@|$(BLOCKLISTS_DIR)|g' \
	< init/pgl-update.service.in > init/pgl-update.service

networkmanager/20pglcmd:
	$(do_subst) < networkmanager/20pglcmd.in > networkmanager/20pglcmd
	chmod +x networkmanager/20pglcmd

# These files are also distributed
# If the files get modified on "make", only the corresponding *.in files are
# listed here. Otherwise the file that gets installed is listed here.
EXTRA_DIST = \
	cron.daily/pglcmd.in \
	init/pgl.in \
	init/pgl.gentoo.in \
	init/pgl.service.in \
	init/pgl-update.service.in \
	init/pgl-update.timer \
	networkmanager/20pglcmd.in \
	pglcmd.defaults.in \
	pglcmd.in \
	pglcmd.lib \
	pglcmd.lsb.lib \
	pglcmd.main.in \
	pglcmd.wd.in

# These files get removed on clean
CLEANFILES = \
	cron.daily/pglcmd \
	init/pgl \
	init/pgl.gentoo \
	init/pgl.service \
	init/pgl-update.service \
	networkmanager/20pglcmd \
	pglcmd.defaults \
	pglcmd \
	pglcmd.main \
	pglcmd.wd
