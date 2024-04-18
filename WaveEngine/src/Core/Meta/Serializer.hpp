#ifndef Serializer_hpp
#define Serializer_hpp

#include "MetaType.hpp"
#include "Core/Math.hpp"
#include <iostream>

namespace MetaType {

class Serializer {
public:
	template<class T>
	Serializer(T* obj);

	template<class T>
	static T read(const std::string& str);
	template<class T>
	static std::string write(const T& obj);
};

template<class T>
inline Serializer::Serializer(T* obj)
{
	MetaObject meta_obj = MetaObject::MetaObjectOf(obj);
	std::cout << "\n==================================\n";
	std::cout << "{\n";
	std::cout << "    \"className\":\"" << meta_obj.className() << "\",\n";

	std::cout << "    \"fields\":{\n";
	for (int i = 0; i < meta_obj.fieldCount(); i++) {
		auto& field = meta_obj.field(i);
		std::string_view type_name = field.typeName();
		std::string_view field_name = field.name();
		Vec3 field_value = *(Vec3*)(field.get((void*)obj));
		std::cout << "        \"" << field_name << "\":";
		std::cout << "\"" << field_value.x << " " << field_value.y << " " << field_value.z << "\"";
		std::cout << ",\n";
	}
	std::cout << "    },\n";

	std::cout << "},\n";
	std::cout << "==================================\n\n";
}

}

#endif
