AM_CFLAGS = -W -Wall -std=c99 -D_POSIX_C_SOURCE=2
bin_PROGRAMS = unupdatapp
unupdatapp_LDFLAGS = -lpthread
unupdatapp_SOURCES = main.c unupdatapp.c updatapp.c packet.c crc.c popen2.c \
    unupdatapp.h updatapp.h packet.h crc.h popen2.h
install-exec-hook:
	cd $(DESTDIR)$(bindir) && \
	    $(LN_S) unupdatapp updatapp
	$(INSTALL_PROGRAM) ../tests/crc $(DESTDIR)$(bindir)/unupdatapp_crc16
uninstall-local:
	$(RM) $(DESTDIR)$(bindir)/updatapp
	$(RM) $(DESTDIR)$(bindir)/unupdatapp_crc16
