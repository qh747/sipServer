#include "SimpleIni.h"
#include <iostream>

int main() {
    CSimpleIniA ini;

    // 加载INI文件
    SI_Error rc = ini.LoadFile("example.ini");
    if (rc < 0) {
        std::cerr << "Failed to load INI file." << std::endl;
        return 1;
    }

    // 读取值
    const char* username = ini.GetValue("User", "Username", "default_user");
    const char* email = ini.GetValue("User", "Email", "");

    std::cout << "Username: " << username << std::endl;
    std::cout << "Email: " << email << std::endl;

    // 写入值
    ini.SetValue("User", "Username", "new_user");
    ini.SetValue("User", "Email", "user@example.com");

    // 保存到文件
    rc = ini.SaveFile("example.ini");
    if (rc < 0) {
        std::cerr << "Failed to save INI file." << std::endl;
        return 1;
    }

    return 0;
}
