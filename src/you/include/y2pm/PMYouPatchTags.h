/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                        (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:       PMYouPatchTags.h

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Tag definitions for TaggedParser.

/-*/
#ifndef PMYouPatchTags_h
#define PMYouPatchTags_h

#include <string>

#include <y2util/TaggedFile.h>
  
class YOUPatchTagSet : public TaggedFile::TagSet
{
  public:
    enum Tags {
      PATCHNAME,
      PATCHVERSION,
      REQUIRES,
      PROVIDES,
      KIND,
      SHORTDESCRIPTION,
      LONGDESCRIPTION,
      PREINFORMATION,
      POSTINFORMATION,
      SIZE,
      BUILDTIME,
      MINYAST1VERSION,
      MINYAST2VERSION,
      UPDATEONLYINSTALLED,
      PACKAGES,
      PRESCRIPT,
      POSTSCRIPT
    };
 
    YOUPatchTagSet();
};
 
class YOUPackageTagSet : public TaggedFile::TagSet
{
  public:
    enum Tags {
      FILENAME,
      LABEL,
      RPMNAME,
      INSTPATH,
      SIZE,
      PATCHRPMSIZE,
      BUILDTIME,
      BUILTFROM,
      RPMGROUP,
      COPYRIGHT,
      AUTHORNAME,
      PKGVERSION,
      OBSOLETES,
      REQUIRES,
      PROVIDES,
      CONFLICTS,
      PATCHRPMBASEVERSIONS
    };
 
    YOUPackageTagSet();
};

#endif
