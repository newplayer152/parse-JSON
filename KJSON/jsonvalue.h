#ifndef _KJSON_JSONVALUE_H  
#define _KJSON_JSONVALUE_H  

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>

class JsonValue
{
public:

    // 默认构造函数
    JsonValue();

    // 带字符串值的构造函数
    explicit JsonValue(const std::string& stringValue);

    // 带标识参数的构造函数，支持数字、空值和布尔类型
    JsonValue(const std::string& stringValue, bool isData, bool isNull, bool isBool);

    // 带对象类型的构造函数
    explicit JsonValue(const std::map<std::string, JsonValue>& objectValue);

    // 带数组类型的构造函数
    explicit JsonValue(const std::vector<JsonValue>& arrayValue);

    // 拷贝构造函数
    JsonValue(const JsonValue& other) = default;

    // 移动构造函数
    JsonValue(JsonValue&& other) noexcept = default;

    // 拷贝赋值运算符
    JsonValue& operator=(const JsonValue& other) = default;

    // 移动赋值运算符
    JsonValue& operator=(JsonValue&& other) noexcept = default;

    // 析构函数
    ~JsonValue() = default;

    // 判断当前值是否为对象
    bool isObject() const;

    // 判断当前值是否为数组
    bool isArray() const;

    //获取m_stringValue成员变量引用
    std::string& getStringValue();

    //获取m_objectValue成员变量引用
    std::map<std::string, JsonValue>& getObjectValue();

    //获取m_arrayValue成员变量引用
    std::vector<JsonValue>& getArrayValue();

    // 递归打印JSON值
    void print(int indent = 0) const;

    //打印当前类的所有键
    void printKeys(std::vector<std::string>& lastKeys);

    //递归生成序列化
    void serializeToStream(std::ofstream& outfile, int indent = 0) const;

    //递归输出xml文件
    void printXML(const std::string& outLayLabel, int indent = 0) const;

    //递归生成xml文件
    void serializeToXMLStream(std::ofstream& outfile, const std::string& outLayLabel, int indent = 0) const;


private:

    bool m_isString = true;  // 默认是字符串类型用于存储所有非列表与json对象的值
    bool m_isData = false;
    bool m_isNullValue = false;
    bool m_isBoolValue = false;

    std::shared_ptr<std::string>  m_stringValue;// 字符串值
    std::shared_ptr<std::map<std::string, JsonValue>> m_objectValue; // 对象类型的值
    std::shared_ptr<std::vector<JsonValue>> m_arrayValue;  // 数组类型的值

};

#endif // _KJSON_JSONVALUE_H  
