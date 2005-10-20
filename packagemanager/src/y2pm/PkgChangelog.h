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

  File:       PkgChangelog.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose: Helper class providing changelog information.

/-*/
#ifndef PkgChangelog_h
#define PkgChangelog_h

#include <iosfwd>
#include <list>
#include <string>

#include <y2util/Date.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PkgChangelog
/**
 * @short Helper class providing changelog information.
 *
 * <code>PkgChangelog</code> contains a <code>std::list&lt;PkgChangelog::Entry></code>.
 * Each <code>Entry</code> consists of date, author and text, as derived from a
 * rpmlib header. Text is expected to be the preformated changes entry with all lines
 * delimited by '\n'.
 *
 * A conversion <code>std::list&lt;std::string> asStringList()</code> is provided.
 * The list returned contains the changelog line by line, fromated the same way
 * <code>'rpm -q --changelog'</code> would do.
 **/
class PkgChangelog {

  /**
   * Print changelog fromatedthe same way <code>'rpm -q --changelog'</code> would do.
   **/
  friend std::ostream & operator<<( std::ostream & str, const PkgChangelog & obj );

  public:

    struct Entry {
      /**
       * Entries date.
       **/
      Date _date;
      /**
       * Entries author.
       **/
      std::string _name;
      /**
       * Entries text. Preformated and all lines delimited by '\n'.
       **/
      std::string _text;
      /**
       * Constructor
       **/
      Entry( const Date & d, const std::string & n, const std::string & t )
	: _date( d ), _name( n ), _text( t )
      {}
    };

  private:

    /**
     * Entry list.
     **/
    std::list<Entry> _entries;

  public:

    /**
     * Constructor
     **/
    PkgChangelog() {}
    /**
     * Destructor
     **/
    ~PkgChangelog() {}
    /**
     * Append a new entry
     **/
    void push_back( const Entry & e_r ) { _entries.push_back( e_r ); }
    /**
     * Prepend a new entry
     **/
    void push_front( const Entry & e_r ) { _entries.push_front( e_r ); }
    /**
     * Number of entries
     **/
    unsigned size() const { return _entries.size(); }

  public:

   typedef std::list<Entry>::iterator               iterator;
   typedef std::list<Entry>::reverse_iterator       reverse_iterator;

   /**
    * Forward iterator pointing to the first entry (if any)
    **/
   iterator begin() { return _entries.begin(); }
   /**
    * Forward iterator pointing behind the last entry.
    **/
   iterator end() { return _entries.end(); }
   /**
    * Reverse iterator pointing to the last entry (if any)
    **/
   reverse_iterator rbegin() { return _entries.rbegin(); }
   /**
    * Reverse iterator pointing before the first entry.
    **/
   reverse_iterator rend() { return _entries.rend(); }

   typedef std::list<Entry>::const_iterator         const_iterator;
   typedef std::list<Entry>::const_reverse_iterator const_reverse_iterator;

   /**
    * Forward const iterator pointing to the first entry (if any)
    **/
   const_iterator begin() const { return _entries.begin(); }
   /**
    * Forward const iterator pointing behind the last entry.
    **/
   const_iterator end() const { return _entries.end(); }
   /**
    * Reverse const iterator pointing to the last entry (if any)
    **/
   const_reverse_iterator rbegin() const { return _entries.rbegin(); }
   /**
    * Reverse const iterator pointing before the first entry.
    **/
   const_reverse_iterator rend() const { return _entries.rend(); }

  public:

   /**
    * The list returned contains the changelog line by line, fromated
    * the same way <code>'rpm -q --changelog'</code> would do.
    **/
   std::list<std::string> asStringList() const;
};

///////////////////////////////////////////////////////////////////

#endif // PkgChangelog_h
