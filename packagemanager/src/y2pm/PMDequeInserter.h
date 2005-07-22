#ifndef _PMDEQUEINSERTER_H
#define _PMDEQUEINSERTER_H

#include <y2pm/PMInserter.h>
#include <deque>

template<typename T>
class PMDequeInserter : public PMInserter<T>
{
    private:
	std::deque<T>& _deque;

    public:
	PMDequeInserter(std::deque<T>& deque) : PMInserter<T>(), _deque(deque)
	{
	}

	virtual ~PMDequeInserter()
	{
	}

	virtual void operator+=(T& elem)
	{
	    _deque.push_back(elem);
	}

	virtual bool empty()
	{
	    return _deque.empty();
	}
};

#endif
