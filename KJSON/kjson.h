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

    // Ĭ�Ϲ��캯��
    KJson();

    // �� JsonValue ����Ĺ��캯��
    explicit KJson(const JsonValue& rootValue);

    // �� JsonValue ���ƶ����캯��
    explicit KJson(JsonValue&& rootValue);

    // Ĭ�Ͽ������캯�����ƶ����캯������ֵ���������������
    KJson(const KJson& other) = default;
    KJson(KJson&& other) noexcept = default;
    KJson& operator=(const KJson& other) = default;
    KJson& operator=(KJson&& other) noexcept = default;
    ~KJson() = default;

    // ���ļ��н���JSON
    bool parseFromFile(const std::string& filePath);

    // ��JSON���л���д���ļ�
    bool serializeToFile(const std::string& filePath) const;

    // ��JSONת��xml���л���д���ļ�
    bool serializeXMLToFile(const std::string& filePath) const;

    // ��ȡֵ��֧��Ƕ�׼�����
    std::shared_ptr<JsonValue> get(const std::vector<std::string>& keys);

    // ɾ��ֵ��֧��Ƕ�׼�����  ����Ķ�keys��ʹ��const &
    void deleteKeyValue(std::vector<std::string> keys);

    // ���� operator[]������ JsonValue ������(֧��Ƕ�׼���)
    JsonValue& operator[](std::vector<std::string> keys);

    // JSON�������
    void printJson();//����const Ϊ���ڶ�ʱ������

    // ��ӡ����xml�ṹ
    void printXml();//����const Ϊ���ڶ�ʱ������

    //��ѯ����
    void queryJson();

    //json�Ķ�̬����
    void manageJsonData();

private:
    std::shared_ptr<JsonValue> m_root;  // �洢����JSON����
    std::shared_ptr<std::mutex> m_mtx;  // ���ڶ��̰߳�ȫ

    // ����JSON�ڲ��ݹ麯��
    JsonValue parseJson(std::istringstream& ss);

    // ���ߺ����������ո�
    void skipWhitespace(std::istringstream& ss);

    // ���ߺ����������ַ���
    std::string parseString(std::istringstream& ss);

    // ���ߺ�������������
    std::string parseData(std::istringstream& ss);

    // ���ߺ�������������
    std::vector<JsonValue> parseArray(std::istringstream& ss);

    // ���ߺ�������������
    std::map<std::string, JsonValue> parseObject(std::istringstream& ss);

    // ���ߺ������ж��Ƿ�Ϊ���֡�NULL��BOOL
    bool isData(std::string& value);

    bool isNull(std::string& value);

    bool isBool(std::string& value);

    //���ߺ��������ڸ���manageJsonData������̬����ĺϷ�����
    bool updateLastKey(JsonValue& current, std::vector<std::string>& lastKeys);

};

#endif //_KJSON_KJSON_H
