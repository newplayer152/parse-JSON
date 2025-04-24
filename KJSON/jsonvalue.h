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

    // Ĭ�Ϲ��캯��
    JsonValue();

    // ���ַ���ֵ�Ĺ��캯��
    explicit JsonValue(const std::string& stringValue);

    // ����ʶ�����Ĺ��캯����֧�����֡���ֵ�Ͳ�������
    JsonValue(const std::string& stringValue, bool isData, bool isNull, bool isBool);

    // ���������͵Ĺ��캯��
    explicit JsonValue(const std::map<std::string, JsonValue>& objectValue);

    // ���������͵Ĺ��캯��
    explicit JsonValue(const std::vector<JsonValue>& arrayValue);

    // �������캯��
    JsonValue(const JsonValue& other) = default;

    // �ƶ����캯��
    JsonValue(JsonValue&& other) noexcept = default;

    // ������ֵ�����
    JsonValue& operator=(const JsonValue& other) = default;

    // �ƶ���ֵ�����
    JsonValue& operator=(JsonValue&& other) noexcept = default;

    // ��������
    ~JsonValue() = default;

    // �жϵ�ǰֵ�Ƿ�Ϊ����
    bool isObject() const;

    // �жϵ�ǰֵ�Ƿ�Ϊ����
    bool isArray() const;

    //��ȡm_stringValue��Ա��������
    std::string& getStringValue();

    //��ȡm_objectValue��Ա��������
    std::map<std::string, JsonValue>& getObjectValue();

    //��ȡm_arrayValue��Ա��������
    std::vector<JsonValue>& getArrayValue();

    // �ݹ��ӡJSONֵ
    void print(int indent = 0) const;

    //��ӡ��ǰ������м�
    void printKeys(std::vector<std::string>& lastKeys);

    //�ݹ��������л�
    void serializeToStream(std::ofstream& outfile, int indent = 0) const;

    //�ݹ����xml�ļ�
    void printXML(const std::string& outLayLabel, int indent = 0) const;

    //�ݹ�����xml�ļ�
    void serializeToXMLStream(std::ofstream& outfile, const std::string& outLayLabel, int indent = 0) const;


private:

    bool m_isString = true;  // Ĭ�����ַ����������ڴ洢���з��б���json�����ֵ
    bool m_isData = false;
    bool m_isNullValue = false;
    bool m_isBoolValue = false;

    std::shared_ptr<std::string>  m_stringValue;// �ַ���ֵ
    std::shared_ptr<std::map<std::string, JsonValue>> m_objectValue; // �������͵�ֵ
    std::shared_ptr<std::vector<JsonValue>> m_arrayValue;  // �������͵�ֵ

};

#endif // _KJSON_JSONVALUE_H  
