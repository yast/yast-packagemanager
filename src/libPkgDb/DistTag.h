#ifndef _PkgDb_DistTag_h
#define _PkgDb_DistTag_h

#error "obsolete"

// A DistTag stands for a group of packages that belong together (are in the
// same PkgSet later). It's currently simply a string.
class DistTag {
	const char *tag;
  public:
	DistTag( const char *p ) { tag = strdup(p); }
	DistTag( const string &p ) { tag = strdup(p.c_str()); }
	~DistTag() { free( (void*)tag ); }
	DistTag( const DistTag& t2 ) { tag = strdup(t2.tag); }
	DistTag& operator= ( const DistTag& t2 ) {
		free( (void*)tag );
		tag = strdup( t2.tag );
		return *this;
	}
	bool operator==( const DistTag& t2 ) const {
		return strcmp(tag,t2.tag) == 0; }
	bool operator!=( const DistTag& t2 ) const {
		return strcmp(tag,t2.tag) != 0; }
	operator const char* () const { return tag; }
};

const DistTag INSTALLED_Tag( "INSTALLED" );
const DistTag NONE_Tag( "" );

typedef list<DistTag> DistTagList;
typedef DistTagList::iterator DistTagList_iterator;
typedef DistTagList::const_iterator DistTagList_const_iterator;


// some abbrevs...
inline DistTagList DTagList1( const char *p1 ) {
	DistTagList dtl;
	dtl.push_front( DistTag(p1) );
	return dtl;
}
	
inline DistTagList DTagList2( const char *p1, const char *p2 ) {
	DistTagList dtl;
	dtl.push_front( DistTag(p1) );
	dtl.push_front( DistTag(p2) );
	return dtl;
}
	
inline DistTagList DTagList3( const char *p1, const char *p2, const char *p3) {
	DistTagList dtl;
	dtl.push_front( DistTag(p1) );
	dtl.push_front( DistTag(p2) );
	dtl.push_front( DistTag(p3) );
	return dtl;
}
	
inline DistTagList DTagListI0() {
	DistTagList dtl;
	dtl.push_front( INSTALLED_Tag );
	return dtl;
}

inline DistTagList DTagListI1( const char *p1 ) {
	DistTagList dtl;
	dtl.push_front( INSTALLED_Tag );
	dtl.push_front( DistTag(p1) );
	return dtl;
}

inline DistTagList DTagListI2( const char *p1, const char *p2 ) {
	DistTagList dtl;
	dtl.push_front( INSTALLED_Tag );
	dtl.push_front( DistTag(p1) );
	dtl.push_front( DistTag(p2) );
	return dtl;
}

inline DistTagList DTagListI3( const char *p1, const char *p2, const char *p3) {
	DistTagList dtl;
	dtl.push_front( INSTALLED_Tag );
	dtl.push_front( DistTag(p1) );
	dtl.push_front( DistTag(p2) );
	dtl.push_front( DistTag(p3) );
	return dtl;
}

#endif  /* _PkgDb_DistTag_h */


// Local Variables:
// tab-width: 4
// End:
