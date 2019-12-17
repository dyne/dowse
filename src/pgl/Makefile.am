BLOCKLISTS_DIR = @BLOCKLISTS_DIR@
LOCAL_BLOCKLIST_DIR = @LOCAL_BLOCKLIST_DIR@
MASTER_BLOCKLIST_DIR = @MASTER_BLOCKLIST_DIR@

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = subdir-objects

# Ensure that make distcheck continues to work
DISTCHECK_CONFIGURE_FLAGS = \
	--with-systemd=$$dc_install_base/$(SYSTEMDUNITDIR)

SUBDIRS = \
	docs \
	pglcmd \
	pgld \
	$(QT_SUB)

# All other docs are added automatically distributed by automake:
dist_doc_DATA = \
	BUGS \
	README.blocklists \
	README.dbus

EXTRA_DIST = \
	autogen.sh \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/gbp.conf \
	debian/pglcmd.config \
	debian/pglcmd.dirs \
	debian/pglcmd.install \
	debian/pglcmd.links \
	debian/pglcmd.maintscript \
	debian/pglcmd.postinst.in \
	debian/pglcmd.templates \
	debian/pgld-dbg.maintscript \
	debian/pgld.dirs \
	debian/pgld.docs \
	debian/pgld.examples \
	debian/pgld.install \
	debian/pgld.manpages \
	debian/pglgui-dbg.maintscript \
	debian/pglgui.install \
	debian/pglgui.links \
	debian/pglgui.maintscript \
	debian/po/POTFILES.in \
	debian/po/templates.pot \
	debian/README.Debian \
	debian/rules \
	debian/source/format \
	debian/watch

# do not recreate existent directories
# this would also mess up custom permissions if present
install-data-local:
	@[ -e $(DESTDIR)$(MASTER_BLOCKLIST_DIR) ] || $(INSTALL) -d $(DESTDIR)$(MASTER_BLOCKLIST_DIR)
	@[ -e $(DESTDIR)$(BLOCKLISTS_DIR) ] || $(INSTALL) -d $(DESTDIR)$(BLOCKLISTS_DIR)
	@[ -e $(DESTDIR)$(LOCAL_BLOCKLIST_DIR) ] || $(INSTALL) -d $(DESTDIR)$(LOCAL_BLOCKLIST_DIR)
	@[ -e $(DESTDIR)$(PIDDIR) ] || $(INSTALL) -d $(DESTDIR)$(PIDDIR)
	@[ -e $(DESTDIR)$(LOGDIR) ] || $(INSTALL) -d $(DESTDIR)$(LOGDIR)
	@[ -e $(DESTDIR)$(TMPDIR) ] || $(INSTALL) -d $(DESTDIR)$(TMPDIR)

CLEANFILES = \
	autom4te.cache/*
