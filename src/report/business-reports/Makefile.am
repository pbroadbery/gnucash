gncscmmoddir = ${GNC_SHAREDIR}/guile-modules/gnucash/report
gncscmmod_DATA = \
   aging.scm \
   business-reports.scm \
   customer-summary.scm \
   easy-invoice.scm \
   fancy-invoice.scm \
   taxinvoice.scm \
   taxinvoice.eguile.scm \
   taxinvoice.css \
   invoice.scm \
   job-report.scm \
   owner-report.scm \
   payables.scm \
   receivables.scm \
   balsheet-eg.css \
   balsheet-eg.eguile.scm \
   balsheet-eg.scm

if GNUCASH_SEPARATE_BUILDDIR
SCM_FILE_LINKS = ${gncscmmod_DATA}
endif

.scm-links:
	$(RM) -rf gnucash
	mkdir -p  gnucash
	mkdir -p  gnucash/report
if GNUCASH_SEPARATE_BUILDDIR
	for X in ${SCM_FILE_LINKS} ; do \
	  $(LN_S) -f ${srcdir}/$$X . ; \
	done
endif
	( cd gnucash/report; for A in $(gncscmmod_DATA) ; do $(LN_S) -f ../../$$A . ; done )
if ! OS_WIN32
# Windows knows no "ln -s" but uses "cp": must copy every time (see bug #566567).
	touch .scm-links
endif

clean-local:
	$(RM) -rf gnucash

noinst_DATA = .scm-links

EXTRA_DIST = \
  ${gncscmmod_DATA}

CLEANFILES = .scm-links
DISTCLEANFILES = ${SCM_FILE_LINKS}

AM_CPPFLAGS = -DG_LOG_DOMAIN=\"gnc.business.reports\"
