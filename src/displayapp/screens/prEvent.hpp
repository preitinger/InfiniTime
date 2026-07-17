#pragma once
#include "prUtils.hpp"

namespace pr
{

template <class Event>
class IEventListener : public BaseListItem
{
  public:
    virtual void onEvent(Event data) = 0;
};

template <class Event>
class HasListeners
{
  public:
    virtual void addListener(IEventListener<Event>*) = 0;
    virtual void removeListener(IEventListener<Event>*) = 0;
};

template <class Event>
class EventSource : public HasListeners<Event>
{
  public:
    using Listener = IEventListener<Event>;

  private:
    DoubleEndedList listeners;
    Listener* nextToBeCalled = nullptr;
    Listener* fresh = nullptr;

  public:
    void addListener(Listener*) override;
    void removeListener(Listener*) override;
    void clearListeners();
    void fire(Event event);
};

// template implementations

template <class Event>
void EventSource<Event>::addListener(Listener* l)
{
    if (nextToBeCalled) {
        // This function is called in an event listener.
        // This listener must not be called in the ongoing firing loop.
        if (!fresh)
            fresh = l;
    }
    listeners.insertBefore(l, nullptr);
}

template <class Event>
void EventSource<Event>::removeListener(Listener* l)
{
    if (l == nextToBeCalled) {
        nextToBeCalled = static_cast<Listener*>(l->getNext());
    }
    listeners.remove(l);
}

template <class Event>
void EventSource<Event>::clearListeners()
{
    listeners.clear();
    nextToBeCalled = nullptr;
    fresh = nullptr;
}

template <class Event>
void EventSource<Event>::fire(Event event)
{
    //
    // Es gibt kein Problem, wenn in einem event listener dieser selbst oder
    // andere entfernt oder neue hinzugefügt werden. Neu hinzugefügte werden in
    // diesem Schleifendurchlauf nicht mehr aufgerufen. Entfernte, die in diesem
    // Schleifendurchgang noch nicht dran waren, werden nicht mehr aufgerufen.
    // Wenn während einer fire-Schleife in Listener-Aufruf clearListeners()
    // aufgerufen wird, stoppt die Schleife unmittelbar nach dem aktuellen
    // Listener-Aufruf.
    auto as = [](BaseListItem* l) {
        return static_cast<Listener*>(l);
    };
    for (nextToBeCalled = as(listeners.getFirst()); nextToBeCalled;) {
        Listener* beingCalled = nextToBeCalled;
        nextToBeCalled = as(beingCalled->getNext());
        if (nextToBeCalled == fresh) {
            nextToBeCalled = nullptr;
            fresh = nullptr;
        }
        beingCalled->onEvent(event);
    }
}

} // pr
