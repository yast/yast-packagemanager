#include <PkgName.h>

UniqStr PkgName::PkgNameHash;

ostream& operator<<( ostream& os, const PkgName& n )
{
	os << (const char *)n;
	return os;
}

ostream& operator<<( ostream& os, const PkgNameEd& ne )
{
	os << ne.name << '-' << ne.edition.as_string();
	return os;
}


// Local Variables:
// tab-width: 4
// End:
