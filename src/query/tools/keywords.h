/*
   keywords.h

   define query keywords and id values
   used by maketable

   !! see query.h for id ranges !!

   Author: kkaempf@suse.de
   Maintainer: kkaempf@suse.de

 */
#ifndef KEYWORDS_H
#define KEYWORDS_H

#include <y2pm/querycodes.h>

static struct keyword {
    char *word;
    int code;
} keywords[] = {

// compare opts, id 1000...
    { "like",		QCODE_LIKE },
    { "contains",	QCODE_CONTAINS },

// flags, 1..99
    { "isInstalled",	QCODE_ISINSTALLED },
    { "isAvailable",	QCODE_ISAVAILABLE },
    { "toInstall",	QCODE_TOINSTALL },
    { "toDelete",	QCODE_TODELETE },
    { "toUpdate",	QCODE_TOUPDATE },
    { "isTaboo",	QCODE_ISTABOO },
    { "isAutomatic",	QCODE_ISAUTOMATIC },
    { "isIncomplete",	QCODE_ISINCOMPLETE },
    { "isCandidate",	QCODE_ISCANDIDATE },

// values, 100..999
    { "Version",	QCODE_VERSION },
    { "Vendor",		QCODE_VENDOR },
    { "Group",		QCODE_GROUP },
    { "Name",		QCODE_NAME },
    { "Requires",	QCODE_REQUIRES },
    { "Provides",	QCODE_PROVIDES },
    { "Conflicts",	QCODE_CONFLICTS },
    { "InstallDate",	QCODE_INSTALLDATE },
    { "InstallTime",	QCODE_INSTALLDATE },
    { "BuildDate",	QCODE_BUILDDATE },
    { "BuildTime",	QCODE_BUILDDATE },
    { "Selection",	QCODE_SELECTION },
    { "Patch",		QCODE_PATCH },
    { "Files",		QCODE_FILES },
    { "Filenames",	QCODE_FILES },
    { "MIME",		QCODE_MIME },
    { "Keywords",	QCODE_KEYWORDS },
    { "Supports",	QCODE_SUPPORTS },
    { "Needs",		QCODE_NEEDS },
    { "Summary",	QCODE_SUMMARY },
    { "Label",		QCODE_SUMMARY },	// alias for "summary"
    { "Description",	QCODE_DESCRIPTION },
    { "From",		QCODE_FROM },		// InstSrc
    { "iVersion",	QCODE_IVERSION },	// installed version
    { "aVersion",	QCODE_AVERSION },	// available version
    { "cVersion",	QCODE_CVERSION },	// candidate version
    { "Empty",		QCODE_EMPTY },
    { "Release",	QCODE_RELEASE },
    { "Edition",	QCODE_EDITION },
    { 0, 0 }
};

#endif	// KEYWORDS_H
