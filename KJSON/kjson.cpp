#include "kjson.h"
#include <stdlib.h>



KJson::KJson() : m_root(std::make_shared<JsonValue>()), m_mtx(std::make_shared<std::mutex>()) {}

KJson::KJson(const JsonValue& rootValue)
    : m_root(std::make_shared<JsonValue>(rootValue)), m_mtx(std::make_shared<std::mutex>()) {}

KJson::KJson(JsonValue&& rootValue)
    : m_root(std::make_shared<JsonValue>(std::move(rootValue))), m_mtx(std::make_shared<std::mutex>()) {}

// ���ļ��н���JSON
bool KJson::parseFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);//�������ƶ�ȡ������
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::istringstream ss(buffer.str());
    try {
        std::lock_guard<std::mutex> lock(*m_mtx);//����
        *m_root = parseJson(ss);
        file.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return false;
    }

    return true;
}

//json�Ķ�̬����
void KJson::manageJsonData() {
    std::vector<std::string> keys;//�洢�ܼ�ֵ
    std::string action, line;//��¼��Ϊ������
    std::shared_ptr<JsonValue> current = get({});  // ��ǰ�ڵ�
    std::vector<std::string> lastKeys;//��ǰ�ڵ�keysѡ��
    std::cout << "Current directory :" << std::endl;
    current->printKeys(lastKeys);  // ��ʼ�Ϸ����б�
    // ��ʾ������ʾ
    std::cout << "Enter action (go, add, update, delete) or 'exit' to quit:\n";
    while (true) {
        std::cout << "Action: ";
        std::getline(std::cin, action);
        if (action == "exit") break;  // �û����� 'exit' ʱ�˳�ѭ��
        if (action != "add" && action != "update" && action != "delete" && action != "go") {
            std::cout << "Invalid action. Please enter 'go','add', 'update', 'delete' or 'exit'.\n";
            continue;
        }
        // ��̬�������
        while (true) {
            std::cout << "Enter key name or index (or 'done' to finish keys): ";
            std::string nowLine;
            std::getline(std::cin, nowLine);
            if (nowLine == "up") {//������һ��
                if (!keys.empty())keys.pop_back();  //ɾ�� keys �б�����Ч���ص���һ��
                current = get(keys);  // ��ȡ�ϲ㼶�� JsonValue ����
                // ���¼��б�
                updateLastKey(*current, lastKeys);
                break;
            }
            if (nowLine == "done") break;  // ���������
            line = nowLine;
            if (action == "add") continue;//��Ӳ���Ҫ���key�Ϸ���
            // �����Ƿ���Ч
            if (!lastKeys.empty() && std::find(lastKeys.begin(), lastKeys.end(), line) == lastKeys.end()) {
                std::cout << "Invalid key or index. Try again.\n";
            }
        }
        try {// ִ���û�ָ���Ĳ���
            if (action == "go" && line != "up") {
                keys.push_back(line);  //����keys �б�����Ч��
                current = get(keys);  // ����Ŀ¼
                // ���¼��б�
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
                {//��׽�Ƿ���ת��
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
                // ���¼��б�
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
                // ���¼��б�
                if (!updateLastKey(*current, lastKeys)) {
                    break;
                }
            }
            // ��ʾ��ǰ����Ӧ�� JSON ����ṹ
            (*current).print();
            std::cout << std::endl;

        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

}

// ��������JSON������JsonValue����
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

//���ߺ��������ڸ���manageJsonData������̬����ĺϷ�����
bool KJson::updateLastKey(JsonValue& current, std::vector<std::string>& lastKeys) {
    // ���¼��б�
    if (current.isArray() || current.isObject()) {
        lastKeys.clear();
        current.printKeys(lastKeys);
        return true;
    }
    return false;
}

// ���ߺ����������հ׼�ע���ַ�
void KJson::skipWhitespace(std::istringstream& ss) {//������������ͷ�����пհ��ַ�
    while (isspace(ss.peek())) {
        ss.get();
    }
    // ����Ƿ�Ϊע����ʼ�� "//"
    if (ss.peek() == '/') {
        ss.get(); // ��ȡ��һ�� '/'
        if (ss.peek() == '/') {
            // ȷ�ϵڶ����ַ��� '/'����������ֱ�����з�
            while (ss.peek() != '\n' && ss) {
                ss.get();
            }
        }
    }
    while (isspace(ss.peek())) {
        ss.get();
    }
}

// ���ߺ����������ַ����������Ű�����
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

// ���ߺ�������������
std::string KJson::parseData(std::istringstream& ss) {
    std::string result;
    char ch;
    while ((!ss.eof()) & (ss.peek() != ' ') & (ss.peek() != ',') & (ss.peek() != '}')) {
        ss.get(ch);
        result += ch;
    }

    return result;
}

// ����JSON����
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
            ss.get();  // ��������
        }
        skipWhitespace(ss);
    }
    ss.get();  // ��ȡ����Ľ�β ']'
    return array;
}

// ����JSON����
std::map<std::string, JsonValue> KJson::parseObject(std::istringstream& ss) {
    std::map<std::string, JsonValue> object;
    skipWhitespace(ss);
    if (ss.get() != '{') {
        throw std::runtime_error("Expected '{' for object.");
    }

    skipWhitespace(ss);
    while (ss.peek() != '}') {
        std::string key = parseString(ss);  // ������
        skipWhitespace(ss);
        if (ss.get() != ':') {
            throw std::runtime_error("Expected ':' after key.");
        }
        skipWhitespace(ss);
        JsonValue value = parseJson(ss);  // ����ֵ
        object[key] = value;

        skipWhitespace(ss);
        if (ss.peek() == ',') {
            ss.get();  // ��������
        }
        skipWhitespace(ss);
    }
    ss.get();  // ��ȡ����Ľ�β '}'
    return object;
}

// ���ߺ������ж��Ƿ�Ϊ���֡�NULL��BOOL
bool KJson::isData(std::string& value) {
    if (('0' > value[0]) || (value[0] > '9')) { return false; }
    std::ostringstream oss;// ����һ������ַ����������ֶ����þ���
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

// ���л�Json��д���ļ�
bool KJson::serializeToFile(const std::string& filePath) const {
    // �Զ�����ģʽ���ļ�����д�룬����ļ��������򴴽���  
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    // ���л�������ݹ�д������
    m_root->serializeToStream(file);
    return true;
}

// ���л�xml��д���ļ�
bool KJson::serializeXMLToFile(const std::string& filePath) const
{
    // �Զ�����ģʽ���ļ�����д�룬����ļ��������򴴽���  
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return false;
    }
    // ���л�������ݹ�д������
    file << "<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \n";//Ĭ�ϼ���xmlͷ
    m_root->serializeToXMLStream(file, "root");
    return true;
}

// ��ȡǶ��ֵ
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
            //auto it = current->m_arrayValue[index-1];//auto ��֪��Ϊʲô�������it current����������ͱ���ָ���Ҳ�������
            *current = current->getArrayValue()[index];
        }
        else {
            throw std::runtime_error("Attempted to access non-object or non-array JSON value.");
        }
    }
    return current;
}

// ɾ����ֵ��
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

// ���� operator[]������ JsonValue ������(֧��Ƕ�׼���)
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

// ��ӡ����JSON�ṹ
void KJson::printJson() {
    std::lock_guard<std::mutex> lock(*m_mtx);
    m_root->print();
}

// ��ӡ����xml�ṹ
void KJson::printXml() {
    std::lock_guard<std::mutex> lock(*m_mtx);
    std::cout << "<?xml version=\"1.0\" encoding=\"UTF - 8\"?> \n";//Ĭ�ϼ���xmlͷ
    m_root->printXML("root");
}

void KJson::queryJson() {
    std::vector<std::string> keys, lastKeys;//keys:�洢�ܲ�ѯ��lastKeys:��¼�������´���������
    std::string line;

    // ��ʼ�����ڵ����Ч���б�
    std::shared_ptr<JsonValue> rootValue = get({});  // ��ȡ���ڵ�� JsonValue ����
    if (rootValue->isObject()) {
        rootValue->printKeys(lastKeys);  // �����ڵ����Ч���б�
    }
    else if (rootValue->isArray()) {
        // ������ڵ������飬���ʼ�������б�
        for (size_t i = 0; i < rootValue->getArrayValue().size(); ++i) {
            lastKeys.push_back(std::to_string(i));
        }
    }

    while (true) {
        // ���� keys �Ƿ�Ϊ�գ���̬��ʾ��ʾ��Ϣ
        std::cout << (keys.empty() ? "Enter key or list index: " : "Next key or index: ");
        std::getline(std::cin, line);
        if (line.empty()) break;  // ����Ϊ��ʱ�˳�ѭ��

        // ��������Ƿ�Ϸ�
        if (!lastKeys.empty() && std::find(lastKeys.begin(), lastKeys.end(), line) == lastKeys.end()) {
            std::cout << "Invalid key/index. Try again.\n";
            continue;
        }

        keys.push_back(line);  // ���Ϸ��ļ���������ӵ� keys ��
        std::shared_ptr<JsonValue> jValue = get(keys);

        // �����ֵ�Ƕ�������飬�����������һ��
        if (jValue->isArray() || jValue->isObject()) {
            lastKeys.clear();  // �����Ч���б�׼��������һ�����Ч��
            jValue->printKeys(lastKeys);  // ������Ч���б�
        }
        else {
            jValue->print();
            std::cout << "\n";
            break;
        }
    }
}



