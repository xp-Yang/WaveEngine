#ifndef Eye_hpp
#define Eye_hpp

// 1.Construct and dispatch rays into the world.
// 2.Use the results of these rays to construct the rendered image.
class Eye {
public:
	void generateRay();
	void rayColor();
};

#endif // !Eye_hpp

