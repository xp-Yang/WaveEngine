#ifndef Slot_hpp
#define Slot_hpp

#include <functional>

template<typename... Args>
class Slot
{
public:
    using onFunc = std::function<void(Args...)>;

    Slot(const onFunc& func)
        : m_func(func)
    {
    }

    void exec(Args&&... args)
    {
        m_func(std::forward<Args>(args)...);
    }

private:
    onFunc m_func = nullptr;
};

#endif