#ifndef _KJSON_KJSON_H  
#define _KJSON_KJSON_H  

#include "jsonvalue.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>


class KJson
{
public:

    // 默认构造函数
    KJson();

    // 带 JsonValue 对象的构造函数
    explicit KJson(const JsonValue& rootValue);

    // 带 JsonValue 的移动构造函数
    explicit KJson(JsonValue&& rootValue);

    // 默认拷贝构造函数、移动构造函数、赋值运算符和析构函数
    KJson(const KJson& other) = default;
    KJson(KJson&& other) noexcept = default;
    KJson& operator=(const KJson& other) = default;
    KJson& operator=(KJson&& other) noexcept = default;
    ~KJson() = default;

    // 从文件中解析JSON
    bool parseFromFile(const std::string& filePath);

    // 将JSON序列化并写入文件
    bool serializeToFile(const std::string& filePath) const;

    // 将JSON转成xml序列化并写入文件
    bool serializeXMLToFile(const std::string& filePath) const;

    // 获取值（支持嵌套键名）
    std::shared_ptr<JsonValue> get(const std::vector<std::string>& keys);

    // 删除值（支持嵌套键名）  方便改动keys不使用const &
    void deleteKeyValue(std::vector<std::string> keys);

    // 重载 operator[]，返回 JsonValue 的引用(支持嵌套键名)
    JsonValue& operator[](std::vector<std::string> keys);

    // JSON对象输出
    void printJson();//不加const 为了在读时候上锁

    // 打印整个xml结构
    void printXml();//不加const 为了在读时候上锁

    //查询操作
    void queryJson();

    //json的动态管理
    void manageJsonData();

private:
    std::shared_ptr<JsonValue> m_root;  // 存储整个JSON对象
    std::shared_ptr<std::mutex> m_mtx;  // 用于多线程安全

    // 解析JSON内部递归函数
    JsonValue parseJson(std::istringstream& ss);

    // 工具函数：跳过空格
    void skipWhitespace(std::istringstream& ss);

    // 工具函数：解析字符串
    std::string parseString(std::istringstream& ss);

    // 工具函数：解析数字
    std::string parseData(std::istringstream& ss);

    // 工具函数：解析数组
    std::vector<JsonValue> parseArray(std::istringstream& ss);

    // 工具函数：解析对象
    std::map<std::string, JsonValue> parseObject(std::istringstream& ss);

    // 工具函数：判断是否为数字、NULL、BOOL
    bool isData(std::string& value);

    bool isNull(std::string& value);

    bool isBool(std::string& value);

    //工具函数（用于更新manageJsonData（）动态管理的合法检查表）
    bool updateLastKey(JsonValue& current, std::vector<std::string>& lastKeys);

};

#endif //_KJSON_KJSON_H
