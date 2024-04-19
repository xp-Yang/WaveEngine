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
	ReflectionInstance refl_obj = ReflectionInstance(obj);
	MetaObject meta_obj_ = MetaObject::MetaObjectOf<T>();
	MetaObject meta_obj = refl_obj.getMetaObject();
	std::cout << "\n==================================\n";
	std::cout << "{\n";
	std::cout << "    \"className\":\"" << meta_obj.className() << "\",\n";

	std::cout << "    \"fields\":{\n";
	for (int i = 0; i < meta_obj.fieldCount(); i++) {
		auto& field = meta_obj.field(i);
		std::string_view field_type_name = field.field_type_name;
		std::string_view field_name = field.field_name;
		void* field_value = refl_obj.getField(i);
		std::cout << "        \"" << field_name << "\":" << "<" << field_type_name << ">";
			//Vec3 field_value_vec3 = *(Vec3*)field_value;
			//std::cout << "\"" << field_value_vec3.x << " " << field_value_vec3.y << " " << field_value_vec3.z << "\"";

			//std::string field_value_str = *(std::string*)field_value;
			//std::cout << "\"" << field_value_str << "\"";
		std::cout << ",\n";
	}
	std::cout << "    },\n";

	std::cout << "},\n";
	std::cout << "==================================\n\n";
}

}

#endif
