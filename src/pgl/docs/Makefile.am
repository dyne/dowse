dist_man_MANS = pgld.1

examplesdir = $(docdir)/examples
dist_examples_DATA =  \
	iptables-custom-insert.sh \
	iptables-custom-remove.sh

# do not recreate existent directories
# this would also mess up custom permissions if present
install-data-local:
	@[ -e $(DESTDIR)$(examplesdir) ] || $(INSTALL) -d $(DESTDIR)$(examplesdir)
