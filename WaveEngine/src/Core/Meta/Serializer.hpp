#ifndef Serializer_hpp
#define Serializer_hpp

#include "Meta.hpp"
#include <json11/json11.hpp>
#include <iostream>
#include <fstream>

namespace Meta {
namespace Serialization {

class Serializer {
public:
	template<class T>
	static T& read(const Json& json, T& obj)
	{
		ReflectionInstance refl_obj = obj;
		T* ret = refl_obj.read(json);
		return *ret;
	}

	template<class T>
	static Json write(const T& obj) {
		ReflectionInstance refl_obj = obj;
		Json ret = refl_obj.write();
		return ret;
	}

	template<class T>
	static void loadFromJsonFile(const std::string& filename, T& obj) {
		std::ifstream fin(filename);
		if (!fin) {
			assert(false);
			return;
		}
		std::stringstream buffer;
		buffer << fin.rdbuf();
		std::string json_text(buffer.str());

		std::string error;
		auto&& json = Json::parse(json_text, error);
		if (!error.empty())
		{
			assert(false);
			return;
		}
		Serializer::read(json, obj);
	}

	template<class T>
	static void saveToJsonFile(const std::string& filename, const T& obj) {
		std::ofstream fout(filename);
		if (!fout) {
			assert(false);
			return;
		}
		Json j = write(obj);
		fout << j.dump();
		fout.flush();
	}

	template<class T>
	static void output_test(const T& obj) {
		ReflectionInstance test_refl_obj = ReflectionInstance(obj);
		test_output(test_refl_obj);
	}

private:
	Serializer();
};

template<class T>
inline void test_output(ReflectionInstance<T>& refl_obj)
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
				if (refl_obj.getFieldValue<std::vector<ecs::Entity>>(i)) {
					std::vector<ecs::Entity> field_value_vector_entity = *refl_obj.getFieldValue<std::vector<ecs::Entity>>(i);
					for (auto& entity : field_value_vector_entity) {
						std::cout << "\"" << entity.getId() << "\"";
					}
				}
				if (refl_obj.getFieldValue<std::vector<ecs::ComponentPool*>>(i)) {
					std::vector<ecs::ComponentPool*> field_value_vector_component_pool = *refl_obj.getFieldValue<std::vector<ecs::ComponentPool*>>(i);
					for (auto& component_pool : field_value_vector_component_pool) {
						std::cout << "\"" << component_pool->m_data << "\"";
						std::cout << "\"" << component_pool->m_componentTypeSize << "\"";
					}
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

		//if (refl_obj.method("transform").signature != "") {
		//	std::cout << tab_str << "\"invokeMethods\"" << colon << refl_obj.method("transform").signature << " = ";
		//	Mat4 transform_mat = refl_obj.invokeMethod<Mat4>("transform");
		//	std::cout << crlf_str << Utils::mat4ToStr(transform_mat, 2);
		//}

	std::cout << close_brace_str;
	std::cout << "====================================================================\n\n";
}

}}

#include "Serializer.inl"

#endif
