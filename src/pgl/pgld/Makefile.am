AUTOMAKE_OPTIONS = subdir-objects

# flags
AM_CFLAGS = @PGLD_CFLAGS@
AM_CPPFLAGS = @PGLD_CPPFLAGS@ \
			  -DPIDFILE=\"$(localstatedir)/run/${PACKAGE}d.pid\"
AM_LDFLAGS = @PGLD_LDFLAGS@

# sources for pgld binary
pgld_SOURCES = \
	       src/pgld.c src/pgld.h \
	       src/stream.c src/stream.h \
	       src/blocklist.c src/blocklist.h \
	       src/parser.c src/parser.h
pgld_LDADD = @PGLD_LIBS@
sbin_PROGRAMS = pgld

# build dbus library for --enable-dbus
if DBUSMAKE
AM_CPPFLAGS += -DPLUGINDIR=\"$(libdir)/$(PACKAGE)\"
libdbusdir = $(libdir)/$(PACKAGE_NAME)
libdbus_LTLIBRARIES = libdbus.la
libdbus_la_SOURCES = src/dbus.c src/dbus.h
libdbus_la_LDFLAGS = -module -avoid-version
libdbus_la_LIBADD = @PGLD_LIBS@
dbusdir = $(sysconfdir)/dbus-1/system.d
dist_dbus_DATA = org.netfilter.pgl.conf
else

endif

if LOGROTATEMAKE
logdir = $(sysconfdir)/logrotate.d
dist_log_DATA = logrotate/pgld
endif

