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

  File:       PkgAttributeValue.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Kludge to return Object attributes. Dependent on the kind
  of attribute you may either expect a single string value (NAME, VERSION, etc.)
  or a list of strings (dependencies, filelist, etc).

/-*/
#ifndef PkgAttributeValue_h
#define PkgAttributeValue_h

#include <iosfwd>
#include <list>
#include <string>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgAttributeValue
/**
 * Kludge to return Object attributes. Dependent on the kind of attribute
 * you may either expect a single string value (NAME, VERSION, etc.) or
 * a list of strings (dependencies, filelist, etc).
 *
 * While it's basically a std::list<std::string>, we'll add some
 * convenience functions mainly for the UI.
 **/
class PkgAttributeValue : public std::list<std::string> {

  public:

    /**
     * Construct as empty list
     **/
    PkgAttributeValue();

    /**
     * Construct als list containing one entry val_r only
     **/
    PkgAttributeValue( const long int val );

    /**
     * Construct als list containing one entry val_r only
     **/
    PkgAttributeValue( const std::string & val_r );

    /**
     * Construct als copy of list val_r
     **/
    PkgAttributeValue( const std::list<std::string> & val_r );

    ~PkgAttributeValue();

  public:

    /**
     * Get as numeric value
     **/
    long int asLong ( long int default_li = 0 ) const;

    /**
     * Usefull for single string values if you don't mind whether there's
     * actually a value stored inside (size==1). It returns the default_r
     * (per default empty), if the list is empty. Ohterwise the first
     * (and probabely only) string stored..
     **/
    std::string firstLine( const std::string & default_r = "" ) const;

    /**
     * Write out a debug line
     **/
    friend std::ostream & operator<<( std::ostream & str, const PkgAttributeValue & obj );
};

///////////////////////////////////////////////////////////////////

#endif // PkgAttributeValue_h
