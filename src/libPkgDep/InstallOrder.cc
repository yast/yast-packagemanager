/*---------------------------------------------------------------------\
|                                                                      |
|                      __   __    ____ _____ ____                      |
|                      \ \ / /_ _/ ___|_   _|___ \                     |
|                       \ V / _` \___ \ | |   __) |                    |
|                        | | (_| |___) || |  / __/                     |
|                        |_|\__,_|____/ |_| |_____|                    |
|                                                                      |
|                            Package Management                        |
|                                                     (C) 2002 SuSE AG |
\----------------------------------------------------------------------/

   File:       InstallOrder.cc
   Purpose:    Determine order for installing packages
   Author:     Ludwig Nussel <lnussel@suse.de>
   Maintainer: Ludwig Nussel <lnussel@suse.de>

/-*/

#include <list>

#include <y2util/Y2SLog.h>

#include <y2pm/InstallOrder.h>
#include <y2pm/PkgDep_int.h>
#include <y2pm/PMPackage.h>

using namespace std;

InstallOrder::InstallOrder(const PkgSet& toinstall, const PkgSet& installed) :
	_toinstall(toinstall),
	_installed(installed),
	_dirty(true),
	_numrun(0)
{
}

// yea, that stuff is suboptimal. there should be a heap sorted by order
InstallOrder::SolvableList InstallOrder::computeNextSet()
{
    SolvableList newlist;

    if(_dirty) startrdfs();

    for(Nodes::iterator it = _nodes.begin();
	it != _nodes.end(); ++it)
    {
	if(it->second.order == 0)
	{
	    D__ << "found " << it->second.solvable->name() <<  endl;

	    newlist.push_back(it->second.solvable);
	}
    }

    return newlist;
}

// decrease order of every adjacent node
void InstallOrder::setInstalled( constPMSolvablePtr ptr )
{
    _dirty = true;

    SolvableList& adj = _rgraph[ptr];

    D__ << ptr->name() << endl;
    
    // order will be < 0
    _nodes[ptr].order--;
    _installed.add(PMSolvablePtr::cast_away_const(ptr));
    _toinstall.remove(ptr->name());

    for(SolvableList::iterator it = adj.begin();
	it != adj.end(); ++it)
    {
	NodeInfo& info = _nodes[*it];
	info.order--;
	if(info.order < 0)
	{
	    ERR << "order of node " << (*it)->name() << " is < 0" << endl;
	}
    }
}

void InstallOrder::setInstalled( const InstallOrder::SolvableList& list )
{
    for(SolvableList::const_iterator it=list.begin();
	it != list.end(); ++it)
    {
	setInstalled(*it);
    }
}

void InstallOrder::rdfsvisit(constPMSolvablePtr node)
{
    PMSolvable::PkgRelList_type reqnprereq;

    M__ << "visiting " << node->name() << endl;

    NodeInfo& info = _nodes[node];
//    SolvableList& reverseedges = _rgraph[node];
    
    info.visited = true;
    info.begintime = _rdfstime;
    _rdfstime++;

    // put prerequires in front and requires on back of list to ensure
    // that prerequires are processed first
    for(PMSolvable::PkgRelList_const_iterator it = node->requires().begin();
	it != node->requires().end();
	++it)
    {
	if(it->isPreReq())
	    reqnprereq.push_front(*it);
	else
	    reqnprereq.push_back(*it);
    }

    for(PMSolvable::PkgRelList_const_iterator requirement = reqnprereq.begin();
	requirement != reqnprereq.end(); ++requirement)
    {
	D__ << "check requirement " << *requirement << " of " << node->name() << endl;
	SolvableList tovisit;

	// package could provide its own requirement
	if(node->doesProvide(*requirement))
	{
		D__ << "self-provides " << endl;
//		tovisit.push_back(node);
	}
	else
	{
	    bool foundinstalled = false;
	    RevRel_for(PkgSet::getRevRelforPkg(_installed.provided(),requirement->name()), iprov)
	    {
		if(requirement->matches(iprov->relation()))
		{
		    M__ << "provided by installed " << iprov->pkg()->name() << endl;
		    foundinstalled = true;
		}
	    }
	    if(!foundinstalled)
	    {
		RevRel_for(PkgSet::getRevRelforPkg(_toinstall.provided(),requirement->name()), prov)
		{
		    if(requirement->matches(prov->relation()))
		    {
			M__ << "provided by " << prov->pkg()->name() << endl;
			tovisit.push_back(prov->pkg());
		    }
		}
	    }
	}

	for( SolvableList::iterator it = tovisit.begin();
		it != tovisit.end(); ++it)
	{
	    if(_nodes[*it].visited == false)
	    {
		info.order++;
		_rgraph[*it].push_back(node);
		_graph[node].push_back(*it);
		rdfsvisit(*it);
	    }
	    else if(_nodes[*it].endtime == 0)
	    {
		if(*it != node)
		{
		    WAR  << "backward edge " << node->name() << " -> " << (*it)->name() << endl;
		}
	    }
	    else
	    {
		// filter multiple depends on same node (cosmetic)
		SolvableList& lrg = _rgraph[*it];
		if(find(lrg.begin(),lrg.end(),node) == lrg.end())
		{
		    info.order++;
		    lrg.push_back(node);

		    SolvableList& lg = _graph[node];
		    if(find(lg.begin(),lg.end(),*it) == lg.end())
			lg.push_back(*it);
		}

	    }
	}
    }
    _topsorted.push_back(node);
    _nodes[node].endtime = _rdfstime;
    _rdfstime++;
    
    M__ << node->name() << " done" << endl;
}

void InstallOrder::startrdfs()
{
    _nodes.erase(_nodes.begin(),_nodes.end());
    _rgraph.erase(_rgraph.begin(),_rgraph.end());
    _graph.erase(_graph.begin(),_graph.end());

    _rdfstime = 1;
    
    _topsorted.erase(_topsorted.begin(),_topsorted.end());

    _numrun++;
    MIL << "run " << _numrun << endl;

    // it->first is PkgName
    // it->second is PMSolvablePtr

    // initialize all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	_nodes[it->second]=NodeInfo(it->second);
	_rgraph[it->second]=SolvableList();
	_graph[it->second]=SolvableList();
    }

    // visit all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	if(_nodes[it->second].visited == false)
	{
	    M__ << "start recursion on " << it->second->name() << endl;
	    rdfsvisit(it->second);
	}
    }

    _dirty = false;
}

const InstallOrder::SolvableList& InstallOrder::getTopSorted() const
{
    return _topsorted;
}

static char* colors[] = { "red", "green", "blue", "yellow" };
static const short numcol = 4;

const void InstallOrder::printAdj(std::ostream& os, bool reversed) const
{
    const Graph& g = (reversed?_rgraph:_graph);
    os << "digraph pkgdeps {" << endl;
    for (Graph::const_iterator gcit = g.begin();
	gcit != g.end(); ++gcit)
    {
	Nodes::const_iterator niit = _nodes.find(gcit->first);
	int order = niit->second.order;
	PkgName name = gcit->first->name();
	os << "\"" << name << "\""
		<< "[label=\"" << name << "\\n"
		<< order << "\"";
	{
	    constPMPackagePtr p = PMSolvablePtr::cast_away_const(gcit->first);
	    if(p != NULL)
	    {
		int nr = p->medianr()-1;
		if(nr >= 0 && nr<numcol)
		{
		    os << " color=\"" << colors[nr] << "\"";
		}
	    }
	}
	os << "] " << endl;
	for (SolvableList::const_iterator scit = gcit->second.begin();
	    scit != gcit->second.end(); ++scit)
	{
	    os << "\"" << name << "\" -> \"" << (*scit)->name() << "\"" << endl;
	}
    }
    os << "}" << endl;
}
