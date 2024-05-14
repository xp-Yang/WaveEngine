#ifndef Serializer_hpp
#define Serializer_hpp

#include "Meta.hpp"
#include <json11/json11.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace Meta {
namespace Serialization {

class Serializer {
public:
	template<class T>
	static T& read(const Json& json, T* obj)
	{
		DynamicReflectionInstance refl_obj = obj;
		read_internal(json, refl_obj);
		return *static_cast<T*>(refl_obj.getInstance());
	}

	template<class T>
	static Json write(const T* obj) {
		DynamicReflectionInstance refl_obj = obj;
		Json ret = write_internal(refl_obj);
		return ret;
	}

	template<class T>
	static void loadFromJsonFile(const std::string& filepath, T* obj) {
		std::ifstream fin(filepath);
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
	static void saveToJsonFile(const std::string& filepath, const T& obj) {
		std::ofstream fout(filepath);
		if (!fout) {
			assert(false);
			return;
		}
		Json j = write(obj);
		fout << j.dump();
		fout.flush();
	}

	//template<class T>
	//static void output_test(const T& obj) {
	//	ReflectionInstance test_refl_obj = ReflectionInstance(obj);
	//	test_output(test_refl_obj);
	//}

protected:
	static void read_internal(const Json& json, DynamicReflectionInstance& refl_obj) {
		for (int i = 0; i < refl_obj.fieldCount(); i++) {
			auto& field = refl_obj.field(i);
			std::string field_type_name = field.field_type_name;
			std::string field_name = field.field_name;
			void* filed_value_ptr = (refl_obj.getFieldValue(i));
			if (field_type_name == traits::className<char>()) {
				assert(json.is_number());
				*static_cast<char*>(filed_value_ptr) = json.number_value();
			}
			else if (field_type_name == traits::className<int>()) {
				assert(json.is_number());
				*static_cast<int*>(filed_value_ptr) = static_cast<int>(json.number_value());
			}
			else if (field_type_name == traits::className<unsigned int>()) {
				assert(json.is_number());
				*static_cast<unsigned int*>(filed_value_ptr) = static_cast<unsigned int>(json.number_value());
			}
			else if (field_type_name == traits::className<float>()) {
				assert(json.is_number());
				*static_cast<float*>(filed_value_ptr) = static_cast<float>(json.number_value());
			}
			else if (field_type_name == traits::className<double>()) {
				assert(json.is_number());
				*static_cast<double*>(filed_value_ptr) = static_cast<double>(json.number_value());
			}
			else if (field_type_name == traits::className<bool>()) {
				assert(json.is_bool());
				*static_cast<bool*>(filed_value_ptr) = json.bool_value();
			}
			else if (field_type_name == traits::className<std::string>()) {
				assert(json.is_string());
				*static_cast<std::string*>(filed_value_ptr) = json.string_value();
			}
			else {
				DynamicReflectionInstance field_refl_obj = DynamicReflectionInstance(field_type_name, filed_value_ptr);
				read_internal(json[field_name], field_refl_obj);
			}
		}
	}

	static Json write_internal(const DynamicReflectionInstance& refl_obj) {
		Json::object json_obj;
		for (int i = 0; i < refl_obj.fieldCount(); i++) {
			auto& field = refl_obj.field(i);
			std::string field_type_name = field.field_type_name;
			std::string field_name = field.field_name;
			void* filed_value_ptr = (refl_obj.getFieldValue(i));
			if (field_type_name == traits::className<char>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<char*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<int>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<int*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<unsigned int>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<int*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<float>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<float*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<double>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<double*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<bool>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<bool*>(filed_value_ptr)));
			}
			else if (field_type_name == traits::className<std::string>()) {
				json_obj.insert_or_assign(field_name, Json(*static_cast<std::string*>(filed_value_ptr)));
			}
			else {
				DynamicReflectionInstance field_refl_obj = DynamicReflectionInstance(field_type_name, filed_value_ptr);
				json_obj.insert_or_assign(field_name, write_internal(field_refl_obj));
			}
		}
		return Json(json_obj);
	}

private:
	Serializer() = default;
};

//template<class T>
//inline void test_output(ReflectionInstance<T>& refl_obj)
//{
//	static std::string tab_str = "    ";
//	static std::string crlf_str = "\n";
//	static std::string open_brace_str = "{\n";
//	static std::string close_brace_str = "},\n";
//	static std::string colon = " : ";
//
//	std::cout << "\n====================================================================\n";
//	std::cout << open_brace_str;
//
//		std::cout << tab_str << "\"className\"" << colon << "\"" << refl_obj.className() << "\"" << "," << crlf_str;
//
//		std::cout << tab_str << "\"fields\"" << colon << open_brace_str;
//			for (int i = 0; i < refl_obj.fieldCount(); i++) {
//				auto& field = refl_obj.field(i);
//				std::string_view field_type_name = field.field_type_name;
//				std::string_view field_name = field.field_name;
//				std::cout << tab_str << tab_str << "\"" << "<" << field_type_name << ">" << " " << field_name << "\"" << colon << " ";
//				if (refl_obj.getFieldValue<Vec3>(i)) {
//					Vec3 field_value_vec3 = *refl_obj.getFieldValue<Vec3>(i);
//					std::cout << "\"" << field_value_vec3.x << " " << field_value_vec3.y << " " << field_value_vec3.z << "\"";
//				}
//				if (refl_obj.getFieldValue<std::string>(i)) {
//					std::string field_value_str = *refl_obj.getFieldValue<std::string>(i);
//					std::cout << "\"" << field_value_str << "\"";
//				}
//				std::cout << "," << crlf_str;
//			}
//		std::cout << tab_str << close_brace_str;
//
//		std::cout << tab_str << "\"methods\"" << colon << open_brace_str;
//			for (int i = 0; i < refl_obj.methodCount(); i++) {
//				auto& method = refl_obj.method(i);
//				std::cout << tab_str << tab_str << "\"" << method.signature << "\"" << "," << crlf_str;
//			}
//		std::cout << tab_str << close_brace_str;
//
//		if (refl_obj.method("transform").signature != "") {
//			std::cout << tab_str << "\"invokeMethods\"" << colon << refl_obj.method("transform").signature << " = ";
//			Mat4 transform_mat = refl_obj.invokeMethod<Mat4>("transform");
//			std::cout << crlf_str << Utils::mat4ToStr(transform_mat, 2);
//		}
//
//	std::cout << close_brace_str;
//	std::cout << "====================================================================\n\n";
//}

}}

#endif
