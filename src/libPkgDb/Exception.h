#ifndef _PkgDb_Exception_h
#define _PkgDb_Exception_h

#include <iostream>
#include <string>
#include <cerrno>

struct PkgDbExcp {
  protected:
	std::string name;
  public:
	PkgDbExcp( std::string str ) : name(str) {
#ifndef NDEBUG
		std::cerr << "PkgDbExcp(" << str << ")\n";
#endif
	}
	PkgDbExcp() : name("") {}
	virtual ~PkgDbExcp() {}
	const char *opname() { return name.c_str(); }
	virtual void print( std::ostream& os ) const;
	
	friend std::ostream& operator<<( std::ostream& os, const PkgDbExcp& e );
};

struct PkgDbFileExcp : public PkgDbExcp {
  protected:
	int err;
  public:
	PkgDbFileExcp(std::string str, int e = -1) : PkgDbExcp(str) {
		err = (e == -1) ? errno : e;
	}
	void print( std::ostream& os ) const;
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
	PkgDbReadTagExcp(std::string str, int l = 0) : PkgDbExcp(str), line(l) {}
	void print( std::ostream& os ) const;
};

#endif  /* _PkgDb_Exception_h */


// Local Variables:
// tab-width: 4
// End:
