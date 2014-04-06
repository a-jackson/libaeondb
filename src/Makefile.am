AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libaeondb-@LIBAEONDB_API_VERSION@.la

libaeondb_@LIBAEONDB_API_VERSION@_la_SOURCES = btree.c btree.h common.h \
        db.c db.h query.c query.h \
        tag.c tag.h timestore.c timestore.h
libaeondb_@LIBAEONDB_API_VERSION@_la_LDFLAGS = -version-info $(LIBAEONDB_SO_VERSION)

libaeondb_includedir = $(includedir)/aeondb-$(LIBAEONDB_API_VERSION)
libaeondb_include_HEADERS = aeondb.h
