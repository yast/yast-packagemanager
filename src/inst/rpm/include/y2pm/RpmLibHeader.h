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

  File:       RpmLibHeader.h

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/
#ifndef RpmLibHeader_h
#define RpmLibHeader_h

extern "C" {
#include <rpm/rpmlib.h>
}

#include <iosfwd>
#include <list>

#include <y2pm/RpmLibHeaderPtr.h>
#include <y2pm/PMPackage.h>
#include <y2pm/FileDeps.h>

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : RpmLibHeader
/**
 *
 **/
class RpmLibHeader : virtual public Rep {
  REP_BODY(RpmLibHeader); // includes no cpoy, no assign

  private:

    Header _h;

  private:

    int_32 tag_type( int_32 tag_r ) const;

    bool has_tag( int_32 tag_r ) const { return( tag_type( tag_r ) != RPM_NULL_TYPE ); }

  private:

    class int32List;
    unsigned int_32_list( int_32 tag_r, int32List & lst_r ) const;

    class stringList;
    unsigned string_list( int_32 tag_r, stringList & lst_r ) const;

  private:

    int_32 int_32_val( int_32 tag_r ) const;

    std::string string_val( int_32 tag_r ) const;

    std::list<std::string> stringList_val( int_32 tag_r ) const;

    PMSolvable::PkgRelList_type PkgRelList_val( int_32 tag_r, FileDeps::FileNames * freq_r = 0 ) const;

  public:

    RpmLibHeader( Header h );

    virtual ~RpmLibHeader();

  public:

    class Changelog {
      public:
	struct Entry {
	  Date        _date;
	  std::string _name;
	  std::string _text;
	  Entry( const Date & d, const std::string & n, const std::string & t )
	    : _date( d ), _name( n ), _text( t )
	  {}
	};
      private:
	std::list<Entry> _entries;
      public:
	Changelog() {}
	~Changelog() {}
	void push_back( const Entry & e_r ) { _entries.push_back( e_r ); }
	void push_front( const Entry & e_r ) { _entries.push_front( e_r ); }
	unsigned size() const { return _entries.size(); }
      public:
	typedef std::list<Entry>::const_iterator         const_iterator;
	typedef std::list<Entry>::const_reverse_iterator const_reverse_iterator;
	const_iterator         begin()  const { return _entries.begin(); }
	const_iterator         end()    const { return _entries.end(); }
	const_reverse_iterator rbegin() const { return _entries.rbegin(); }
	const_reverse_iterator rend()   const { return _entries.rend(); }
      public:
	std::list<std::string> asStringList() const;
    };

  public:

    PkgName    tag_name()        const;
    PkgEdition tag_edition()     const;
    PkgArch    tag_arch()        const;

    Date       tag_installtime() const;
    Date       tag_buildtime()   const;

    PMSolvable::PkgRelList_type tag_provides ( FileDeps::FileNames * freq_r = 0 ) const;
    PMSolvable::PkgRelList_type tag_requires ( FileDeps::FileNames * freq_r = 0 ) const;
    PMSolvable::PkgRelList_type tag_conflicts( FileDeps::FileNames * freq_r = 0 ) const;
    PMSolvable::PkgRelList_type tag_obsoletes( FileDeps::FileNames * freq_r = 0 ) const;

    FSize tag_size()        const;
    FSize tag_archivesize() const;

    std::string tag_summary()      const;
    std::string tag_description()  const;
    std::string tag_group()        const;
    std::string tag_vendor()       const;
    std::string tag_distribution() const;
    std::string tag_license()      const;
    std::string tag_buildhost() const;
    std::string tag_packager() const;
    std::string tag_url() const;
    std::string tag_os() const;
    std::string tag_prein() const;
    std::string tag_postin() const;
    std::string tag_preun() const;
    std::string tag_postun() const;
    std::string tag_sourcerpm() const;

    Changelog   tag_changelog() const;
    std::list<std::string> tag_filenames() const;

  public:

    virtual std::ostream & dumpOn( std::ostream & str ) const;
};

///////////////////////////////////////////////////////////////////

#endif // RpmLibHeader_h

