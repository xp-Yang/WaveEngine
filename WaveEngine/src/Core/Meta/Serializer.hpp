#ifndef Serializer_hpp
#define Serializer_hpp

#include "Meta.hpp"
#include "Core/Math.hpp"
#include "Core/Utils.hpp"
#include <iostream>

namespace Meta {

class Serializer {
public:
	template<class T>
	Serializer(T* obj) { 
		ReflectionInstance refl_obj = obj;
		test_output(refl_obj);
	}

	template<class T>
	static T read(const std::string& str);

	template<class T>
	static std::string write(const T& obj);

	template<class T>
	void test_output(ReflectionInstance<T>& refl_obj);
};

template<class T>
inline void Serializer::test_output(ReflectionInstance<T>& refl_obj)
{
	static std::string tab_str = "    ";
	static std::string crlf_str = "\n";
	static std::string open_brace_str = "{\n";
	static std::string close_brace_str = "},\n";
	static std::string colon = " : ";

	std::cout << "\n====================================================================\n";
	std::cout << open_brace_str;

		std::cout << tab_str << "\"className\"" << colon << "\"" << refl_obj.className() << "\"" << "," << crlf_str;

		std::cout << tab_str << "\"fields\"" << colon << open_brace_str;
			for (int i = 0; i < refl_obj.fieldCount(); i++) {
				auto& field = refl_obj.field(i);
				std::string_view field_type_name = field.field_type_name;
				std::string_view field_name = field.field_name;
				std::cout << tab_str << tab_str << "\"" << "<" << field_type_name << ">" << " " << field_name << "\"" << colon << " ";
				if (refl_obj.getFieldValue<Vec3>(i)) {
					Vec3 field_value_vec3 = *refl_obj.getFieldValue<Vec3>(i);
					std::cout << "\"" << field_value_vec3.x << " " << field_value_vec3.y << " " << field_value_vec3.z << "\"";
				}
				if (refl_obj.getFieldValue<std::string>(i)) {
					std::string field_value_str = *refl_obj.getFieldValue<std::string>(i);
					std::cout << "\"" << field_value_str << "\"";
				}
				std::cout << "," << crlf_str;
			}
		std::cout << tab_str << close_brace_str;

		std::cout << tab_str << "\"methods\"" << colon << open_brace_str;
			for (int i = 0; i < refl_obj.methodCount(); i++) {
				auto& method = refl_obj.method(i);
				std::cout << tab_str << tab_str << "\"" << method.signature << "\"" << "," << crlf_str;
			}
		std::cout << tab_str << close_brace_str;

		if (refl_obj.method("transform").signature != "") {
			std::cout << tab_str << "\"invokeMethods\"" << colon << refl_obj.method("transform").signature << " = ";
			Mat4 transform_mat = refl_obj.invokeMethod<Mat4>("transform");
			std::cout << crlf_str << Utils::mat4ToStr(transform_mat, 2);
		}

	std::cout << close_brace_str;
	std::cout << "====================================================================\n\n";
}

}

#endif
