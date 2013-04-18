VERSION=2.0

tarball:
	git archive --format=tar --prefix=libphycontmem-$(VERSION)/ HEAD | gzip > libphycontmem-$(VERSION).tar.gz
