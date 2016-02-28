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
	pgld

# All other docs are added automatically distributed by automake:
dist_doc_DATA = \
	BUGS \
	README.blocklists \
	README.dbus

EXTRA_DIST = \
	autogen.sh

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
