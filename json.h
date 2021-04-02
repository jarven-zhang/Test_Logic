#ifndef SERVER_JSON_H
#define SERVER_JSON_H
extern "C"
{
//导入cJson接口
#include "cJSON.h"
};

#include <map>
#include <string>
#include <vector>
#include <string.h>

//文件读取BUFF 的大小
#define BUFF_SIZE 1024

class pJson
{
public:
	pJson();
	virtual ~pJson();
	//拷贝复制
	pJson(const pJson & j);
	pJson &operator=(pJson & j);

	cJSON *get_json_root();

	const char* c_str()const;
	int ParseJson(const char* str);
	int ParseJson(const std::string&json);

	cJSON* get_object(cJSON* obj, const std::string key);

	cJSON* get_array(cJSON* obj, const std::string key);

	cJSON* get_child(cJSON* obj);

	int get_value(cJSON* obj, const std::string key, long&value);
	int get_value(cJSON* obj, const std::string key, unsigned int&value);
	int get_value(cJSON* obj, const std::string key, int&value);
	int get_value(cJSON* obj,const std::string key,double&value);
	int get_value(cJSON* obj, const std::string key, std::string&value, int len = 512);
	int get_value(cJSON* obj, const std::string key, unsigned long long&value);
	int get_value(cJSON* obj, const std::string key, long long&value);

	int set_value(cJSON* obj, const std::string key, long value);
	int set_value(cJSON* obj, const std::string key, unsigned int value);
	int set_value(cJSON* obj, const std::string key, int value);
	int set_value(cJSON* obj,const std::string key, double value);
	int set_value(cJSON* obj, const std::string key, std::string value);
	int set_value(cJSON* obj, const std::string key, unsigned long long value);
	int set_value(cJSON* obj, const std::string key, long long value);

	cJSON* create_array(cJSON* obj, const std::string key);

	cJSON* create_array(cJSON * obj, const std::string key, cJSON * array, bool Delete = false);

	cJSON* create_object();

	int set_array(cJSON* obj, cJSON* item, bool Delete = true);

	int set_object(cJSON* obj, const std::string key, cJSON* item, bool Delete = true);
	int set_object(cJSON* obj, const std::string key, std::string value);

private:
	
	int clear();//释放json_root_

	int get_from_json(const pJson& j);//深拷贝

	cJSON* EachJson(cJSON *obj, const std::string key,int type);
	
	static std::vector<std::string> split(const std::string& s, const std::string&sub_split);//字符分割

private:
	cJSON *json_root_;
};

#endif