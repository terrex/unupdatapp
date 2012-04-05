SUBDIRS = src tests
distclean-local:
	-$(mv) $(srcdir)/tests/UPDATA.APP /tmp/UPDATA.APP
	git clean -f -d
	-$(mv) /tmp/UPDATA.APP $(srcdir)/tests/UPDATA.APP
	find -name '*~' -delete
