#ifndef Demo_hpp
#define Demo_hpp

#include <string>

class Demo {
public:
	virtual void init() = 0;
public:
	static Demo* createDemo(const std::string& name);
};

#endif // !Demo_hpp
