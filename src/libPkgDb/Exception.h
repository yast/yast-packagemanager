#ifndef _PkgDb_Exception_h
#define _PkgDb_Exception_h

#include <iostream>
#include <string>
#include <cerrno>

using namespace std;

struct PkgDbExcp {
  protected:
	string name;
  public:
	PkgDbExcp( string str ) : name(str) {
#ifndef NDEBUG
		cerr << "PkgDbExcp(" << str << ")\n";
#endif
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
/*
struct PkgDbNoTagExcp : public PkgDbExcp {
  protected:
	int tagno;
  public:
	PkgDbNoTagExcp() : PkgDbExcp("tag not found") {}
	PkgDbNoTagExcp(int tag) : PkgDbExcp("tag not found"), tagno(tag) {}
	void print( ostream& os ) const;
};
*/
struct PkgDbReadTagExcp : public PkgDbExcp {
  protected:
	int line;
  public:
	PkgDbReadTagExcp(string str, int l = 0) : PkgDbExcp(str), line(l) {}
	void print( ostream& os ) const;
};

#endif  /* _PkgDb_Exception_h */


// Local Variables:
// tab-width: 4
// End:
