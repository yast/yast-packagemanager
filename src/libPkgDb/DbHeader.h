#if 0 // rpm dependant
#ifndef _PkgDb_Header_h
#define _PkgDb_Header_h

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <Exception.h>
#include <pkgdbheader.h>
#include <fcntl.h>

// own special tags in first header of a cache file
#define PKGDBTAG_START			200000
#define PKGDBTAG_VERSION		(PKGDBTAG_START+0)
#define PKGDBTAG_REQUIRED_FILES	(PKGDBTAG_START+1)
#define PKGDBTAG_OVERRIDETAG	(PKGDBTAG_START+2)
#define PKGDBTAG_OVERRIDEMODE	(PKGDBTAG_START+3)

#define PKGDB_CURRENT_VERSION	1

struct Tag {
	void *data;
	int_32 tag, type, count;
	int *refcnt;

	enum ovmode { NONE, ADD, REMOVE, REPLACE };
	struct tag2name_type {
		int_32 tag;
		const char *name;
	};

	Tag() : refcnt(NULL) {}
	Tag( const Tag& t2 ) { copy( t2 ); }
	~Tag() { destruct(); }
	Tag& operator= ( const Tag& t2 ) { destruct(); copy( t2 ); return *this; }

	Tag( int_32 t, int_32 *val )
		: data((void *)val), tag(t), type(RPM_INT32_TYPE), count(1), refcnt(NULL) {}
	Tag( int_32 t, const char *val )
		: data((void *)val), tag(t), type(RPM_STRING_TYPE), count(1), refcnt(NULL) {}
	Tag( int_32 t, const char **val )
		: data((void *)val), tag(t), type(RPM_STRING_ARRAY_TYPE), count(1), refcnt(NULL) {}
	Tag( int_32 t, int_32 ty, const char **val, int_32 co )
		: data((void *)val), tag(t), type(ty), count(co), refcnt(NULL) {}
	Tag( int_32 t, int_32 ty, void *val, int_32 co )
		: data((void *)val), tag(t), type(ty), count(co), refcnt(NULL) {}

	static string tagname( int_32 tagno );
	
  private:
	static const tag2name_type tag2name[];

	void destruct() {
		if (refcnt && --*refcnt == 0) {
			free(data);
			delete refcnt;
		}
	}
	void copy( const Tag& t2 ) {
		tag = t2.tag;
		type = t2.type;
		count = t2.count;
		data = t2.data;
		if ((refcnt = t2.refcnt))
			++*refcnt;
	}
};

template<class T>
struct typedTag {
	void *data;
	typedTag( const Tag& t ) : data(t.data) {}
	const T operator[] (unsigned i) {
		return ((T *)data)[i];
	}
};

// need specialization for T == char *: If type == STRING, the string is
// stored directly, otherwise (STRING_ARRAY) data points to an array of char *
struct typedTag<const char *> {
	void *data;
	int_32 count, type;
	typedTag( const Tag& t ) :
		data(t.data), count(t.count), type(t.type) {}
	const char * operator[] (unsigned i) {
		assert((int)i < count);
		if (type == RPM_STRING_ARRAY_TYPE)
			return ((const char **)data)[i];
		else
			return (const char *)data;
	}
};

typedef typedTag<int_16> shTag;
typedef typedTag<int_32> iTag;
typedef typedTag<const char *> cpTag;

class DbHeader {
	PkgDb_Header header;
	const char *_filename;
	off_t _offset; // -1 means
	
  public:
	class iterator {
		friend class DbHeader;
		enum { INIT, OK, END } state;
		PkgDb_HeaderIterator rpmI;
		Tag val;

	  public:
		iterator() : state(END) {};
		iterator( PkgDb_HeaderIterator i )
			: state(INIT), rpmI(i) { operator++(); };
		~iterator() { PkgDb_headerFreeIterator( rpmI ); }

		operator const void * () const {
			return state == OK ? (const void *)1 : 0; }
		bool operator! () const { return state != OK; }
		const Tag& operator* () {
			assert(state == OK);
			return val;
		}
		const Tag* operator-> () {
			assert(state == OK);
			return &val;
		}
		iterator& operator++ () {
			state = PkgDb_headerNextIterator( rpmI, &val.tag, &val.type,
											  &val.data, &val.count ) ? OK : END;
			if (state == OK && val.type == RPM_STRING_ARRAY_TYPE)
				val.refcnt = new int(1);
			return *this;
		}
	   iterator operator++ (int) {
		   iterator temp = *this;
		   operator++();
		   return temp;
	   }
	};
	friend class iterator;

	// read header from open file (at current position)
	DbHeader( const char *fn, PFD_t fd );

	// read header from file at position offs
	DbHeader( const char *fn, off_t offs );

	DbHeader( const DbHeader& hdr ) {
		_filename = strdup(hdr._filename);
		_offset = hdr._offset;
		header = PkgDb_headerCopy( hdr.header );
	}

	DbHeader( PkgDb_Header hdr, const char *fn = "", off_t offs = -1 ) {
		_filename = strdup(fn);
		_offset = offs;
		header = hdr;
	}
	
	DbHeader() {
		_filename = strdup("");
		_offset = 0;
		header = PkgDb_headerNew();
	}
	
	~DbHeader() {
		PkgDb_headerFree( header );
		free( (void *)_filename );
	}

	DbHeader& operator=( const DbHeader& hdr ) {
		PkgDb_headerFree( header );
		free( (void *)_filename );
		_filename = strdup(hdr._filename);
		_offset = hdr._offset;
		header = PkgDb_headerCopy( hdr.header );
		return *this;
	}
	
	const char *filename() const { return _filename; };
	off_t offset() const { return _offset; }
	
	iterator begin() const { return iterator( PkgDb_headerInitIterator(header) ); }
	iterator end() const { return iterator(); }

	Tag get_tag( int_32 tag ) const {
		Tag val;
		val.tag = tag;
		if (!PkgDb_headerGetEntry( header, tag, &val.type, &val.data, &val.count ))
			throw PkgDbNoTagExcp(tag);
		if (val.type == RPM_STRING_ARRAY_TYPE)
			val.refcnt = new int(1);
		return val;
	}
	Tag operator[]( int_32 tag ) const { return get_tag(tag); }
	bool has_tag( int_32 tag ) const {
		return PkgDb_headerIsEntry( header, tag );
	}
	
	void add_tag( const Tag& t ) {
		if (!PkgDb_headerAddEntry( header, t.tag, t.type, t.data, t.count ))
			throw PkgDbExcp( "headerAddEntry failed" );
	}
	void extend_tag( const Tag& t ) {
		if (!PkgDb_headerAddOrAppendEntry( header, t.tag, t.type,
										   t.data, t.count ))
			throw PkgDbExcp( "headerAddOrAppendEntry failed" );
	}
	void modify_tag( const Tag& t ) {
		if (!PkgDb_headerModifyEntry( header, t.tag, t.type, t.data, t.count ))
			throw PkgDbExcp( "headerModifyEntry failed" );
	}
	void remove_tag( int_32 tag ) {
		if (!PkgDb_headerRemoveEntry( header, tag ))
			throw PkgDbExcp( "headerRemoveEntry failed" );
	}
	
	void write( PFD_t fd ) const {
		PkgDb_headerWrite( fd, header, HEADER_MAGIC_YES );
	}

	// read a header from ASCII representation from a stream
	// the read() interface can also maintain a line number counter and use it
	// in error messages. For << it isn't possible to pass that additional
	// info...
	void read( istream& is, int *lineno = NULL, bool is_override = false );
	friend istream& operator>>( istream&, DbHeader& );
};

#endif  /* _PkgDb_Header_h */


// Local Variables:
// tab-width: 4
// End:
#endif
