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

InstallOrder::InstallOrder(const PkgSet& toinstall) : _toinstall(toinstall)
{
}

InstallOrder::SolvableList InstallOrder::computeNextSet()
{
    return SolvableList();
}

void InstallOrder::setInstalled( constPMSolvablePtr ptr )
{
}

void InstallOrder::rdfsvisit(constPMSolvablePtr node)
{
    PMSolvable::PkgRelList_type reqnprereq;

    DBG << "visiting " << node->name() << endl;

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
		    rdfsvisit(prov->pkg());
		}
		else if(_nodes[prov->pkg()].endtime == 0)
		{
		    if(prov->pkg() != node)
		    {
			DBG  << "backward edge " << node->name() << " -> " << prov->pkg()->name() << endl;
		    }
		}
		else
		{
		    info.order++;
		    _rgraph[prov->pkg()].push_back(node);
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

    _rdfstime = 1;
    
    _topsorted.erase(_topsorted.begin(),_topsorted.end());

    // it->key is PkgName
    // it->value is PMSolvablePtr

    // initialize all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	_nodes[it->value]=NodeInfo();
	_rgraph[it->value]=SolvableList();
    }

    // visit all nodes
    for(PkgSet::iterator it = _toinstall.begin(); it != _toinstall.end(); ++it)
    {
	if(_nodes[it->value].visited == false)
	{
	    DBG << "start recursion on " << it->value->name() << endl;
	    rdfsvisit(it->value);
	}
    }
}

const InstallOrder::SolvableList& InstallOrder::getTopSorted() const
{
    return _topsorted;
}
