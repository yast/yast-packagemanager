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

using namespace std;

InstallOrder::InstallOrder(const PkgSet& toinstall) : _toinstall(toinstall), _dirty(true)
{
}

// yea, that stuff is suboptimal. there should be a heap sorted by order
InstallOrder::SolvableList InstallOrder::computeNextSet()
{
    SolvableList newlist;

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

    D__ << "visiting " << node->name() << endl;

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
	D__ << "check requirement " << *requirement << endl;
	RevRel_for(_toinstall.provided()[requirement->name()], prov)
	{
	    if(requirement->matches(prov->relation()))
	    {
		D__ << "provided by " << prov->pkg()->name() << endl;
		if(_nodes[prov->pkg()].visited == false)
		{
		    info.order++;
		    _rgraph[prov->pkg()].push_back(node);
		    _graph[node].push_back(prov->pkg());
		    rdfsvisit(prov->pkg());
		}
		else if(_nodes[prov->pkg()].endtime == 0)
		{
		    if(prov->pkg() != node)
		    {
			WAR  << "backward edge " << node->name() << " -> " << prov->pkg()->name() << endl;
		    }
		}
		else
		{
		    // filter multiple depends on same node (cosmetic)
		    SolvableList& lrg = _rgraph[prov->pkg()];
		    if(find(lrg.begin(),lrg.end(),node) == lrg.end())
		    {
			info.order++;
			lrg.push_back(node);

			SolvableList& lg = _graph[prov->pkg()];
			if(find(lg.begin(),lg.end(),node) == lg.end())
			    lg.push_back(node);
		    }

		}
	    }
	}
    }
    _topsorted.push_back(node);
    _nodes[node].endtime = _rdfstime;
    _rdfstime++;
}

void InstallOrder::startrdfs()
{
    _nodes.erase(_nodes.begin(),_nodes.end());
    _rgraph.erase(_rgraph.begin(),_rgraph.end());
    _graph.erase(_graph.begin(),_graph.end());

    _rdfstime = 1;
    
    _topsorted.erase(_topsorted.begin(),_topsorted.end());

    // it->key is PkgName
    // it->value is PMSolvablePtr

    // initialize all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	_nodes[it->value]=NodeInfo(it->value);
	_rgraph[it->value]=SolvableList();
	_graph[it->value]=SolvableList();
    }

    // visit all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	if(_nodes[it->value].visited == false)
	{
	    WAR << "start recursion on " << it->value->name() << endl;
	    rdfsvisit(it->value);
	}
    }
}

const InstallOrder::SolvableList& InstallOrder::getTopSorted() const
{
    return _topsorted;
}

const void InstallOrder::printAdj(std::ostream& os, bool reversed) const
{
    const Graph& g = (reversed?_rgraph:_graph);
    for (Graph::const_iterator gcit = g.begin();
	gcit != g.end(); ++gcit)
    {
	Nodes::const_iterator niit = _nodes.find(gcit->first);
	int order = niit->second.order;
	os << gcit->first->name() << "(" << order << "): ";
	for (SolvableList::const_iterator scit = gcit->second.begin();
	    scit != gcit->second.end(); ++scit)
	{
	    os << (*scit)->name() << " ";
	}
	os << endl;
    }
}
