# dependency tracking does not work well with our
# moc rules
AUTOMAKE_OPTIONS = \
	no-dependencies \
	subdir-objects

# QT tools
RCC = @RCC@
UIC = @UIC@
MOC = @MOC@

# flags
AM_CXXFLAGS = @QT_CXXFLAGS@
AM_CPPFLAGS = \
	@QT_CPPFLAGS@ \
	-DPGLCMDDEFAULTSPATH=\"$(libdir)/$(PACKAGE)/$(PACKAGE)cmd.defaults\" \
	-I./ui
AM_LDFLAGS = @QT_LDFLAGS@


bin_PROGRAMS = pglgui

# additional libraries and objects to link in pglgui
pglgui_LDADD = \
	@QT_LIBS@ \
	$(nodist_EXTRA_pglgui_SOURCES:.cpp=.o) \
	qrc_images.o

dist_pglgui_SOURCES = \
	src/enums.h \
	src/port.h src/port.cpp \
	src/option.cpp src/option.h src/option_p.h \
	src/whitelist_item.cpp src/whitelist_item.h src/whitelist_item_p.h \
	src/list_item.cpp src/list_item.h \
	src/blocklist.cpp src/blocklist.h src/blocklist_p.h \
	src/pglcore.cpp src/pglcore.h \
	src/add_exception_dialog.cpp src/add_exception_dialog.h \
	src/command.cpp src/command.h \
	src/error_dialog.cpp src/error_dialog.h \
	src/file_transactions.cpp src/file_transactions.h \
	src/pglgui.cpp src/pglgui.h \
	src/pgl_daemon.cpp src/pgl_daemon.h \
	src/pglcmd.cpp src/pglcmd.h \
	src/blocklist_manager.cpp src/blocklist_manager.h \
	src/pgl_settings.cpp src/pgl_settings.h \
	src/whitelist_manager.cpp src/whitelist_manager.h \
	src/proc_thread.cpp src/proc_thread.h \
	src/settings.cpp src/settings.h \
	src/super_user.cpp src/super_user.h \
	src/utils.cpp src/utils.h \
	src/viewer_widget.cpp src/viewer_widget.h \
	src/main.cpp

# these moc files are also built from SOURCES
nodist_EXTRA_pglgui_SOURCES = \
	src/moc_add_exception_dialog.cpp \
	src/moc_pglcore.cpp \
	src/moc_whitelist_manager.cpp \
	src/moc_blocklist_manager.cpp \
	src/moc_command.cpp \
	src/moc_error_dialog.cpp \
	src/moc_file_transactions.cpp \
	src/moc_pglgui.cpp \
	src/moc_pgl_daemon.cpp \
	src/moc_pglcmd.cpp \
	src/moc_pgl_settings.cpp \
	src/moc_proc_thread.cpp \
	src/moc_settings.cpp \
	src/moc_super_user.cpp \
	src/moc_utils.cpp \
	src/moc_viewer_widget.cpp

# rule to build moc files
src/moc_%.cpp: src/%.h
	$(MOC) $(AM_CPPFLAGS) "$<" -o "$@"

# ui forms
FORMSSOURCES = \
	ui/add_exception.ui \
	ui/main_window.ui \
	ui/settings.ui

# these ui forms headers are built from FORMSSOURCES
nodist_FORMSHEADERS = \
	ui/ui_add_exception.h \
	ui/ui_main_window.h \
	ui/ui_settings.h

# rule to build ui forms headers
ui/ui_%.h: ui/%.ui
	$(UIC) $< -o $@

# rule to build qrc_images.cpp
qrc_images.cpp: images/images.qrc
	$(RCC) -name images images/images.qrc -o qrc_images.cpp

# .desktop file rules
desktopdir = $(datadir)/applications
dist_desktop_DATA = other/pglgui.desktop

# icon
iconsdir = @ICONSDIR@
dist_icons_DATA = images/pglgui.png

# additional files to be added to the tar.gz on "make dist"
EXTRA_DIST = \
	images/images.qrc \
	images/*.png \
	$(FORMSSOURCES)

BUILT_SOURCES = \
	$(nodist_EXTRA_pglgui_SOURCES) \
	$(nodist_FORMSHEADERS) \
	qrc_images.cpp

CLEANFILES = \
	$(BUILT_SOURCES) \
	$(nodist_EXTRA_pglgui_SOURCES:.cpp=.o) \
	.deps/*.Po
