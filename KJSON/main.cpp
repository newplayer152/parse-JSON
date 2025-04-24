#include"kjson.h"

int main() {
    KJson json;
    system("chcp 65001");//控制台utf-8显示
    // 从文件中解析JSON
    if (!json.parseFromFile("input.json")) return 1;
    std::cout << "==============================Printing JSON Parsing Results (Out of Order)================================" << std::endl;
    // 打印json解析结果
    json.printJson();
    std::cout << std::endl;
    std::cout << "==================================Printing XML Parsing Results (Out of Order)==============================" << std::endl;
    // 打印xml解析结果
    json.printXml();
    //管理json
    std::cout << "========================Management JSON (adding, deleting, modifying, and querying)========================" << std::endl;

    try {//----------------------------------其中"cityInfo","newdata"可以从控制台接收，实现动态管理json对象---------------------------
        json.manageJsonData();
        // 查询键值
        std::cout << "=======================================Query key-value=====================================================" << std::endl;
        json.queryJson();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    //序列化并输出
    std::cout << "=======================================Serialize and output=====================================================" << std::endl;

    // 序列化为json文件（没有文件会创建文件）,序列化为xml文件（没有文件会创建文件）
    if (!json.serializeToFile("output.json") || !json.serializeXMLToFile("output.xml"))
        std::cerr << "Failed to serialize JSON/XML.\n";

    return 0;
}
