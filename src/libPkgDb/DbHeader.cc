#warning __FILE__
#if 0 //rpm dependant
#include <string>
#include <cstring>
#include <cctype>
#include <cassert>
#include <iostream>
#include <strstream>
#include <unistd.h>
#include <DbHeader.h>
#include <rpm/rpmlib.h>

#ifndef RPMTAG_FILENAMES
# define RPMTAG_FILENAMES RPMTAG_OLDFILENAMES
#endif

const Tag::tag2name_type Tag::tag2name[] = {
	{ RPMTAG_NAME,			  "Name" },
	{ RPMTAG_VERSION,		  "Version" },
	{ RPMTAG_RELEASE,		  "Release" },
	{ RPMTAG_EPOCH,			  "Epoch" },
	{ RPMTAG_ARCH,			  "Arch" },
	{ RPMTAG_REQUIRENAME,	  "Requires-Names" },
	{ RPMTAG_REQUIREFLAGS,	  "Requires-Flags" },
	{ RPMTAG_REQUIREVERSION,  "Requires-Versions" },
	{ RPMTAG_CONFLICTNAME,	  "Conflicts-Names" },
	{ RPMTAG_CONFLICTFLAGS,	  "Conflicts-Flags" },
	{ RPMTAG_CONFLICTVERSION, "Conflicts-Versions" },
	{ RPMTAG_PROVIDES,		  "Provides" },
	{ RPMTAG_OBSOLETES,		  "Obsoletes" },
	{ RPMTAG_FILENAMES,		  "Files" },
	{ RPMTAG_SIZE,			  "Size" },
	{ RPMTAG_SUMMARY,         "Summary" },
	{ RPMTAG_GROUP,		      "Group" },
	{ RPMTAG_PACKAGER,        "Packager" },
	{ RPMTAG_DESCRIPTION,     "Description" },
	{ RPMTAG_DISTRIBUTION,    "Distribution" },
	{ RPMTAG_VENDOR,          "Vendor" },
	{ RPMTAG_SOURCERPM,       "Source-RPM" },
	{ RPMTAG_LICENSE,         "License" },
	{ RPMTAG_CHANGELOGTIME,   "Changelog-Time" },
	{ RPMTAG_CHANGELOGNAME,   "Changelog-Name" },
	{ RPMTAG_CHANGELOGTEXT,   "Changelog" },
	{ RPMTAG_PREIN,			  "Prein" },
	{ RPMTAG_POSTIN,          "Postin" },
	{ RPMTAG_PREUN,			  "Preun" },
	{ RPMTAG_POSTUN,          "Postun" },
	{ RPMTAG_PREINPROG,		  "Preinprog" },
	{ RPMTAG_POSTINPROG,      "Postinprog" },
	{ RPMTAG_PREUNPROG,		  "Preunprog" },
	{ RPMTAG_POSTUNPROG,      "Postunprog" },
	{ RPMTAG_GIF,             "GIF" },
	{ RPMTAG_XPM,             "XPM" },
	{ RPMTAG_ICON,            "Icon" }
};

string Tag::tagname( int_32 tagno )
{
	unsigned i;
	const tag2name_type *p;
	
	for( i = 0, p = tag2name; i < sizeof(tag2name)/sizeof(tag2name_type);
		 ++i, ++p ) {
		if (p->tag == tagno)
			return string(p->name);
	}
	char str[20];
	ostrstream os(str,sizeof(str));
	os << "#" << tagno << ends;
	return string(os.str());
}



DbHeader::DbHeader( const char *fn, PFD_t fd )
{
	off_t offs = pfdLseek( fd, 0, SEEK_CUR );
	off_t eof = pfdLseek( fd, 0, SEEK_END );

	if (offs == eof)
		throw PkgDbExcp( "EOF while reading header" );
	pfdLseek( fd, offs, SEEK_SET );
	
	if (!(header = PkgDb_headerRead( fd, HEADER_MAGIC_YES )))
		throw PkgDbExcp( "can't read header" );
	_offset = offs;
	_filename = strdup(fn);
}

DbHeader::DbHeader( const char *fn, off_t offs )
{
	PFD_t fd = pfdOpen( fn, O_RDONLY );
	if (!pfdValid(fd))
		throw PkgDbFileExcp(fn);
	
	if (pfdLseek( fd, offs, SEEK_SET ) != offs) {
		pfdClose( fd );
		throw PkgDbExcp(string(fn) + ": seek failed");
	}		

	if (!(header = PkgDb_headerRead( fd, HEADER_MAGIC_YES ))) {
		pfdClose( fd );
		throw PkgDbExcp( "can't read header" );
	}
	_offset = offs;
	_filename = strdup(fn);
	pfdClose( fd );
}


#define RPMTAG_DISCARD	1

static const struct tag_desc {
	const char *name;
	bool is_array;
	int_32 type;
	int_32 tag;
	int_32 tag2, tag3;
} known_tags[] = {
	{ "Name",			false, RPM_STRING_TYPE, RPMTAG_NAME, 0, 0, },
	{ "Version",		false, RPM_STRING_TYPE, RPMTAG_VERSION, 0, 0, },
	{ "Arch",		false, RPM_STRING_TYPE, RPMTAG_ARCH, 0, 0, },
	{ "Release",		false, RPM_STRING_TYPE, RPMTAG_RELEASE, 0, 0, },
	{ "Epoch",			false, RPM_INT32_TYPE,  RPMTAG_EPOCH, 0, 0, },
	{ "Serial",			false, RPM_INT32_TYPE,  RPMTAG_EPOCH, 0, 0, },
	{ "Requires",		true,  RPM_STRING_TYPE, RPMTAG_REQUIRENAME,
							   RPMTAG_REQUIREFLAGS, RPMTAG_REQUIREVERSION, },
	{ "Conflicts",		true,  RPM_STRING_TYPE, RPMTAG_CONFLICTNAME,
							   RPMTAG_CONFLICTFLAGS, RPMTAG_CONFLICTVERSION, },
	{ "Provides",		true,  RPM_STRING_TYPE, RPMTAG_PROVIDES,
							   RPMTAG_DISCARD, RPMTAG_DISCARD },
	{ "Obsoletes",		true,  RPM_STRING_TYPE, RPMTAG_OBSOLETES,
							   RPMTAG_DISCARD, RPMTAG_DISCARD },
	{ "Files",			true,  RPM_STRING_TYPE, RPMTAG_FILENAMES, 0, 0 },
	{ "Size",			false, RPM_INT32_TYPE,  RPMTAG_SIZE, 0, 0 },
	{ "Summary",		false, RPM_STRING_TYPE, RPMTAG_SUMMARY, 0, 0 },
	{ "Group",			false, RPM_STRING_TYPE, RPMTAG_GROUP, 0, 0 },
	{ "Packager",		false, RPM_STRING_TYPE, RPMTAG_PACKAGER, 0, 0 },
	{ "Description",	false, RPM_STRING_TYPE, RPMTAG_DESCRIPTION, 0, 0 },
	{ "Distribution",	false, RPM_STRING_TYPE, RPMTAG_DISTRIBUTION, 0, 0 },
	{ "Vendor",			false, RPM_STRING_TYPE, RPMTAG_VENDOR, 0, 0 },
	{ "Source-RPM",		false, RPM_STRING_TYPE, RPMTAG_SOURCERPM, 0, 0 },
	{ "License",		false, RPM_STRING_TYPE, RPMTAG_LICENSE, 0, 0 },
	{ "Changelog-Name",	false, RPM_STRING_TYPE, RPMTAG_CHANGELOGNAME, 0, 0 },
	{ "Changelog",		false, RPM_STRING_TYPE, RPMTAG_CHANGELOGTEXT, 0, 0 },
	{ "Prein",			false, RPM_STRING_TYPE, RPMTAG_PREIN, 0, 0 },
	{ "Postin",			false, RPM_STRING_TYPE, RPMTAG_POSTIN, 0, 0 },
	{ "Preun",			false, RPM_STRING_TYPE, RPMTAG_PREUN, 0, 0 },
	{ "Postun",			false, RPM_STRING_TYPE, RPMTAG_POSTUN, 0, 0 },
	{ "Preinprog",		false, RPM_STRING_TYPE, RPMTAG_PREINPROG, 0, 0 },
	{ "Postinprog",		false, RPM_STRING_TYPE, RPMTAG_POSTINPROG, 0, 0 },
	{ "Preunprog",		false, RPM_STRING_TYPE, RPMTAG_PREUNPROG, 0, 0 },
	{ "Postunprog",		false, RPM_STRING_TYPE, RPMTAG_POSTUNPROG, 0, 0 },
	{ "GIF",			false, RPM_BIN_TYPE,    RPMTAG_GIF, 0, 0 },
	{ "XPM",			false, RPM_BIN_TYPE,    RPMTAG_XPM, 0, 0 },
	{ "Icon",			false, RPM_BIN_TYPE,    RPMTAG_ICON, 0, 0 },
	{ NULL,				false, 0, 0, 0, 0 }
};


static string strip_ws( string s )
{
	while( s.length() && isspace( s[0] ))
		s = s.substr( 1 );
	while( s.length() && isspace( s[s.length()-1] ))
		s = s.substr( 0, s.length()-1 );
	return s;
}

#define line			(lineno ? *lineno : 0)
#define prev_line		(lineno ? *lineno-1 : 0)

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

	value = strip_ws( value );
	// cout << "Line " << prev_line << ": Found tagname \"" << tagname << "\", value \"" << value << "\"\n";

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



// Local Variables:
// tab-width: 4
// End:
#endif
