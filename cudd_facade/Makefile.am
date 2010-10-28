# Process this file with automake to produce Makefile.in

CUDD_PATH = ./cudd

AM_CXXFLAGS = -I$(CUDD_PATH)/include

CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)

LIBNAME = libcudd_facade.a

noinst_SCRIPTS = $(LIBNAME)


CUDD_LIBS  =
CUDD_LIBS += $(CUDD_PATH)/cudd/libcudd.a
CUDD_LIBS += $(CUDD_PATH)/epd/libepd.a
CUDD_LIBS += $(CUDD_PATH)/mtr/libmtr.a
CUDD_LIBS += $(CUDD_PATH)/st/libst.a
CUDD_LIBS += $(CUDD_PATH)/util/libutil.a
CUDD_LIBS += $(CUDD_PATH)/dddmp/libdddmp.a


OBJ_FILES = cudd_facade.o

# Archiver flags
AR_CREATE_FLAGS   = r
AR_EXTRACT_FLAGS  = x


cudd:
	cd $(CUDD_PATH) ; make

cudd_facade.o: cudd_facade.cc
	$(CXXCOMPILE) -o $@ -c $<

$(LIBNAME): $(OBJ_FILES) cudd
	TMP_DIR=`mktemp -d` ; \
	cp $(OBJ_FILES) $${TMP_DIR} ; \
	cp $(CUDD_LIBS) $${TMP_DIR} ; \
	( \
		cd $${TMP_DIR} ; \
		for i in *.a ; do \
			$(AR) $(AR_EXTRACT_FLAGS) $${i} ; \
		done ; \
		rm *.a ; \
		$(AR) $(AR_CREATE_FLAGS) $(LIBNAME) *.o ; \
		rm *.o ; \
	) ; \
	cp $${TMP_DIR}/$@ . ; \
	rm -rf $${TMP_DIR}
	$(RANLIB) $(LIBNAME)
