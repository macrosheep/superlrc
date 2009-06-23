#author:Yang Hongyang<burnef@gmail.com>
SUBDIRS=src
LIBDIRS=lib

all install clean:
	@for subdir in $(SUBDIRS); do \
		echo "==> $$subdir"; \
		(cd $$subdir; $(MAKE) $@); \
	done

liball:
	@for subdir in $(LIBDIRS); do \
		echo "==> $$subdir"; \
		(cd $$subdir; $(MAKE) all); \
	done

libclean:
	@for subdir in $(LIBDIRS); do \
		echo "==> $$subdir"; \
		(cd $$subdir; $(MAKE) clean); \
	done
