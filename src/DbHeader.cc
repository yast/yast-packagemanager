#include <string>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <unistd.h>
#include <errno.h>

#include <y2pm/PkgEdition.h>
#include <y2pm/PMPackage.h>

#include <y2pm/PMPackageDataProvider.h>

#include "y2pm/PMDequeInserter.h"

#include "y2pm/DbHeader.h"

using namespace std;
static const struct tag_desc {
	const char *name;
} known_tags[] =
{
    { "Name" },
    { "Edition" },
    { "Arch" },
    { "Requires" },
    { "Conflicts" },
    { "Provides" },
    { "Obsoletes" },
    { "Files" },
    { NULL }
};

/*** exception.h ***/
struct PkgDbExcp {
  protected:
	string name;
  public:
	PkgDbExcp( string str ) : name(str)
	{
	}
	PkgDbExcp() : name("") {}
	virtual ~PkgDbExcp() {}
	const char *opname() { return name.c_str(); }
	virtual void print( ostream& os ) const;

	friend ostream& operator<<( ostream& os, const PkgDbExcp& e );
};

struct PkgDbFileExcp : public PkgDbExcp {
  protected:
	int err;
  public:
	PkgDbFileExcp(string str, int e = -1) : PkgDbExcp(str) {
		err = (e == -1) ? errno : e;
	}
	void print( ostream& os ) const;
};

struct PkgDbNoTagExcp : public PkgDbExcp {
  protected:
	int tagno;
  public:
	PkgDbNoTagExcp() : PkgDbExcp("tag not found") {}
	PkgDbNoTagExcp(int tag) : PkgDbExcp("tag not found"), tagno(tag) {}
	void print( ostream& os ) const;
};

struct PkgDbReadTagExcp : public PkgDbExcp {
  protected:
	int line;
  public:
	PkgDbReadTagExcp(string str, int l = 0) : PkgDbExcp(str), line(l) {}
	void print( ostream& os ) const;
};

/************** /exception.h ***/


/***** exception.cc ***********/
ostream& operator<<( ostream& os, const PkgDbExcp& e )
{
	e.print( os );
	return os;
}

void PkgDbExcp::print( ostream& os ) const
{
	os << name;
}

void PkgDbFileExcp::print( ostream& os ) const
{
	os << name;
	if (err)
		os << ": " << strerror(err);
}

void PkgDbNoTagExcp::print( ostream& os ) const
{
	if (tagno)
		os << "tag " << tagno << " not found";
	else
		os << name;
}

void PkgDbReadTagExcp::print( ostream& os ) const
{
	os << name;
	if (line)
		os << " (line " << line << ")";
}

/***** /exception.cc ***********/

class Tag
{
    public:
	enum ovmode { NONE, ADD, REMOVE, REPLACE };
};

DEFINE_DERIVED_POINTER(PMFakeVendorPackageDataProvider,PMPackageDataProvider);

// default dataprovider, except returns specified vendor to avoid packages
// beeing set to protected
class PMFakeVendorPackageDataProvider : public PMPackageDataProvider  {
    REP_BODY(PMFakeVendorPackageDataProvider);

    private:
	Vendor _vendor;

    public:

	PMFakeVendorPackageDataProvider(Vendor v) : _vendor(v) {};
	virtual ~PMFakeVendorPackageDataProvider() {};

	virtual Vendor instSrcVendor( const PMPackage & pkg_r ) const
	{
	    return _vendor;
	}

	virtual Vendor vendor( const PMPackage & pkg_r ) const
	{
	    return _vendor;
	}
};

IMPL_DERIVED_POINTER(PMFakeVendorPackageDataProvider,PMPackageDataProvider);

class DbHeader
{
    private:
	static PMPackageDataProviderPtr _datap;
	static std::set<string>* _known_tags;
	typedef std::map<std::string,std::string> TagMap;
	TagMap _tags;

    public:
	DbHeader();
	void read( istream& is, int *lineno, bool is_override );
	bool addtag(std::string name, std::string value);
	PMPackagePtr toPackage();
};

std::set<string>* DbHeader::_known_tags = NULL;
PMPackageDataProviderPtr DbHeader::_datap = NULL;

DbHeader::DbHeader()
{
    if(!_known_tags)
    {
	const struct tag_desc *td;
	_known_tags = new std::set<string>;
	for( td = known_tags; td->name; ++td )
	{
	    _known_tags->insert(string(td->name));
	}
    }
    if(!_datap)
	_datap = new PMFakeVendorPackageDataProvider(Vendor("SuSE"));
}


bool DbHeader::addtag(std::string name, std::string value)
{
    if(_known_tags->count(name) == 0)
	return false;
    _tags[name]=value;
    return true;
}

static string strip_ws( string s )
{
	while( s.length() && isspace( s[0] ))
		s = s.substr( 1 );
	while( s.length() && isspace( s[s.length()-1] ))
		s = s.substr( 0, s.length()-1 );
	return s;
}

// split "foo, bar >= 4, blub" into { "foo","bar >= 4", "blub" }
static std::list<std::string> relstring2strings(std::string str)
{
    std::list<std::string> strlist;
    for( size_t start = 0; start < str.length(); ) {
	size_t end = str.find_first_of( ",\n", start );
	string substr = stringutil::trim(str.substr( start, end-start ));
	if(substr.length() > 0)
	    strlist.push_back(substr);
	start = (end == string::npos) ? end : end+1;
    }

    return strlist;
}


PMPackagePtr DbHeader::toPackage()
{
    TagMap::iterator it;
    PkgName name;
    PkgEdition edition;
    PkgArch arch;
    PMPackagePtr pkg;

    it = _tags.find("Name");
    if(it == _tags.end())
	PkgDbExcp( "Required tag \"Name\" not found" );
    else
	name = PkgName(it->second);

    it = _tags.find("Edition");
    if(it == _tags.end())
	PkgDbExcp( "Required tag \"Edition\" not found" );
    else
	edition = PkgEdition::fromString(it->second);

    it = _tags.find("Arch");
    if(it == _tags.end())
	arch = PkgArch("noarch");
    else
	arch = PkgArch(it->second);

    pkg = new PMPackage (name, edition, arch, _datap);

    it = _tags.find("Requires");
    if(it != _tags.end())
    {
	PMSolvable::PkgRelList_type rellist;
	std::list<std::string> pkglist;
	pkglist = relstring2strings(it->second);
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	pkg->setRequires(rellist);
    }


    it = _tags.find("Provides");
    if(it != _tags.end())
    {
	PMSolvable::PkgRelList_type rellist;
	std::list<std::string> pkglist;
	pkglist = relstring2strings(it->second);
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	pkg->setProvides(rellist);
    }

    // add files as provides
    it = _tags.find("Files");
    if(it != _tags.end())
    {
	std::list<std::string> pkglist;
	pkglist = relstring2strings(it->second);

	for(std::list<std::string>::iterator fit = pkglist.begin(); fit != pkglist.end(); ++fit)
	{
	    pkg->addProvides(PkgName(*fit));
	}
    }

    it = _tags.find("Conflicts");
    if(it != _tags.end())
    {
	PMSolvable::PkgRelList_type rellist;
	std::list<std::string> pkglist;
	pkglist = relstring2strings(it->second);
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	pkg->setConflicts(rellist);
    }

    it = _tags.find("Obsoletes");
    if(it != _tags.end())
    {
	PMSolvable::PkgRelList_type rellist;
	std::list<std::string> pkglist;
	pkglist = relstring2strings(it->second);
	rellist = PMSolvable::StringList2PkgRelList (pkglist);
	pkg->setObsoletes(rellist);
    }

    return pkg;
}

#define line			(lineno ? *lineno : 0)
#define prev_line		(lineno ? *lineno-1 : 0)
/*
static void parse_relation( string s, const struct tag_desc *td,
							DbHeader& hdr, int *lineno )
{
	string name, rel, vers;
	size_t p1 = s.find_first_of( " \t" );
	if (p1 != string::npos) {
		name = s.substr( 0, p1 );
		size_t p2 = s.find_first_not_of( " \t", p1 );
		size_t p3 = s.find_first_of( " \t", p2 );
		if (p2 == string::npos)
			throw PkgDbReadTagExcp( "no relation operator found", prev_line );
		rel = s.substr( p2, p3-p2 );
		size_t p4 = s.find_first_not_of( " \t", p3 );
		size_t p5 = s.find_first_of( " \t", p4 );
		if (p4 == string::npos)
			throw PkgDbReadTagExcp( "no version after relation operator",
									prev_line );
		vers = s.substr( p4, p5-p4 );
	}
	else
		name = s;

	const char *p = name.c_str();
	hdr.extend_tag( Tag( td->tag, &p ));

	if (td->tag2 != RPMTAG_DISCARD || td->tag3 != RPMTAG_DISCARD) {
		int_32 v;
		if (rel.length() == 0)
			v = RPMSENSE_ANY;
		else if (rel == "=")
			v = RPMSENSE_EQUAL;
		else if (rel == "<")
			v = RPMSENSE_LESS;
		else if (rel == "<=")
			v = RPMSENSE_LESS|RPMSENSE_EQUAL;
		else if (rel == ">")
			v = RPMSENSE_GREATER;
		else if (rel == ">=")
			v = RPMSENSE_GREATER|RPMSENSE_EQUAL;
		else
			throw PkgDbReadTagExcp( "bad relation operator "+rel, prev_line );

		const char *p = vers.c_str();

		// omit "= -*-UNSPEC-*-" and "= -*-MAXIMUM-*-" relations; they're
		// internally generated and will be regenerated
		if (rel == "=" && (vers == "-*-UNSPEC-*-" || vers == "-*-MAXIMUM-*-")){
			v = 0;
			vers = "";
		}
		hdr.extend_tag( Tag( td->tag2, &v ));
		hdr.extend_tag( Tag( td->tag3, &p ));
	}
}

static void parse_edition( string str, int& epoch, string& version,
						   string& release )
{
	size_t pos;

	if ((pos = str.find_first_not_of( "0123456789" )) > 0 &&
		pos != string::npos && str[pos] == ':') {
		epoch = atoi(str.c_str());
		str = str.substr( pos+1 );
	}

	if ((pos = str.rfind( '-' )) != string::npos) {
		release = str.substr( pos+1 );
		str = str.substr( 0, pos );
	}

	version = str;
}

static inline bool is_override_key_tag( int_32 tag )
{
	return( tag == RPMTAG_NAME || tag == RPMTAG_EPOCH ||
			tag == RPMTAG_VERSION || tag == RPMTAG_RELEASE );
}

static inline bool is_array_tag( int_32 tag )
{
	return( tag == RPMTAG_REQUIRENAME || tag == RPMTAG_CONFLICTNAME ||
			tag == RPMTAG_PROVIDES || tag == RPMTAG_OBSOLETES ||
			tag == RPMTAG_FILENAMES );
}

*/


#define check_is(eof_allowed)									\
	do {														\
		if ((!eof_allowed && is.eof()) || is.fail())			\
			throw PkgDbReadTagExcp("EOF while reading tags");	\
	} while(0)

#define add_line()		do { if (lineno) ++*lineno; } while(0)
#define add_line_if(c)	do { if ((c) == '\n' && lineno) ++*lineno; } while(0)

static bool parse_one_tag( istream& is, DbHeader& hdr, int *lineno,
						   bool is_override )
{
	if (is.eof() || is.fail())
		// no more tags -> end of header
		return false;

	char c;
	string tagname, value;
	Tag::ovmode override_mode = Tag::NONE;

	// read tag name and optional override mode prefix
  repeat:
	is.get(c);
	add_line_if(c);
	if (c == '\n') {
		// empty line marks end of header; eat all consec. newlines
		while( is.good() && (is.get(c), c == '\n') )
			;
		if (c != '\n')
			is.putback(c);
		return false;
	}
	if(is.eof()) return false;
	if (is_override && (c == '+' || c == '-' || c == '!')) {
		switch( c ) {
		  case '+':	override_mode = Tag::ADD; break;
		  case '-':	override_mode = Tag::REMOVE; break;
		  case '!':	override_mode = Tag::REPLACE; break;
		}
	}
	else if (c == '#') {
		// comment
		while( is.good() ) {
			is.get(c);
			if (c == '\n') {
				add_line();
				break;
			}
		}
		goto repeat;
	}
	else
		tagname += c;

	while( is.good() ) {
		is.get(c);
		if (c == ':')
			break;
		add_line_if(c);
		if (isspace(c))
			throw PkgDbReadTagExcp( "whitespace instead of colon "
									"after tag name", line );
		tagname += c;
	}
	check_is(false);

	// skip whitespace before value
	// newline after ws can mean empty value or first value on continuation
	// line!
	while( is.good() ) {
		is.get(c);
		if (c == '\n') {
			is.get(c);
			if (c != ' ' && c != '\t') {
				is.putback(c);
				is.putback('\n');
				break;
			}
			add_line();
		}
		else if (c != ' ' && c != '\t') {
			value += c;
			break;
		}
	}
	check_is(false);

	while(true) {
		// read value until newline
		while( is.good() ) {
			is.get(c);
			if (c == '\n') {
				add_line();
				break;
			}
			value += c;
		}

		// check if next line is continuation line
		if (!is.good())
			break;
		is.get(c);
		if (is.eof()) {
			is.clear( ios::eofbit );
			break;
		}
		add_line_if(c);
		if (c != ' ' && c != '\t') {
			is.putback( c );
			break;
		}
		value += '\n';
	}
	check_is(true);

	value = stringutil::trim( value );
//	cout << "Line " << prev_line << ": Found tagname \"" << tagname << "\", value \"" << value << "\"\n";

	if(!hdr.addtag(tagname,value))
	    throw PkgDbReadTagExcp( string("unknown tag name \"") + tagname + "\"", prev_line );

	/*

	int_32 tagno;
	if (strcasecmp( tagname.c_str(), "edition" ) == 0) {
		// edition is a special case, as it doesn't translate into only one tag
		int epoch = 0;
		string version, release;
		parse_edition( value, epoch, version, release );
		if (epoch)
			hdr.add_tag( Tag( RPMTAG_EPOCH, &epoch ));
		if (version.length())
			hdr.add_tag( Tag( RPMTAG_VERSION, version.c_str() ));
		if (release.length())
			hdr.add_tag( Tag( RPMTAG_RELEASE, release.c_str() ));
		tagno = RPMTAG_VERSION;
	}
	else {
		const struct tag_desc *td;
		for( td = known_tags; td->name; ++td ) {
			if (strcasecmp( td->name, tagname.c_str() ) == 0)
				break;
		}
		if (!td->name)
			throw PkgDbReadTagExcp( string("unknown tag name \"") +
									tagname + "\"", prev_line );
		tagno = td->tag;

		if (!td->is_array) {
			if (td->type == RPM_STRING_TYPE) {
				hdr.add_tag( Tag(td->tag, td->type, (void *)value.c_str(), 1));
			}
			else if (td->type == RPM_INT32_TYPE) {
				int_32 ival = atoi( value.c_str() );
				hdr.add_tag( Tag( td->tag, td->type, (void *)&ival, 1 ));
			}
			else if (td->type == RPM_BIN_TYPE) {
				unsigned l = value.length()/2;
				char buf[l];
				const char *p = value.c_str();
				for( unsigned i = 0; i < l; ++i, p += 2 ) {
					char v;
					if (!isxdigit(p[0]) || !isxdigit(p[1]))
						throw PkgDbReadTagExcp( string("invalid hex "
													   "representation `") +
												p[0] + p[1] + "'", line );
					v = isdigit(p[0]) ? p[0] - '0' : tolower(p[0]) - 'a' + 10;
					v <<= 4;
					v |= isdigit(p[1]) ? p[1] - '0' : tolower(p[1]) - 'a' + 10;
					buf[i] = v;
				}
				hdr.add_tag( Tag( td->tag, td->type, (void *)buf, l ));
			}
			else {
				assert( td->type == RPM_STRING_TYPE ||
						td->type == RPM_INT32_TYPE ||
						td->type == RPM_BIN_TYPE );
			}
		}
		else {
			assert( td->type == RPM_STRING_TYPE );
			for( size_t start = 0; start < value.length(); ) {
				size_t end = value.find_first_of( ",\n", start );
				string subv = strip_ws(value.substr( start, end-start ));
				if (td->tag2 && td->tag3)
					parse_relation( subv, td, hdr, lineno );
				else {
					const char *p = subv.c_str();
					hdr.extend_tag( Tag( td->tag, &p ));
				}
				start = (end == string::npos) ? end : end+1;
			}
		}
	}

	if (is_override) {
		// store override mode in own tags PKGDBTAG_OVERRIDE{TAG,MODE}
		if (override_mode != Tag::NONE && is_override_key_tag(tagno))
			throw PkgDbReadTagExcp( "override key tag "+tagname+
									" has override marker", prev_line );
		if (override_mode == Tag::NONE && !is_override_key_tag(tagno))
			throw PkgDbReadTagExcp( "override marker missing for "+tagname,
									prev_line );
		if ((override_mode == Tag::ADD || override_mode == Tag::REMOVE) &&
			!is_array_tag(tagno))
			throw PkgDbReadTagExcp( "+ or - marker used on non-array field "+
									tagname, prev_line );
		if (override_mode != Tag::NONE) {
			int_32 int_om = int(override_mode);
			hdr.extend_tag( Tag( PKGDBTAG_OVERRIDETAG, &tagno ));
			hdr.extend_tag( Tag( PKGDBTAG_OVERRIDEMODE, &int_om ));
		}
	}
	*/
	return true;
}



istream& operator>>( istream& is, DbHeader& hdr )
{
	while( parse_one_tag( is, hdr, NULL, false ) )
		;
	return is;
}

void DbHeader::read( istream& is, int *lineno, bool is_override )
{
	while( parse_one_tag( is, *this, lineno, is_override ) )
		;
}

PMError read_package_list(PMInserter<PMPackagePtr>& dest, string file)
{
    PMError err = PMError::E_ok;
    ifstream is;
    int lineno = 0;

    is.open(file.c_str());

    if(!is)
    {
	err = PMError::E_error;
	err.setDetails(stringutil::form("could not open %s", file.c_str()));
	return err;
    }

    try
    {
	while( !is.eof() && !is.fail() )
	{
	    DbHeader hdr;
	    hdr.read( is, &lineno, false );
	    PMPackagePtr p = hdr.toPackage();
	    if(p)
	    {
		dest += p;
	    }
	}
    }
    catch( PkgDbExcp& e )
    {
	stringstream s;
	s << file << ": " << e << endl;

	err = PMError::E_error;
	err.setDetails(s.str());
	return err;
    }

    is.close();

    return err;
}

// vim: sw=4
