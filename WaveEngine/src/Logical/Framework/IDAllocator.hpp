#ifndef IDAllocator_hpp
#define IDAllocator_hpp

template<typename T>
class IDAllocator {
public:
	IDAllocator() : id(++global_id) {}
	IDAllocator(int id) : id(id) {} // TODO remove
	bool operator==(const IDAllocator& rhs) const {
		return id == rhs.id;
	}
	operator int() const { return id; }
	int id;

protected:
	static inline int global_id = 0;
};

#endif