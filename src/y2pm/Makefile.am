#
# Makefile.am for liby2util/src/include/y2pm
#

pkgincludedir = $(includedir)/y2pm

# these go to $(pkgincludedir)

pkginclude_HEADERS = Y2PMCallbacks.h \
	Timecount.h	\
	Alternatives.h	\
	PkgSet.h	\
	PkgName.h PkgEdition.h PkgRelation.h PkgRevRel.h PkgSplit.h PkgArch.h PkgDu.h PkgChangelog.h	\
	\
	PMError.h			\
	ModulePkgError.h		\
	PMCallbacks.h			\
	\
	PMSolvable.h PMSolvablePtr.h	\
	PMObject.h PMObjectPtr.h	\
	\
	PMManager.h		\
	PMObjectContainerIter.h	\
	\
	PMSelectable.h PMSelectablePtr.h SelState.h \
        \
	binHeader.h binHeaderPtr.h \
	binHeaderCache.h \
	RpmHeader.h RpmHeaderPtr.h \
	RpmHeaderCache.h \
	librpmDb.h librpmDbPtr.h \
	RpmDb.h RpmDbPtr.h RpmDbCallbacks.h \
	PMRpmPackageDataProvider.h PMRpmPackageDataProviderPtr.h \
        \
        InstSrcManager.h InstSrcManagerCallbacks.h	\
	\
	InstSrcError.h				\
	InstSrc.h	InstSrcPtr.h		\
	InstSrcDescr.h	InstSrcDescrPtr.h	\
	InstSrcData.h	InstSrcDataPtr.h	\
	InstSrcDataUL.h	InstSrcDataULPtr.h	\
	ULSelectionParser.h ULSelectionParserPtr.h	\
	ULPackagesParser.h ULPackagesParserPtr.h	\
	InstSrcDataPLAIN.h InstSrcDataPLAINPtr.h	\
	PMPLAINPackageDataProvider.h PMPLAINPackageDataProviderPtr.h	\
	UpdateInfParser.h			\
	InstData.h				\
	FileDeps.h				\
	UpdateInfParser.h			\
	\
	InstTargetError.h			\
	InstTargetCallbacks.h			\
	InstTarget.h				\
	InstTargetSelDB.h InstTargetSelDBPtr.h	\
	InstTargetProdDB.h InstTargetProdDBPtr.h\
	InstSrcTags.h				\
	PMULPackageDataProvider.h PMULPackageDataProviderPtr.h	\
	PMULSelectionDataProvider.h PMULSelectionDataProviderPtr.h \
        \
        PkgDep.h InstallOrder.h \
        \
       	MediaAccess.h \
	MediaAccessPtr.h \
	MediaHandler.h \
	MediaDIR.h \
	MediaSMB.h \
	MediaDISK.h \
	MediaNFS.h \
	MediaCD.h \
        MediaCurl.h \
	Mount.h \
	MediaError.h \
        \
	PMPackage.h PMPackagePtr.h \
	PMPackageDataProvider.h PMPackageDataProviderPtr.h \
	PMPackageManager.h PMPackageManagerCallbacks.h PMPackageManager_update.h \
	PMPackageImEx.h PMPackageImExPtr.h \
        \
	Query.h		\
	QueryPtr.h	\
	QueryNode.h	\
	QueryError.h	\
	QueryParser.h	\
	QueryScanner.h \
        \
	PMSelection.h \
	PMSelectionDataProvider.h \
	PMSelectionDataProviderPtr.h \
	PMSelectionManager.h PMSelectionManagerCallbacks.h \
	PMSelectionPtr.h \
        \
	InstYou.h			\
	PMYouPatch.h			\
	PMYouPatchPtr.h			\
	PMYouPackageDataProvider.h	\
	PMYouPackageDataProviderPtr.h	\
	PMYouPatchInfo.h		\
	PMYouPatchInfoPtr.h		\
	PMYouPatchTags.h		\
	PMYouPatchManager.h		\
	PMYouPatchPaths.h		\
	PMYouPatchPathsPtr.h		\
	PMYouServers.h			\
	PMYouServersPtr.h		\
	YouError.h

#<INSTALL-HEADER-TARGET>

# Automatically inserted by insert_install_target
# DO NOT REMOVE THE TAGS!

# Install header files only if different to version already installed -
# prevent unnecessary installations to keep make dependency changes
# and thus rebuilds for dependent packages as low as possible.

install-pkgincludeHEADERS: $(pkginclude_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgincludedir)
	@list='$(pkginclude_HEADERS)';							\
	for p in $$list; do								\
		if test -f "$$p"; then d= ; else d="$(srcdir)/"; fi;			\
		if ! cmp -s $$d$$p $(DESTDIR)$(pkgincludedir)/$$p; then			\
			echo " $(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p";	\
			$(INSTALL_DATA) $$d$$p $(DESTDIR)$(pkgincludedir)/$$p;		\
		fi									\
	done

#</INSTALL-HEADER-TARGET>
