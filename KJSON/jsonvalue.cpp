#include "jsonvalue.h"




JsonValue::JsonValue() : m_isString(true), m_isData(false), m_isNullValue(false), m_isBoolValue(false),
m_stringValue(std::make_shared<std::string>()),
m_objectValue(std::make_shared<std::map<std::string, JsonValue>>()),
m_arrayValue(std::make_shared<std::vector<JsonValue>>()) {}

JsonValue::JsonValue(const std::string& stringValue) : m_isString(true), m_isData(false), m_isNullValue(false), m_isBoolValue(false),
m_stringValue(std::make_shared<std::string>(stringValue)),
m_objectValue(nullptr), m_arrayValue(nullptr) {}

JsonValue::JsonValue(const std::string& stringValue, bool isData, bool isNull, bool isBool)
    : m_isString(true), m_isData(isData), m_isNullValue(isNull), m_isBoolValue(isBool),
    m_stringValue(std::make_shared<std::string>(stringValue)),
    m_objectValue(nullptr), m_arrayValue(nullptr) {
    if (!isData && !isNull && !isBool) {
        throw std::runtime_error("Invalid type specified.");
    }
}

JsonValue::JsonValue(const std::map<std::string, JsonValue>& objectValue)
    : m_isString(false), m_isData(false), m_isNullValue(false), m_isBoolValue(false),
    m_stringValue(nullptr),
    m_objectValue(std::make_shared<std::map<std::string, JsonValue>>(objectValue)),
    m_arrayValue(nullptr) {}

JsonValue::JsonValue(const std::vector<JsonValue>& arrayValue) : m_isString(false), m_isData(false), m_isNullValue(false), m_isBoolValue(false),
m_stringValue(nullptr),
m_objectValue(nullptr),
m_arrayValue(std::make_shared<std::vector<JsonValue>>(arrayValue)) {}

// 判断当前值是否为对象
bool JsonValue::isObject() const {
    if (m_objectValue != nullptr) {
        return !m_objectValue->empty();
    }
    return false;
}

// 判断当前值是否为数组
bool JsonValue::isArray() const {
    if (m_arrayValue != nullptr) {
        return !m_arrayValue->empty();
    }
    return false;
}

//获取m_stringValue成员变量引用
std::string& JsonValue::getStringValue() {
    return *m_stringValue;
}

//获取m_objectValue成员变量引用
std::map<std::string, JsonValue>& JsonValue::getObjectValue() {
    return *m_objectValue;
}

//获取m_arrayValue成员变量引用
std::vector<JsonValue>& JsonValue::getArrayValue() {
    return *m_arrayValue;
}

// 递归打印JSON值
void JsonValue::print(int indent) const {//indent控制空格数量

    if (isObject()) {
        std::cout << std::string(indent, ' ') << "{\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;

            // 打印当前键值对
            std::cout << std::string(indent + 2, ' ') << "\"" << key_value.first << "\":";
            key_value.second.print(indent + 2);

            // 判断是否为最后一个元素
            if (std::next(it) != m_objectValue->end()) {
                std::cout << ",\n";  // 如果不是最后一个元素，输出逗号和换行
            }
            else {
                std::cout << "\n";   // 如果是最后一个元素，不输出逗号
            }
        }
        std::cout << std::string(indent, ' ') << "}";
    }
    else if (isArray()) {
        std::cout << "[\n";
        for (auto it = m_arrayValue->begin(); it != m_arrayValue->end(); ++it) {
            it->print(indent + 2);
            // 判断是否为最后一个元素
            if (std::next(it) != m_arrayValue->end()) {
                std::cout << ",\n";  // 如果不是最后一个元素，输出逗号
            }
        }
        std::cout << std::string(indent, ' ') << "\n" << std::string(indent, ' ') << "]";
    }
    else if (m_isData || m_isBoolValue) {
        std::cout << *m_stringValue;
    }
    else if (m_isNullValue || *m_stringValue == "") {
        std::cout << "null";
    }
    else {
        std::cout << "\"" << *m_stringValue << "\"";
    }
}

//递归生成json序列化
void JsonValue::serializeToStream(std::ofstream& outfile, int indent) const
{
    if (isObject()) {
        outfile << std::string(indent, ' ') << "{\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;

            // 打印当前键值对
            outfile << std::string(indent + 2, ' ') << "\"" << key_value.first << "\":";
            key_value.second.serializeToStream(outfile, indent + 2);

            // 判断是否为最后一个元素
            if (std::next(it) != m_objectValue->end()) {
                outfile << ",\n";  // 如果不是最后一个元素，输出逗号和换行
            }
            else {
                outfile << "\n";   // 如果是最后一个元素，不输出逗号
            }
        }
        outfile << std::string(indent, ' ') << "}";
    }
    else if (isArray()) {
        outfile << "[\n";
        for (auto it = m_arrayValue->begin(); it != m_arrayValue->end(); ++it) {
            it->serializeToStream(outfile, indent + 2);
            // 判断是否为最后一个元素
            if (std::next(it) != m_arrayValue->end()) {
                outfile << ",\n";  // 如果不是最后一个元素，输出逗号
            }
        }
        outfile << std::string(indent, ' ') << "\n" << std::string(indent, ' ') << "]";
    }
    else if (m_isData || m_isBoolValue) {
        outfile << *m_stringValue;
    }
    else if (m_isNullValue || *m_stringValue == "") {
        outfile << "null";
    }
    else {
        outfile << "\"" << *m_stringValue << "\"";
    }
}

void JsonValue::printKeys(std::vector<std::string>& lastKeys)
{
    if (isObject()) {
        for (const auto& elem : *m_objectValue) {
            std::cout << elem.first << "  ";
            lastKeys.push_back(elem.first);//记录限制下次输入内容
        }
        std::cout << std::endl;
    }
    else if (isArray()) {
        std::cout << "The maximum list number :" << m_arrayValue->size() << std::endl;
        for (size_t i = 0; i < m_arrayValue->size(); i++) {
            lastKeys.push_back(std::to_string(i + 1));//记录限制下次输入内容
        }

    }
    else {
        std::cout << "Not find value !" << std::endl;
    }
}

// 递归打印XML值
void JsonValue::printXML(const std::string& outLayLabel, int indent) const {
    //处理头部label
    if (isArray() || (!isArray() && m_isNullValue) || (!isArray() && !m_isNullValue && !isObject() && m_stringValue->empty())) {//处理空值
         //若是数组不打印外层键值（因为数组内部会打印）
         //若不是数组但是是"null"值也不打印外层键值
         //若不是数组也不是"null"值也不是map对象但是存储值为空，也不打印外层键值
    }
    else {
        std::cout << std::string(indent, ' ') << "<" << outLayLabel << ">";// 打印当前键值
    }
    //处理内部
    if (isObject()) {
        std::cout << "\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;
            key_value.second.printXML(key_value.first, indent + 2);
        }
        std::cout << std::string(indent, ' ');
    }
    else if (isArray()) {
        for (const auto& elem : *m_arrayValue) {
            elem.printXML(outLayLabel, indent);
        }
    }
    else if (m_isData || m_isBoolValue) {
        std::cout << *m_stringValue;
    }
    else if (m_isNullValue || m_stringValue->empty()) {
        std::cout << std::string(indent, ' ');//因为null值没有输出头部标签<label> 要补上对应空缺
    }
    else {
        std::cout << *m_stringValue;
    }

    //处理尾部label
    if (!isArray()) {
        std::cout << "<" << outLayLabel << "/>" << "\n";
    }

}

//递归生成xml序列
void JsonValue::serializeToXMLStream(std::ofstream& outfile, const std::string& outLayLabel, int indent) const
{
    //处理头部label
    if (isArray() || (!isArray() && m_isNullValue) || (!isArray() && !m_isNullValue && !isObject() && m_stringValue->empty())) {//处理空值
         //若是数组不打印外层键值（因为数组内部会打印）
         //若不是数组但是是"null"值也不打印外层键值
         //若不是数组也不是"null"值也不是map对象但是存储值为空，也不打印外层键值
    }
    else {
        outfile << std::string(indent, ' ') << "<" << outLayLabel << ">";// 打印当前键值
    }
    //处理内部
    if (isObject()) {
        outfile << "\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;
            key_value.second.serializeToXMLStream(outfile, key_value.first, indent + 2);
        }
        outfile << std::string(indent, ' ');
    }
    else if (isArray()) {
        for (const auto& elem : *m_arrayValue) {
            elem.serializeToXMLStream(outfile, outLayLabel, indent);
        }
    }
    else if (m_isData || m_isBoolValue) {
        outfile << *m_stringValue;
    }
    else if (m_isNullValue || m_stringValue->empty()) {
        outfile << std::string(indent, ' ');//因为null值没有输出头部标签<label> 要补上对应空缺
    }
    else {
        outfile << *m_stringValue;
    }

    //处理尾部label
    if (!isArray()) {
        outfile << "<" << outLayLabel << "/>" << "\n";
    }
}


