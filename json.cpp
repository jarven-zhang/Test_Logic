#include "json.h"
#include <stdlib.h>
#include <stdio.h>

pJson::pJson()
{
	json_root_ =  cJSON_CreateObject();
}

pJson::pJson(const pJson & j)
{
	json_root_ = NULL;
	get_from_json(j);
}

pJson & pJson::operator=(pJson & j)
{
	get_from_json(j);
	return *this;
}
pJson::~pJson()
{
	clear();
}

cJSON * pJson::get_json_root()
{
	return json_root_;
}

int pJson::clear()
{
	if (json_root_ != NULL) {
		cJSON_Delete(json_root_);
		json_root_ = NULL;
	}
	return 0;
}

int pJson::ParseJson(const char* str)
{
	clear();
	json_root_ = cJSON_Parse(str);
	if (NULL == json_root_)
		return -1;
	return 0;
}

int pJson::ParseJson(const std::string & json)
{
	clear();
	json_root_ = cJSON_Parse(json.c_str());
	if (NULL == json_root_)
		return -1;
	return 0;
}

const char* pJson::c_str()const
{
	if (NULL == json_root_)
		return NULL;

	return cJSON_Print(json_root_);;
}

std::vector<std::string> pJson::split(const std::string & s, const std::string & sub_split)
{
	size_t pos = 0;
	std::vector<std::string> ret;
	std::string str = s + sub_split;
	while (true)
	{
		//auto split_pos = str.find(sub_split, pos);
		int split_pos = str.find(sub_split, pos);
		if (std::string::npos == split_pos) {
			break;
		}
		ret.push_back(str.substr(pos, split_pos - pos));
		pos = split_pos + sub_split.size();
	}
	return ret;
}

int pJson::get_from_json(const pJson & j)
{
	ParseJson(j.c_str());
	return 0;
}

cJSON* pJson::EachJson(cJSON *obj, const std::string key, int type)
{
	cJSON *obj_Json = NULL;
	while (obj) {
		//printf("obj->type:%ld\n", obj->type);
		//printf("type:%ld\n", type);
        if (type == obj->type){
            if (NULL != obj->string) {
				if (0 == strcmp(key.c_str(), obj->string)) {
					obj_Json = obj;
					return obj_Json;
				}
            }
		}
		if (obj->child) {
			obj_Json = EachJson(obj->child,key,type);
			if (obj_Json && type == obj_Json->type)
			{
				return obj_Json;
			}
		}

		obj = obj->next;
	}
	return obj_Json;
}

cJSON* pJson::get_object(cJSON* obj, const std::string key)
{
	if (NULL == json_root_) { return NULL; }
	if (NULL == obj) { obj = json_root_; }
	return EachJson(obj,key,cJSON_Object);
}

cJSON* pJson::get_array(cJSON* obj, const std::string key)
{
	if (NULL == json_root_) { return NULL; }
	if (NULL == obj) { obj = json_root_; }
	return EachJson(obj, key,cJSON_Array);
}

cJSON* pJson::get_child(cJSON* obj)
{
	if (NULL == json_root_) { return NULL; }
	if (NULL == obj) { obj = json_root_; }
	return obj->child;
}

int pJson::get_value(cJSON * obj, const std::string key, long & value)
{
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	obj = EachJson(obj, key, cJSON_Number);
	if (NULL == obj) {
		return -1;
	}
	value = obj->valueint;
	return 0;
}

int pJson::get_value(cJSON * obj, const std::string key, unsigned int & value)
{
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	obj = EachJson(obj, key, cJSON_Number);
	if (NULL == obj) {
		return -1;
	}
	value = obj->valueint;
	return 0;
}

int pJson::get_value(cJSON* obj, const std::string key, int&value)
{
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	//printf("cJSON_Print(json_root_):%s\n", cJSON_Print(json_root_));
	//printf("json_root_->type:%d\n", json_root_->type);
	obj = EachJson(obj, key,cJSON_Number);
	if (NULL == obj) {
		return -1;
	}
	value = obj->valueint;
	return 0;
}

int pJson::get_value(cJSON* obj, const std::string key, double&value)
{
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_;}
	obj = EachJson(obj, key, cJSON_Number);
	if (NULL == obj) {
		return -1;
	}
	value = obj->valuedouble;
	return 0;
}

int pJson::get_value(cJSON* obj, const std::string key, std::string&value, int len)
{
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	obj = EachJson(obj, key, cJSON_String);
	if (NULL == obj) {
		return -1;
	}
	value = obj->valuestring;
	if(value.length() > len) { return -1; }
	return 0;
}

int pJson::get_value(cJSON * obj, const std::string key, unsigned long long & value)
{
	std::string v;
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	obj = EachJson(obj, key, cJSON_String);
	if (NULL == obj) {
		return -1;
	}
	v = obj->valuestring;
	if (v.length() > 18) { return -1; }
	value = atoll(v.c_str());
	return 0;
}

int pJson::get_value(cJSON * obj, const std::string key, long long & value)
{
	std::string v;
	if (NULL == json_root_) { return -1; }
	if (NULL == obj) { obj = json_root_; }
	obj = EachJson(obj, key, cJSON_String);
	if (NULL == obj) {
		return -1;
	}
	v = obj->valuestring;
	if (v.length() > 18) { return -1; }
	value = atoll(v.c_str());
	return 0;
}

cJSON* pJson::create_array(cJSON* obj,const std::string key)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON* Jarray = cJSON_CreateArray();
	set_object(obj, key.c_str(), Jarray);
	return Jarray;
}

cJSON* pJson::create_array(cJSON* obj, const std::string key, cJSON* array, bool Delete)
{
	if (NULL == obj) { obj = json_root_; }
	set_object(obj, key.c_str(), array, Delete);
	return array;
}
cJSON* pJson::create_object()
{
	return  cJSON_CreateObject();
}

int pJson::set_array(cJSON* obj, cJSON* item, bool Delete)
{
	cJSON * v = item;
	if (Delete == false) {
		std::string j = cJSON_Print(v);
		v = cJSON_Parse(j.c_str());
	}
	cJSON_AddItemToArray(obj, v);
	return 0;
}

int pJson::set_object(cJSON* obj, const std::string key, cJSON* item, bool Delete)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON * v = item;
	if (Delete == false) {
		std::string j = cJSON_Print(v);
		v = cJSON_Parse(j.c_str());
	}
	cJSON_AddItemToObject(obj, key.c_str(), v);
	return 0;
}

int pJson::set_object(cJSON * obj, const std::string key, std::string value)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON * v = cJSON_Parse(value.c_str());
	cJSON_AddItemToObject(obj, key.c_str(), v);
	return 0;
}

int pJson::set_value(cJSON * obj, const std::string key, long value)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateNumber(value));
	return 0;
}

int pJson::set_value(cJSON * obj, const std::string key, unsigned int value)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateNumber(value));
	return 0;
}

int pJson::set_value(cJSON* obj, const std::string key, int value)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateNumber(value));
	return 0;
}

int pJson::set_value(cJSON* obj, const std::string key, double value)
{
	if (NULL == obj) { obj = json_root_; }
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateNumber(value));
	return 0;
}

int pJson::set_value(cJSON* obj, const std::string key, std::string value)
{
	if (NULL == obj) {obj = json_root_;}
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateString(value.c_str()));
	return 0;
}

int pJson::set_value(cJSON * obj, const std::string key, unsigned long long value)
{
	if (NULL == obj) { obj = json_root_; }
	char c[64];
	sprintf(c, "%llu", value);
	//cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateString(std::to_string(value).c_str()));
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateString(c));
	return 0;
}

int pJson::set_value(cJSON * obj, const std::string key, long long value)
{
	if (NULL == obj) { obj = json_root_; }
	char c[64];
	sprintf(c, "%llu", value);
	//cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateString(std::to_string(value).c_str()));
	cJSON_AddItemToObject(obj, key.c_str(), cJSON_CreateString(c));
	return 0;
}
