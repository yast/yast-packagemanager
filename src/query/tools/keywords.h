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

static struct keyword {
    char *word;
    int code;
} keywords[] = {

// compare opts, id 1000...
    { "like",		1000 },
    { "contains",	1001 },

// flags, 1..99
    { "isInstalled",	1 },
    { "isAvailable",	2 },
    { "isSelected",	3 },
    { "isDeleted",	4 },
    { "isUpdated",	5 },
    { "isTaboo",	6 },
    { "isAutomatic",	7 },
    { "isIncomplete",	8 },

// values, 100..999
    { "Version",	100 },
    { "Vendor",		101 },
    { "Group",		102 },
    { "Name",		103 },
    { "Requires",	104 },
    { "Provides",	105 },
    { "Conflicts",	106 },
    { "InstallDate",	107 },
    { "BuildDate",	108 },
    { "Selection",	110 },
    { "Patch",		111 },
    { "Files",		112 },
    { "MIME",		113 },
    { "Keywords",	114 },
    { "Supports",	115 },
    { "Needs",		116 },
    { "Summary",	117 },
    { "Label",		117 },		// alias for "summary"
    { "Description",	118 },
    { "From",		119 },		// InstSrc
    { 0, 0 }
};

#endif	// KEYWORDS_H

