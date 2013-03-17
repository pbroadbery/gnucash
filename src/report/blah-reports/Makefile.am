SUBDIRS = . test

pkglib_LTLIBRARIES = libgncmod-blah-reports.la

libgncmod_blah_reports_la_SOURCES = \
  gncmod-blah-reports.c

libgncmod_blah_reports_la_LDFLAGS = -avoid-version

libgncmod_blah_reports_la_LIBADD = \
  ${top_builddir}/src/gnc-module/libgnc-module.la \
  ${GUILE_LIBS} \
  ${GLIB_LIBS}

AM_CPPFLAGS = \
  -I${top_srcdir}/src \
  -I${top_srcdir}/src/gnc-module \
  ${GUILE_INCS} \
  ${GLIB_CFLAGS}

gncscmmoddir = ${GNC_SHAREDIR}/guile-modules/gnucash/report/
gncscmmod_DATA = \
  blah-reports.scm

gncscmreportmoddir = ${GNC_SHAREDIR}/guile-modules/gnucash/report/blah-reports
gncscmreportmod_DATA = \
   net-barchart.scm \
   category-barchart.scm

if GNUCASH_SEPARATE_BUILDDIR
#For executing test cases
SCM_FILE_LINKS = ${gncscmmod_DATA} ${gncscmreportmod_DATA}
endif

.scm-links:
	$(RM) -rf gnucash
	mkdir -p  gnucash/report/blah-reports
if GNUCASH_SEPARATE_BUILDDIR
	for X in ${SCM_FILE_LINKS} ; do \
	  $(LN_S) -f ${srcdir}/$$X . ; \
	done
endif
	( cd gnucash/report; for A in $(gncscmmod_DATA) ; do $(LN_S) -f ../../$$A . ; done )
	( cd gnucash/report/blah-reports; for A in $(gncscmreportmod_DATA) ; do $(LN_S) -f ../../../$$A . ; done )
if ! OS_WIN32
# Windows knows no "ln -s" but uses "cp": must copy every time (see bug #566567).
	touch .scm-links
endif

clean-local:
	$(RM) -rf gnucash $(SCM_FILE_LINKS)

noinst_DATA = .scm-links

EXTRA_DIST = ${gncscmmod_DATA} ${gncscmreportmod_DATA}

CLEANFILES = .scm-links
DISTCLEANFILES = ${SCM_FILE_LINKS}

AM_CPPFLAGS += -DG_LOG_DOMAIN=\"gnc.report.blah_reports\"
