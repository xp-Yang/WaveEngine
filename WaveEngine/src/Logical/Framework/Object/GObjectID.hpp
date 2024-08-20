#ifndef GObjectID_hpp
#define GObjectID_hpp

struct GObjectID {
	GObjectID() : id(++global_id) {}
	GObjectID(int id) : id(id) {}
	bool operator==(const GObjectID& rhs) const {
		return id == rhs.id;
	}
	operator int() const { return id; }

	int id;

	static inline int global_id = 0;
};

#endif