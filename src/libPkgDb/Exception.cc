#include <iostream>
#include <cstring>
#include <Exception.h>
#include <DbHeader.h>

using namespace std;

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
/*
void PkgDbNoTagExcp::print( ostream& os ) const
{
	if (tagno)
		os << "tag " << Tag::tagname(tagno) << " not found";
	else
		os << name;
}
*/
void PkgDbReadTagExcp::print( ostream& os ) const
{
	os << name;
	if (line)
		os << " (line " << line << ")";
}


// Local Variables:
// tab-width: 4
// End:
