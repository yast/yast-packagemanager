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

  File:       PMLanguage.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

#include <iostream>

#include <y2pm/PMLanguage.h>

using namespace std;

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMLanguagePtr
//	CLASS NAME : constPMLanguagePtr
///////////////////////////////////////////////////////////////////
IMPL_DERIVED_POINTER(PMLanguage,PMSolvable);

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::PMLanguage
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMLanguage::PMLanguage( const LangCode & lang_r )
    : PMObject( PkgName( lang_r.code() ), PkgEdition(), PkgArch() )
    , _lang( lang_r )
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::~PMLanguage
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMLanguage::~PMLanguage()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::summary
//	METHOD TYPE : std::string
//
std::string PMLanguage::summary() const
{
  return _lang.name();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::description
//	METHOD TYPE : std::list<std::string>
//
std::list<std::string> PMLanguage::description() const
{
  return std::list<std::string>();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::insnotify
//	METHOD TYPE : std::list<std::string>
//
std::list<std::string> PMLanguage::insnotify() const
{
  return std::list<std::string>();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::delnotify
//	METHOD TYPE : std::list<std::string>
//
std::list<std::string> PMLanguage::delnotify() const
{
  return std::list<std::string>();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::size
//	METHOD TYPE : FSize
//
FSize PMLanguage::size() const
{
  return FSize();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::providesSources
//	METHOD TYPE : bool
//
bool PMLanguage::providesSources() const
{
  return false;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::instSrcLabel
//	METHOD TYPE : std::string
//
std::string PMLanguage::instSrcLabel() const
{
  return std::string();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::instSrcVendor
//	METHOD TYPE : Vendor
//
Vendor PMLanguage::instSrcVendor() const
{
  return Vendor();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::instSrcRank
//	METHOD TYPE : unsigned
//
unsigned PMLanguage::instSrcRank() const
{
  return 0;
}


///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMLanguage::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMLanguage::dumpOn( ostream & str ) const
{
  Rep::dumpOn( str );
  return str;
}

