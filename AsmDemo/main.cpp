#include <windows.h>
#include <iostream>
#include <print>
#include <windowsx.h>
#include <string>

#include <Windows.h>
#include <bcrypt.h>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "bcrypt.lib")

int main()
{
    const char* data = "hello";

    // --------------------------------
    // 1. 打开 MD5 算法提供程序
    // --------------------------------

    BCRYPT_ALG_HANDLE hAlg = nullptr;

    NTSTATUS status = BCryptOpenAlgorithmProvider(
        &hAlg,
        BCRYPT_MD5_ALGORITHM,
        nullptr,
        0
    );

    if (status != 0)
    {
        std::cerr << "BCryptOpenAlgorithmProvider failed\n";
        return 1;
    }


    // --------------------------------
    // 2. MD5 输出固定为 16 字节
    // --------------------------------

    BYTE hash[16];


    // --------------------------------
    // 3. 计算 MD5
    // --------------------------------

    status = BCryptHash(
        hAlg,
        nullptr,                       // pbSecret
        0,                              // cbSecret
        (PUCHAR)data,                   // pbInput
        static_cast<ULONG>(strlen(data)),
        hash,                           // pbOutput
        sizeof(hash)                    // cbOutput
    );

    if (status != 0)
    {
        std::cerr << "BCryptHash failed\n";

        BCryptCloseAlgorithmProvider(hAlg, 0);
        return 1;
    }


    // --------------------------------
    // 4. 输出十六进制
    // --------------------------------

    for (BYTE byte : hash)
    {
        std::cout
            << std::hex
            << std::setw(2)
            << std::setfill('0')
            << static_cast<int>(byte);
    }

    std::cout << '\n';


    // --------------------------------
    // 5. 关闭算法提供程序
    // --------------------------------

    BCryptCloseAlgorithmProvider(hAlg, 0);

    return 0;
}
