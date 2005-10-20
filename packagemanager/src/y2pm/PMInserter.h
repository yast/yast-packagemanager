#ifndef _PMINSERTER_H
#define _PMINSERTER_H

/** abstract class to add elements to a container with elements T */
template<typename T>
class PMInserter
{
    public:
	PMInserter() {}

	virtual ~PMInserter() {}

	virtual void operator+=(T& elem) = 0;

	virtual bool empty() = 0;
};

template<typename Container, typename T>
class PMBackInserter : public PMInserter<T>
{
    private:
	Container& _container;

    public:
	PMBackInserter(Container& container) : PMInserter<T>(), _container(container) {}

	virtual ~PMBackInserter() {}

	virtual void operator+=(T& elem)
	{
	    _container.push_back(elem);
	}

	virtual bool empty()
	{
	    return _container.empty();
	}
};

#endif
