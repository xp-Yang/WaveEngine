#ifndef Component_hpp
#define Component_hpp

#include <Core/Math.hpp>
#include <Core/Meta/Meta.hpp>

#include <memory>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

class GObject;
struct Component {
    Component() = delete;
    Component(GObject* parent) : parent_object(parent) {};
    virtual ~Component() {}

    //virtual void tick(float delta_time) = 0;
    virtual std::string typeName() { return "Component"; }

public:
    std::shared_ptr<GObject> parent_object;
};

#endif // !Component_hpp
