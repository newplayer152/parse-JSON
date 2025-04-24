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

// �жϵ�ǰֵ�Ƿ�Ϊ����
bool JsonValue::isObject() const {
    if (m_objectValue != nullptr) {
        return !m_objectValue->empty();
    }
    return false;
}

// �жϵ�ǰֵ�Ƿ�Ϊ����
bool JsonValue::isArray() const {
    if (m_arrayValue != nullptr) {
        return !m_arrayValue->empty();
    }
    return false;
}

//��ȡm_stringValue��Ա��������
std::string& JsonValue::getStringValue() {
    return *m_stringValue;
}

//��ȡm_objectValue��Ա��������
std::map<std::string, JsonValue>& JsonValue::getObjectValue() {
    return *m_objectValue;
}

//��ȡm_arrayValue��Ա��������
std::vector<JsonValue>& JsonValue::getArrayValue() {
    return *m_arrayValue;
}

// �ݹ��ӡJSONֵ
void JsonValue::print(int indent) const {//indent���ƿո�����

    if (isObject()) {
        std::cout << std::string(indent, ' ') << "{\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;

            // ��ӡ��ǰ��ֵ��
            std::cout << std::string(indent + 2, ' ') << "\"" << key_value.first << "\":";
            key_value.second.print(indent + 2);

            // �ж��Ƿ�Ϊ���һ��Ԫ��
            if (std::next(it) != m_objectValue->end()) {
                std::cout << ",\n";  // ����������һ��Ԫ�أ�������źͻ���
            }
            else {
                std::cout << "\n";   // ��������һ��Ԫ�أ����������
            }
        }
        std::cout << std::string(indent, ' ') << "}";
    }
    else if (isArray()) {
        std::cout << "[\n";
        for (auto it = m_arrayValue->begin(); it != m_arrayValue->end(); ++it) {
            it->print(indent + 2);
            // �ж��Ƿ�Ϊ���һ��Ԫ��
            if (std::next(it) != m_arrayValue->end()) {
                std::cout << ",\n";  // ����������һ��Ԫ�أ��������
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

//�ݹ�����json���л�
void JsonValue::serializeToStream(std::ofstream& outfile, int indent) const
{
    if (isObject()) {
        outfile << std::string(indent, ' ') << "{\n";
        for (auto it = m_objectValue->begin(); it != m_objectValue->end(); ++it) {
            const auto& key_value = *it;

            // ��ӡ��ǰ��ֵ��
            outfile << std::string(indent + 2, ' ') << "\"" << key_value.first << "\":";
            key_value.second.serializeToStream(outfile, indent + 2);

            // �ж��Ƿ�Ϊ���һ��Ԫ��
            if (std::next(it) != m_objectValue->end()) {
                outfile << ",\n";  // ����������һ��Ԫ�أ�������źͻ���
            }
            else {
                outfile << "\n";   // ��������һ��Ԫ�أ����������
            }
        }
        outfile << std::string(indent, ' ') << "}";
    }
    else if (isArray()) {
        outfile << "[\n";
        for (auto it = m_arrayValue->begin(); it != m_arrayValue->end(); ++it) {
            it->serializeToStream(outfile, indent + 2);
            // �ж��Ƿ�Ϊ���һ��Ԫ��
            if (std::next(it) != m_arrayValue->end()) {
                outfile << ",\n";  // ����������һ��Ԫ�أ��������
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
            lastKeys.push_back(elem.first);//��¼�����´���������
        }
        std::cout << std::endl;
    }
    else if (isArray()) {
        std::cout << "The maximum list number :" << m_arrayValue->size() << std::endl;
        for (size_t i = 0; i < m_arrayValue->size(); i++) {
            lastKeys.push_back(std::to_string(i + 1));//��¼�����´���������
        }

    }
    else {
        std::cout << "Not find value !" << std::endl;
    }
}

// �ݹ��ӡXMLֵ
void JsonValue::printXML(const std::string& outLayLabel, int indent) const {
    //����ͷ��label
    if (isArray() || (!isArray() && m_isNullValue) || (!isArray() && !m_isNullValue && !isObject() && m_stringValue->empty())) {//�����ֵ
         //�������鲻��ӡ����ֵ����Ϊ�����ڲ����ӡ��
         //���������鵫����"null"ֵҲ����ӡ����ֵ
         //����������Ҳ����"null"ֵҲ����map�����Ǵ洢ֵΪ�գ�Ҳ����ӡ����ֵ
    }
    else {
        std::cout << std::string(indent, ' ') << "<" << outLayLabel << ">";// ��ӡ��ǰ��ֵ
    }
    //�����ڲ�
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
        std::cout << std::string(indent, ' ');//��Ϊnullֵû�����ͷ����ǩ<label> Ҫ���϶�Ӧ��ȱ
    }
    else {
        std::cout << *m_stringValue;
    }

    //����β��label
    if (!isArray()) {
        std::cout << "<" << outLayLabel << "/>" << "\n";
    }

}

//�ݹ�����xml����
void JsonValue::serializeToXMLStream(std::ofstream& outfile, const std::string& outLayLabel, int indent) const
{
    //����ͷ��label
    if (isArray() || (!isArray() && m_isNullValue) || (!isArray() && !m_isNullValue && !isObject() && m_stringValue->empty())) {//�����ֵ
         //�������鲻��ӡ����ֵ����Ϊ�����ڲ����ӡ��
         //���������鵫����"null"ֵҲ����ӡ����ֵ
         //����������Ҳ����"null"ֵҲ����map�����Ǵ洢ֵΪ�գ�Ҳ����ӡ����ֵ
    }
    else {
        outfile << std::string(indent, ' ') << "<" << outLayLabel << ">";// ��ӡ��ǰ��ֵ
    }
    //�����ڲ�
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
        outfile << std::string(indent, ' ');//��Ϊnullֵû�����ͷ����ǩ<label> Ҫ���϶�Ӧ��ȱ
    }
    else {
        outfile << *m_stringValue;
    }

    //����β��label
    if (!isArray()) {
        outfile << "<" << outLayLabel << "/>" << "\n";
    }
}


