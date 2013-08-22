/*
 * EventQueue.h
 *
 *  Created on: Aug 16, 2013
 *      Author: andy
 */

#ifndef EVENTQUEUE_H_
#define EVENTQUEUE_H_

#include "rrOSSpecifics.h"
#include <deque>
#include <queue>
#include <set>
#include <list>

namespace rr
{
class LLVMExecutableModel;
}

namespace rrllvm {

class Event
{
public:
    Event(rr::LLVMExecutableModel&, uint id);
    Event(const Event& other);
    Event& operator=( const Event& rhs );
    ~Event();


    void assign() const;

    bool isExpired() const;

    bool isCurrent() const;

    double getPriority() const;

    bool isPersistent() const;

    bool useValuesFromTriggerTime() const;

    bool isTriggered() const;

    rr::LLVMExecutableModel& model;
    uint id;
    double delay;
    double assignTime;
    uint dataSize;
    double* data;


    friend bool operator<(const Event& a, const Event& b);

};

std::ostream& operator <<(std::ostream& os, const Event& data);


class EventQueue
{
public:

    typedef std::list<Event>::iterator iterator;

    void make_heap();

    bool eraseExpiredEvents();

    bool hasCurrentEvents();

    bool applyEvent();

    void push(const rrllvm::Event& x);

    uint size() const;

    const Event& top() const;

private:
    std::list<Event> c;

};


} /* namespace rrllvm */



#endif /* EVENTQUEUE_H_ */
