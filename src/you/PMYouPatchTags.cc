/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                               core system                            |
|                                                    (C) SuSE Linux AG |
\----------------------------------------------------------------------/

  File:       PMYouPatchTags.cc

  Author:     Cornelius Schumacher <cschum@suse.de>
  Maintainer: Cornelius Schumacher <cschum@suse.de>

  Purpose: Tag definitions for TaggedParser.

/-*/

#include <y2pm/PMYouPatchTags.h>

YOUPatchTagSet::YOUPatchTagSet()
{
  addTag( "Patchname",           PATCHNAME,           TaggedFile::SINGLE );
  addTag( "Patchversion",        PATCHVERSION,        TaggedFile::SINGLE );
  addTag( "Requires",            REQUIRES,            TaggedFile::SINGLE );
  addTag( "Provides",            PROVIDES,            TaggedFile::SINGLE );
  addTag( "Kind",                KIND,                TaggedFile::SINGLE );
  addTag( "Shortdescription",    SHORTDESCRIPTION,    TaggedFile::SINGLE,   TaggedFile::ALLOWLOCALE );
  addTag( "Longdescription",     LONGDESCRIPTION,     TaggedFile::MULTIYOU, TaggedFile::ALLOWLOCALE );
  addTag( "Preinformation",      PREINFORMATION,      TaggedFile::MULTIYOU, TaggedFile::ALLOWLOCALE );
  addTag( "Postinformation",     POSTINFORMATION,     TaggedFile::MULTIYOU, TaggedFile::ALLOWLOCALE );
  addTag( "UpdateOnlyInstalled", UPDATEONLYINSTALLED, TaggedFile::SINGLE );
  addTag( "Packages",            PACKAGES,            TaggedFile::MULTIOLD );
  addTag( "Prescript",           PRESCRIPT,           TaggedFile::SINGLE );
  addTag( "Postscript",          POSTSCRIPT,          TaggedFile::SINGLE );
  addTag( "Files",               FILES,               TaggedFile::MULTIOLD );
  addTag( "UpdateOnlyNew",       UPDATEONLYNEW,       TaggedFile::SINGLE );
}

YOUPackageTagSet::YOUPackageTagSet() : TagSet()
{
  addTag( "Filename",        FILENAME,             TaggedFile::SINGLE, TaggedFile::START );
  addTag( "Label",           LABEL,                TaggedFile::SINGLE );
  addTag( "RpmName",         RPMNAME,              TaggedFile::SINGLE );
  addTag( "InstPath",        INSTPATH,             TaggedFile::SINGLE );
  addTag( "Size",            SIZE,                 TaggedFile::SINGLE );
  addTag( "PatchRpmSize",    PATCHRPMSIZE,         TaggedFile::SINGLE );
  addTag( "Buildtime",       BUILDTIME,            TaggedFile::SINGLE );
  addTag( "BuiltFrom",       BUILTFROM,            TaggedFile::SINGLE );
  addTag( "RpmGroup",        RPMGROUP,             TaggedFile::SINGLE );
  addTag( "Copyright",       COPYRIGHT,            TaggedFile::SINGLE );
  addTag( "AuthorName",      AUTHORNAME,           TaggedFile::SINGLE );
  addTag( "Version",         PKGVERSION,           TaggedFile::SINGLE );
  addTag( "Obsoletes",       OBSOLETES,            TaggedFile::SINGLE );
  addTag( "Requires",        REQUIRES,             TaggedFile::SINGLE );
  addTag( "Provides",        PROVIDES,             TaggedFile::SINGLE );
  addTag( "Conflicts",       CONFLICTS,            TaggedFile::SINGLE );
  addTag( "PatchRpmBasedOn", PATCHRPMBASEVERSIONS, TaggedFile::SINGLE );
  addTag( "Series",          ARCH,                 TaggedFile::SINGLE );
  addTag( "ForceInstall",    FORCEINSTALL,         TaggedFile::SINGLE );
  addTag( "Deltas",          DELTAS,               TaggedFile::MULTIOLD);
}
