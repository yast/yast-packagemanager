#include <y2pm/PkgName.h>

using namespace std;

///////////////////////////////////////////////////////////////////

UstringHash PkgName::_nameHash;

///////////////////////////////////////////////////////////////////

ostream& operator<<( ostream& os, const PkgNameEd& ne )
{
	os << ne.name << '-' << ne.edition.as_string();
	return os;
}


// Local Variables:
// tab-width: 4
// End:
