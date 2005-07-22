#include <y2pm/IgnoredRelations.h>
#include <y2pm/PMSolvable.h>

using namespace std;

IgnoredRelations::Ignored::Ignored( PkgName* n, PkgRelation r) : pkg(n), rel(r)
{
}

IgnoredRelations::Ignored::Ignored(const Ignored& rhs) : pkg(rhs.pkg), rel(rhs.rel)
{
}

IgnoredRelations::Ignored& IgnoredRelations::Ignored::operator=(const Ignored& rhs)
{
    pkg = rhs.pkg;
    rel = rhs.rel;

    return *this;
}

// ====================

IgnoredRelations::IgnoredRelations()
{
}

IgnoredRelations::~IgnoredRelations()
{
}

void IgnoredRelations::add(const PkgRelation& rel, PMSolvablePtr pkg)
{
    if(!pkg)
	add(rel, NULL);
    else
    {
	PkgName n = pkg->name();
	add(rel, &n);
    }
}

void IgnoredRelations::add(const PkgRelation& rel, PkgName* n)
{
    _ignored.insert(make_pair(rel.name(),Ignored(n, rel)));
}

bool IgnoredRelations::isIgnored(const PkgRelation& rel, PkgName* n)
{
    IgnoreList::iterator it = _ignored.lower_bound(rel.name());
    IgnoreList::iterator end = _ignored.upper_bound(rel.name());

    for(; it != end; ++it)
    {
	Ignored& i = it->second;
	if(rel.matches(i.rel))
	{
	    if(!n || !i.pkg)
		return true;
	    else
		return (*n == *i.pkg);
	}
    }

    return false;
}

bool IgnoredRelations::isIgnored(const PkgRelation& rel, PMSolvablePtr p)
{
    if(!p)
	return isIgnored(rel, NULL);
    else
    {
	PkgName n = p->name();
	return isIgnored(rel, &n);
    }
}


IgnoredRequirements::IgnoredRequirements() : IgnoredRelations()
{
}

IgnoredRequirements::~IgnoredRequirements()
{
}

bool IgnoredRequirements::isIgnored(const PkgRelation& rel, PkgName* n)
{
    string s = rel.name().asString().substr(0,7);

    if(s == "rpmlib(")
	return true;

    return IgnoredRelations::isIgnored(rel, n);
}

bool IgnoredRequirements::isIgnored(const PkgRelation& rel, PMSolvablePtr p)
{
    if(!p)
	return this->isIgnored(rel, NULL);
    else
    {
	PkgName n = p->name();
	return this->isIgnored(rel, &n);
    }
}
