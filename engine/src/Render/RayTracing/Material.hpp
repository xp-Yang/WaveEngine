#ifndef Material_hpp
#define Material_hpp

// 1.Produce a scattered ray (or say it absorbed the incident ray).
// 2.If scattered, say how much the ray should be attenuated.
class Material{
public:
    virtual bool scatter() const = 0;

protected:
    virtual ~Material() = default;
}

#endif // !Material_hpp
