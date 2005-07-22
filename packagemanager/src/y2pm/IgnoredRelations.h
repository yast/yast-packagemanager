/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                     (C) 2004 SUSE AG |
\----------------------------------------------------------------------/

   File:       IgnoredRelations.h
   Purpose:    Classes for ignoring Relations
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#ifndef _IgnoredRelations_h
#define _IgnoredRelations_h

#include <map>

#include <y2pm/PkgName.h>
#include <y2pm/PkgRelation.h>

class IgnoredRelations
{
    public:
	class Ignored
	{
	    public:
		Ignored(PkgName* pkg, PkgRelation r);
		
		Ignored(const Ignored& rhs);

		Ignored& operator=(const Ignored& rhs);

	    public:
		/** name of a package, empty for any */
		PkgName* pkg;

		/** relation to ignore */
		PkgRelation rel;
	};
	
    private:
	// name of requirement -> ignore class
	typedef std::multimap<PkgName, Ignored> IgnoreList;

	IgnoreList _ignored;

    public:

	IgnoredRelations();

	virtual ~IgnoredRelations();

	void add(const PkgRelation& rel, PkgName* n = NULL);
	
	void add(const PkgRelation& rel, PMSolvablePtr pkg);
	
	virtual bool isIgnored(const PkgRelation& rel, PkgName* n = NULL);
	
	virtual bool isIgnored(const PkgRelation& rel, PMSolvablePtr);
};

class IgnoredRequirements : public IgnoredRelations
{
    public:
	IgnoredRequirements();
	~IgnoredRequirements();

	virtual bool isIgnored(const PkgRelation& rel, PkgName* n = NULL);
	
	virtual bool isIgnored(const PkgRelation& rel, PMSolvablePtr);
};

#endif
