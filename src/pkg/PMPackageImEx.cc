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

  File:       PMPackageImEx.cc

  Author:     Michael Andres <ma@suse.de>
  Maintainer: Michael Andres <ma@suse.de>

  Purpose:

/-*/

extern "C" {
#include <unistd.h>
}

#include <iostream>
#include <fstream>

#include <y2util/Y2SLog.h>

#include <Y2PM.h>
#include <y2pm/PMPackageImEx.h>
#include <y2pm/PMSelectionManager.h>
#include <y2pm/PMPackageManager.h>

using namespace std;

#undef Y2LOG
#define Y2LOG "PMPackageImEx"

///////////////////////////////////////////////////////////////////
//	CLASS NAME : PMPackageImExPtr
//	CLASS NAME : constPMPackageImExPtr
///////////////////////////////////////////////////////////////////
IMPL_BASE_POINTER(PMPackageImEx);

/******************************************************************
**
**
**	FUNCTION NAME : pkgNameEd
**	FUNCTION TYPE : PkgNameEd
**
**	DESCRIPTION :
*/
inline PkgNameEd pkgNameEd( const constPMObjectPtr & obj_r )
{
  return PkgNameEd( obj_r->name(), obj_r->edition() );
}

///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageImEx::Magic
//
///////////////////////////////////////////////////////////////////

const std::string PMPackageImEx::Magic::_magic( "YaST-PackageImEx");

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::Magic::readFrom
//	METHOD TYPE : std::istream &
//
//	DESCRIPTION :
//
std::istream & PMPackageImEx::Magic::readFrom( std::istream & str )
{
  *this = Magic();

  if ( str ) {
    streampos curpos = str.tellg();
    string line = stringutil::getline( str );
    std::vector<std::string> words;
    if ( stringutil::split( line, words ) == 3
	 && words[0][0] == '#'
	 && words[1] == _magic ) {
      _version = PkgEdition( words[2] );
    } else {
      ERR << "Illegal magic for PackageImEx at pos " << curpos
	<< ": \""  << line << "\"" << endl;
    }
  } else {
    ERR << "Error reading magic for PackageImEx" << endl;
  }

  return str;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::Magic::writeOn
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & PMPackageImEx::Magic::writeOn( std::ostream & str ) const
{
  static char hostname_ti[256];
  hostname_ti[0] = hostname_ti[255] = '\0';
  if ( gethostname( hostname_ti, 255 ) == -1 ) {
    strcpy( hostname_ti, "(unknown)" );
  }

  str << "## " << _magic << " " << _version << endl;
  str << "## Generated on " << Date(Date::now()) << " at " << hostname_ti << endl;
  str << endl;

  return str;
}


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : PMPackageImEx
//
///////////////////////////////////////////////////////////////////

const PMPackageImEx::Magic PMPackageImEx::_ImExMagic( PkgEdition("1.0","0") );

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::PMPackageImEx
//	METHOD TYPE : Constructor
//
//	DESCRIPTION :
//
PMPackageImEx::PMPackageImEx()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::~PMPackageImEx
//	METHOD TYPE : Destructor
//
//	DESCRIPTION :
//
PMPackageImEx::~PMPackageImEx()
{
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::dumpOn
//	METHOD TYPE : ostream &
//
//	DESCRIPTION :
//
ostream & PMPackageImEx::dumpOn( ostream & str ) const
{
  str << rep_name() << "{";
#define OUTSTR(v,s) str << v "(" << s.size() << ")"
  OUTSTR( "Rlc",	_requestedLocales );
  OUTSTR( "Ons",	_onSystemSel );
  OUTSTR( "Offs",	_offSystemSel );
  OUTSTR( "Onp",	_onSystemPkg );
  OUTSTR( "Ontp",	_onSystemTabooPkg );
  OUTSTR( "Offp",	_offSystemPkg );
  OUTSTR( "Offtp",	_offSystemTabooPkg );
#undef OUTSTR
  return str << "}";
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::reset
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
inline void PMPackageImEx::reset()
{
  _requestedLocales.clear();

  _onSystemSel.clear();
  _offSystemSel.clear();

  _onSystemPkg.clear();
  _onSystemTabooPkg.clear();
  _offSystemPkg.clear();
  _offSystemTabooPkg.clear();
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::collect_Sel
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMPackageImEx::collect_Sel( const constPMSelectablePtr & sel_r )
{
  if ( !sel_r )
    return false;

  if ( sel_r->to_delete() ) {
    _offSystemSel.insert( sel_r->name() );
    return true;
  }

  if ( sel_r->to_install() ) {
    _onSystemSel.insert( pkgNameEd( sel_r->candidateObj() ) );
    return true;
  }

  // unmodified selections:

  if ( sel_r->has_installed() ) {
    _onSystemSel.insert( pkgNameEd( sel_r->installedObj() ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::remember_onSystemPkg
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
inline bool PMPackageImEx::collect_Pkg( const constPMSelectablePtr & sel_r )
{
  if ( !sel_r )
    return false;

  if ( sel_r->by_auto() ) {
    return false;
  }

  if ( sel_r->to_delete() ) {
    _offSystemPkg.insert( sel_r->name() );
    return true;
  }

  if ( sel_r->to_install() ) {
    _onSystemPkg.insert( pkgNameEd( sel_r->candidateObj() ) );
    return true;
  }

  // unmodified packages:

  if ( sel_r->has_installed() ) {
    (sel_r->is_taboo() ? _onSystemTabooPkg : _onSystemPkg).insert( pkgNameEd( sel_r->installedObj() ) );
    return true;
  }
  if ( sel_r->is_taboo() ) {
    _offSystemTabooPkg.insert( sel_r->name() );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::getPMState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageImEx::getPMState()
{
  reset();
  _requestedLocales = Y2PM::getRequestedLocales();

  for ( PMManager::PMSelectableVec::const_iterator it = Y2PM::selectionManager().begin();
	it != Y2PM::selectionManager().end(); ++it ) {
    collect_Sel( *it );
  }

  for ( PMManager::PMSelectableVec::const_iterator it = Y2PM::packageManager().begin();
	it != Y2PM::packageManager().end(); ++it ) {
    collect_Pkg( *it );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::setPMState
//	METHOD TYPE : void
//
//	DESCRIPTION :
//
void PMPackageImEx::setPMState()
{
  INT << "TBD" << endl;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doImport
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMPackageImEx::doImport( const Pathname & path_r )
{
  ifstream file( path_r.asString().c_str() );
  doImport( file );
  if ( file.bad() || file.fail() ) {
    ERR << "Error reading file '" << path_r << "' ("
      << (file.good()?'g':'_') << (file.eof()?'e':'_')
      << (file.fail()?'F':'_') << (file.bad()?'B':'_') << ")" << endl;
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doExport
//	METHOD TYPE : bool
//
//	DESCRIPTION :
//
bool PMPackageImEx::doExport( const Pathname & path_r ) const
{
  ofstream file( path_r.asString().c_str() );
  doExport( file );
  if ( !file.good() ) {
    ERR << "Error writing file '" << path_r << "' ("
      << (file.good()?'g':'_') << (file.eof()?'e':'_')
      << (file.fail()?'F':'_') << (file.bad()?'B':'_') << ")" << endl;
    return false;
  }
  return true;
}

///////////////////////////////////////////////////////////////////
//
// clumsy TaggedFile interface
//
///////////////////////////////////////////////////////////////////
#include <y2util/TaggedFile.h>

struct TagDescr {
  std::string _name;
  bool        _multi;

  TagDescr()
    : _multi( false )
  {}
  TagDescr( const std::string & name, bool multi )
    : _name( name )
    , _multi( multi )
  {}

  bool isSingle() const { return !_multi; }
  bool isMulti()  const { return _multi; }

  const std::string & name() const { return _name; }

  std::ostream & writeStart( std::ostream & str ) const {
    if ( _multi ) {
      str << "+" << _name << ":" << endl;
    } else {
      str << "=" << _name << ": ";
    }
    return str;
  }

  std::ostream & writeEnd( std::ostream & str ) const {
    if ( _multi ) {
      str << "-" << _name << ":" << endl;
    } else {
      str << endl;
    }
    return str;
  }
};

class Mtags {

  public:

    enum tags {
      RLC = 0,
      ON_S,
      OFF_S,
      ON_P,
      ON_TP,
      OFF_P,
      OFF_TP,
      // last entry
      NUMTAGS
    };

  private:

    static const std::vector<TagDescr> _tagvec;

    static std::vector<TagDescr> init() {
      std::vector<TagDescr> tagvec;
      tagvec.resize( NUMTAGS );
      for ( unsigned i = 0; i < NUMTAGS; ++i ) {
	switch ( (tags)i ) {
#define Mstag(t,v,m) case t: tagvec[i] = TagDescr(v,m); break
	  Mstag(RLC,	"Rlc",	true);
	  Mstag(ON_S,	"Ons",	true);
	  Mstag(OFF_S,	"Offs",	true);
	  Mstag(ON_P,	"Onp",	true);
	  Mstag(ON_TP,	"Ontp",	true);
	  Mstag(OFF_P,	"Offp",	true);
	  Mstag(OFF_TP,	"Offtp",true);
	  // no default:
	  case NUMTAGS: break;
	}
      }
      return tagvec;
    }

  public:

    static const TagDescr & tag( tags t ) { return _tagvec[t]; }

    static void initTagSet( TaggedFile::TagSet & tagset_r ) {
      tagset_r.setAllowMultipleSets( false );	// no multiple tagsets per file
      tagset_r.setAllowUnknownTags( true );	// skip unknown tags
      for ( unsigned i = 0; i < _tagvec.size(); ++i ) {
	// i corresponds to enum tags!
	tagset_r.addTag( _tagvec[i]._name, i, (_tagvec[i]._multi ? TaggedFile::MULTI : TaggedFile::SINGLE) );
      }
    }

    static std::string strStatus( TaggedFile::assignstatus status ) {
      switch ( status ) {
#define OUTSTR(v) case TaggedFile::v: return #v
	OUTSTR( ACCEPTED );
	OUTSTR( ACCEPTED_FULL );
	OUTSTR( REJECTED_EOF );
	OUTSTR( REJECTED_NOMATCH );
	OUTSTR( REJECTED_LOCALE );
	OUTSTR( REJECTED_NOLOCALE );
	OUTSTR( REJECTED_FULL );
	OUTSTR( REJECTED_NOENDTAG );
#undef OUTSTR
      }
      return stringutil::numstring( status );
    }

  private:

      TaggedFile::TagSet * _tagsetptr;
      std::istream &       _str;

  public:

    Mtags( std::istream & str ) : _tagsetptr( 0 ), _str( str ) {}
    ~Mtags() { delete _tagsetptr; }

  public:

    TaggedFile::assignstatus assignSet() {
      delete _tagsetptr;
      _tagsetptr = new TaggedFile::TagSet;
      initTagSet( *_tagsetptr );

      TaggedParser parser;
      TaggedFile::assignstatus status = _tagsetptr->assignSet( parser, _str );

      if ( status != TaggedFile::ACCEPTED_FULL ) {
	ERR << "Parse error(" << strStatus( status ) << ") at line " << parser.lineNumber()
	  << ". Last tag read: " << parser.currentTag() << endl;
      }
      return status;
    }

    bool getData( tags tag_r, std::list<std::string> & mdata_r ) {
      mdata_r.clear();

      if ( !_tagsetptr )
	return false;

      if ( _tagvec[tag_r]._multi ) {
	_tagsetptr->getTagByIndex( tag_r )->Pos().retrieveData( _str, mdata_r );
      } else {
	mdata_r.push_back( _tagsetptr->getTagByIndex( tag_r )->Data() );
      }

      return true;
    }
};

///////////////////////////////////////////////////////////////////

const std::vector<TagDescr> Mtags::_tagvec( Mtags::init() );

///////////////////////////////////////////////////////////////////

/******************************************************************
**
**
**	FUNCTION NAME : doSet...
**	FUNCTION TYPE : void
**
**	DESCRIPTION :
*/
void doSet( std::list<LangCode> & data, const std::list<std::string> & values )
{
  data.clear();
  for ( std::list<std::string>::const_iterator i = values.begin(); i != values.end(); ++i ) {
    data.push_back( LangCode(*i) );
  }
}
void doSet( std::set<PkgNameEd> & data, const std::list<std::string> & values )
{
  data.clear();
  for ( std::list<std::string>::const_iterator i = values.begin(); i != values.end(); ++i ) {
    data.insert( PkgNameEd::fromString( *i ) );
  }
}
void doSet( std::set<PkgName> & data, const std::list<std::string> & values )
{
  data.clear();
  for ( std::list<std::string>::const_iterator i = values.begin(); i != values.end(); ++i ) {
    data.insert( PkgName(*i) );
  }
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doImport
//	METHOD TYPE : std::istream &
//
//	DESCRIPTION :
//
std::istream & PMPackageImEx::doImport( std::istream & str )
{
  reset();
  Magic magic;
  magic.readFrom( str );
  if ( magic._version == _ImExMagic._version ) {
    Mtags parser( str );

    if ( parser.assignSet() != TaggedFile::ACCEPTED_FULL ) {
      str.setstate( ios::failbit );
      return str;
    }

    std::list<std::string> mdata;
    parser.getData( Mtags::RLC, mdata );
    doSet( _requestedLocales, mdata );
    parser.getData( Mtags::ON_S, mdata );
    doSet( _onSystemSel, mdata );
    parser.getData( Mtags::OFF_S, mdata );
    doSet( _offSystemSel, mdata );
    parser.getData( Mtags::ON_P, mdata );
    doSet( _onSystemPkg, mdata );
    parser.getData( Mtags::ON_TP, mdata );
    doSet( _onSystemTabooPkg, mdata );
    parser.getData( Mtags::OFF_P, mdata );
    doSet( _offSystemPkg, mdata );
    parser.getData( Mtags::OFF_TP, mdata );
    doSet( _offSystemTabooPkg, mdata );

    DBG << "Imported: " << *this << endl;

  } else {
    INT << "Don't know how to handle PackageImEx " << magic._version << endl;
    str.setstate( ios::failbit );
  }

  return str;
}

/******************************************************************
**
**
**	FUNCTION NAME : doWrite...
**	FUNCTION TYPE : void
**
**	DESCRIPTION :
*/
void doWrite( std::ostream & str, const TagDescr & tag, const std::list<LangCode> & data )
{
  tag.writeStart( str );
  for ( std::list<LangCode>::const_iterator i = data.begin(); i != data.end(); ++i ) {
    str << (*i) << endl;
  }
  tag.writeEnd( str );
}
void doWrite( std::ostream & str, const TagDescr & tag, const std::set<PkgNameEd> & data )
{
  tag.writeStart( str );
  for ( std::set<PkgNameEd>::const_iterator i = data.begin(); i != data.end(); ++i ) {
    str << PkgNameEd::toString( *i ) << endl;
  }
  tag.writeEnd( str );
}
void doWrite( std::ostream & str, const TagDescr & tag, const std::set<PkgName> & data )
{
  tag.writeStart( str );
  for ( std::set<PkgName>::const_iterator i = data.begin(); i != data.end(); ++i ) {
    str << (*i) << endl;
  }
  tag.writeEnd( str );
}

///////////////////////////////////////////////////////////////////
//
//
//	METHOD NAME : PMPackageImEx::doExport
//	METHOD TYPE : std::ostream &
//
//	DESCRIPTION :
//
std::ostream & PMPackageImEx::doExport( std::ostream & str ) const
{
  if ( str ) {
    _ImExMagic.writeOn( str );
    // write data
    doWrite( str, Mtags::tag( Mtags::RLC ), _requestedLocales );
    doWrite( str, Mtags::tag( Mtags::ON_S ), _onSystemSel );
    doWrite( str, Mtags::tag( Mtags::OFF_S ), _offSystemSel );
    doWrite( str, Mtags::tag( Mtags::ON_P ), _onSystemPkg );
    doWrite( str, Mtags::tag( Mtags::ON_TP ), _onSystemTabooPkg );
    doWrite( str, Mtags::tag( Mtags::OFF_P ), _offSystemPkg );
    doWrite( str, Mtags::tag( Mtags::OFF_TP ), _offSystemTabooPkg );
    str << endl << "## END OF FILE" << endl;
  }
  return str;
}

