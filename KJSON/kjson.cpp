#include "kjson.h"
#include <stdlib.h>



KJson::KJson() : m_root(std::make_shared<JsonValue>()), m_mtx(std::make_shared<std::mutex>()) {}

KJson::KJson(const JsonValue& rootValue)
    : m_root(std::make_shared<JsonValue>(rootValue)), m_mtx(std::make_shared<std::mutex>()) {}

KJson::KJson(JsonValue&& rootValue)
    : m_root(std::make_shared<JsonValue>(std::move(rootValue))), m_mtx(std::make_shared<std::mutex>()) {}

// 从文件中解析JSON
bool KJson::parseFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);//按二进制读取不编码
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::istringstream ss(buffer.str());
    try {
        std::lock_guard<std::mutex> lock(*m_mtx);//上锁
        *m_root = parseJson(ss);
        file.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }

    return true;
}

//json的动态管理
void KJson::manageJsonData() {
    std::vector<std::string> keys;//存储总键值
    std::string action, line;//记录行为与内容
    std::shared_ptr<JsonValue> current = get({});  // 当前节点
    std::vector<std::string> lastKeys;//当前节点keys选项
    std::cout << "Current directory :" << std::endl;
    current->printKeys(lastKeys);  // 初始合法键列表
    // 显示操作提示
    std::cout << "Enter action (go, add, update, delete) or 'exit' to quit:\n";
    while (true) {
        std::cout << "Action: ";
        std::getline(std::cin, action);
        if (action == "exit") break;  // 用户输入 'exit' 时退出循环
        if (action != "add" && action != "update" && action != "delete" && action != "go") {
            std::cout << "Invalid action. Please enter 'go','add', 'update', 'delete' or 'exit'.\n";
            continue;
        }
        // 动态输入键名
        while (true) {
            std::cout << "Enter key name or index (or 'done' to finish keys): ";
            std::string nowLine;
            std::getline(std::cin, nowLine);
            if (nowLine == "up") {//返回上一级
                if (!keys.empty())keys.pop_back();  //删除 keys 列表中有效键回到上一级
                current = get(keys);  // 获取上层级的 JsonValue 对象
                // 更新键列表
                updateLastKey(*current, lastKeys);
                break;
            }
            if (nowLine == "done") break;  // 键输入完成
            line = nowLine;
            if (action == "add") continue;//添加不需要检测key合法性
            // 检查键是否有效
            if (!lastKeys.empty() && std::find(lastKeys.begin(), lastKeys.end(), line) == lastKeys.end()) {
                std::cout << "Invalid key or index. Try again.\n";
            }
        }
        try {// 执行用户指定的操作
            if (action == "go" && line != "up") {
                keys.push_back(line);  //加入keys 列表中有效键
                current = get(keys);  // 更新目录
                // 更新键列表
                if (!updateLastKey(*current, lastKeys)) {
                    break;
                }
            }
            else if (action == "add" || action == "update") {
                std::cout << "Enter value to " << (action == "add" ? "add" : "update") << ": ";
                std::string value; std::getline(std::cin, value);
                std::istringstream ss(value);
                JsonValue jValue;
                try
                {//捕捉是否能转换
                    jValue = parseJson(ss);
                }
                catch (const std::exception& e)
                {
                    std::cout << e.what() << std::endl << "Fail to " << (action == "add" ? "add" : "updat") << " value.\n";
                    continue;
                }
                if (current->isObject()) {
                    (*current).getObjectValue()[line] = jValue;
                }
                else {
                    std::cout << "Fail to " << (action == "add" ? "add" : "updat") << " value.\n";
                    continue;
                }
                std::cout << "Successfully " << (action == "add" ? "added" : "updated") << " value.\n";
                // 更新键列表
                if (!updateLastKey(*current, lastKeys)) {
                    break;
                }
            }
            else if (action == "delete") {
                if (current->isObject())(*current).getObjectValue().erase(line);
                else {
                    std::cout << "Not to delete key.\n";
                    continue;
                }
                std::cout << "Successfully deleted key.\n";
                // 更新键列表
                if (!updateLastKey(*current, lastKeys)) {
                    break;
                }
            }
            // 显示当前键对应的 JSON 对象结构
            (*current).print();
            std::cout << std::endl;

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

}

// 解析整个JSON并返回JsonValue对象
JsonValue KJson::parseJson(std::istringstream& ss) {
    skipWhitespace(ss);
    char next = ss.peek();
    try {
        if (next == '{') {
            return JsonValue(parseObject(ss));
        }
        else if (next == '[') {
            return JsonValue(parseArray(ss));
        }
        else if (next == '"') {
            return JsonValue(parseString(ss));
        }
        else if (std::isdigit(next) || next == 'f' || next == 't' || next == 'n') {
            std::string value = parseData(ss);
            bool isDataFlag = isData(value);
            return JsonValue(value, isDataFlag, isNull(value), isBool(value));
        }
        else {
            throw std::runtime_error("Unknown JSON value format at position: " + std::to_string(ss.tellg()));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON at position: " << ss.tellg() << " - " << e.what() << std::endl;
        throw; // Re-throw to handle in higher-level function if needed
    }
}

//工具函数（用于更新manageJsonData（）动态管理的合法检查表）
bool KJson::updateLastKey(JsonValue& current, std::vector<std::string>& lastKeys) {
    // 更新键列表
    if (current.isArray() || current.isObject()) {
        lastKeys.clear();
        current.printKeys(lastKeys);
        return true;
    }
    return false;
}

// 工具函数：跳过空白及注释字符
void KJson::skipWhitespace(std::istringstream& ss) {//跳过输入流开头的所有空白字符
    while (isspace(ss.peek())) {
        ss.get();
    }
    // 检查是否为注释起始符 "//"
    if (ss.peek() == '/') {
        ss.get(); // 读取第一个 '/'
        if (ss.peek() == '/') {
            // 确认第二个字符是 '/'，跳过整行直到换行符
            while (ss.peek() != '\n' && ss) {
                ss.get();
            }
        }
    }
    while (isspace(ss.peek())) {
        ss.get();
    }
}

// 工具函数：解析字符串（以引号包裹）
std::string KJson::parseString(std::istringstream& ss) {
    std::string result;
    char ch;
    if (ss.get() != '"') {
        throw std::runtime_error("Expected opening quote for string.");
    }
    while (ss.get(ch)) {
        if (ch == '"') {
            break;
        }
        result += ch;
    }
    //std::cout << result <<" "<< std::endl;
    return result;
}

// 工具函数：解析数字
std::string KJson::parseData(std::istringstream& ss) {
    std::string result;
    char ch;
    while ((!ss.eof()) & (ss.peek() != ' ') & (ss.peek() != ',') & (ss.peek() != '}')) {
        ss.get(ch);
        result += ch;
    }

    return result;
}

// 解析JSON数组
std::vector<JsonValue> KJson::parseArray(std::istringstream& ss) {
    std::vector<JsonValue> array;
    skipWhitespace(ss);
    if (ss.get() != '[') {
        throw std::runtime_error("Expected '[' for array.");
    }

    skipWhitespace(ss);
    while (ss.peek() != ']') {
        array.push_back(parseJson(ss));
        skipWhitespace(ss);
        if (ss.peek() == ',') {
            ss.get();  // 跳过逗号
        }
        skipWhitespace(ss);
    }
    ss.get();  // 读取数组的结尾 ']'
    return array;
}

// 解析JSON对象
std::map<std::string, JsonValue> KJson::parseObject(std::istringstream& ss) {
    std::map<std::string, JsonValue> object;
    skipWhitespace(ss);
    if (ss.get() != '{') {
        throw std::runtime_error("Expected '{' for object.");
    }

    skipWhitespace(ss);
    while (ss.peek() != '}') {
        std::string key = parseString(ss);  // 解析键
        skipWhitespace(ss);
        if (ss.get() != ':') {
            throw std::runtime_error("Expected ':' after key.");
        }
        skipWhitespace(ss);
        JsonValue value = parseJson(ss);  // 解析值
        object[key] = value;

        skipWhitespace(ss);
        if (ss.peek() == ',') {
            ss.get();  // 跳过逗号
        }
        skipWhitespace(ss);
    }
    ss.get();  // 读取对象的结尾 '}'
    return object;
}

// 工具函数：判断是否为数字、NULL、BOOL
bool KJson::isData(std::string& value) {
    if (('0' > value[0]) || (value[0] > '9')) { return false; }
    std::ostringstream oss;// 创建一个输出字符串流，不手动设置精度
    // Check for integer
    size_t idx;
    int intValue = std::stoi(value, &idx);
    if (idx == value.size()) {
        oss << intValue;
        value = oss.str();
        return true;
    }
    // Check for double
    double doubleValue = std::stod(value, &idx);
    if (idx == value.size()) {
        oss << doubleValue;
        value = oss.str();
        return true;
    }
    return false;
}

bool KJson::isNull(std::string& value) {
    // Check for "null"
    if (value == "null") {
        return true;
    }
    return false;
}

bool KJson::isBool(std::string& value) {

    // Check for boolean values
    if ((value == "true") || (value == "false")) {
        return true;
    }
    return false;
}

// 序列化Json并写入文件
bool KJson::serializeToFile(const std::string& filePath) const {
    // 以二进制模式打开文件用于写入，如果文件不存在则创建它  
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    // 序列化输出，递归写入内容
    m_root->serializeToStream(file);
    return true;
}

// 序列化xml并写入文件
bool KJson::serializeXMLToFile(const std::string& filePath) const
{
    // 以二进制模式打开文件用于写入，如果文件不存在则创建它  
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    // 序列化输出，递归写入内容
    file << "<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \n";//默认加入xml头
    m_root->serializeToXMLStream(file, "root");
    return true;
}

// 获取嵌套值
std::shared_ptr<JsonValue> KJson::get(const std::vector<std::string>& keys) {
    std::shared_ptr<JsonValue> current = m_root;
    for (const auto& key : keys) {
        if (current->isObject()) {
            auto it = current->getObjectValue().find(key);
            if (it == current->getObjectValue().end()) {
                throw std::runtime_error("Key not found: " + key);
            }
            *current = it->second;
        }
        else if (current->isArray()) {
            size_t index = std::stoi(key);
            if (index >= current->getArrayValue().size()) {
                throw std::runtime_error("Index out of range: " + key);
            }
            //auto it = current->m_arrayValue[index-1];//auto 不知道为什么加入这个it current超过作用域就报错指针找不到内容
            *current = current->getArrayValue()[index];
        }
        else {
            throw std::runtime_error("Attempted to access non-object or non-array JSON value.");
        }
    }
    return current;
}

// 删除键值对
void KJson::deleteKeyValue(std::vector<std::string> keys) {
    std::string backKey;
    if (!keys.empty()) {
        backKey = keys.back();
        keys.pop_back();
    }
    else {
        std::cout << "keys is empty!" << std::endl;
    }
    (get(keys))->getObjectValue().erase(backKey);

}

// 重载 operator[]，返回 JsonValue 的引用(支持嵌套键名)
JsonValue& KJson::operator[](std::vector<std::string> keys) {
    std::string backKey;
    if (!keys.empty()) {
        backKey = keys.back();
        keys.pop_back();
    }
    else {
        std::cout << "Keys is empty!" << std::endl;
    }
    return (get(keys))->getObjectValue()[backKey];
}

// 打印整个JSON结构
void KJson::printJson() {
    std::lock_guard<std::mutex> lock(*m_mtx);
    m_root->print();
}

// 打印整个xml结构
void KJson::printXml() {
    std::lock_guard<std::mutex> lock(*m_mtx);
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \n";//默认加入xml头
    m_root->printXML("root");
}

void KJson::queryJson() {
    std::vector<std::string> keys, lastKeys;//keys:存储总查询键lastKeys:记录并限制下次输入内容
    std::string line;

    // 初始化根节点的有效键列表
    std::shared_ptr<JsonValue> rootValue = get({});  // 获取根节点的 JsonValue 对象
    if (rootValue->isObject()) {
        rootValue->printKeys(lastKeys);  // 填充根节点的有效键列表
    }
    else if (rootValue->isArray()) {
        // 如果根节点是数组，则初始化索引列表
        for (size_t i = 0; i < rootValue->getArrayValue().size(); ++i) {
            lastKeys.push_back(std::to_string(i));
        }
    }

    while (true) {
        // 根据 keys 是否为空，动态显示提示信息
        std::cout << (keys.empty() ? "Enter key or list index: " : "Next key or index: ");
        std::getline(std::cin, line);
        if (line.empty()) break;  // 输入为空时退出循环

        // 检查输入是否合法
        if (!lastKeys.empty() && std::find(lastKeys.begin(), lastKeys.end(), line) == lastKeys.end()) {
            std::cout << "Invalid key/index. Try again.\n";
            continue;
        }

        keys.push_back(line);  // 将合法的键或索引添加到 keys 中
        std::shared_ptr<JsonValue> jValue = get(keys);

        // 如果键值是对象或数组，则继续进入下一层
        if (jValue->isArray() || jValue->isObject()) {
            lastKeys.clear();  // 清空有效键列表，准备加载下一层的有效键
            jValue->printKeys(lastKeys);  // 更新有效键列表
        }
        else {
            jValue->print();
            std::cout << "\n";
            break;
        }
    }
}



