#ifndef Signal_hpp
#define Signal_hpp

#include "Slot.hpp"
#include <memory>
#include <functional>

#define emit
#define slots
#define signals public

template<typename... Args>
class Signal
{
public:
    using onFunc = std::function<void(Args...)>;

    void bind(const onFunc& func)
    {
        m_slots.push_back(std::make_shared<Slot<Args...>>(func));
    }

    //void operator()(Args&&... args) // TODO FIXME
    void operator()(Args&... args)
    {
        for (auto& iter : m_slots)
        {
            iter->exec(std::forward<Args>(args)...);
        }
    }

private:
    std::vector<std::shared_ptr<Slot<Args...>>> m_slots;
};

template<class Sender, typename... Args>
void connect(Sender* sender, Signal<Args...>* signal, std::function<void(Args...)> slot)
{
    signal->bind(slot);
}

template<class Sender, class Receiver, typename... Args>
void connect(Sender* sender, Signal<Args...>* signal, Receiver* receiver, void (Receiver::* slot)(Args...))
{
    std::function<void(Args...)> func = [receiver, slot](Args... args) { (receiver->*slot)(std::forward<Args>(args)...); };
    signal->bind(func);
}

#endif