#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <string>
#include <map>
#include <print>
#include <iostream>
#pragma comment (lib, "bcrypt.lib")


int main()
{
	uint32_t value32{};
	uint64_t value64{};
	NTSTATUS status32 = BCryptGenRandom(NULL, (UCHAR*)&value32, sizeof(value32), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	NTSTATUS status64 = BCryptGenRandom(NULL, (UCHAR*)&value64, sizeof(value64), BCRYPT_USE_SYSTEM_PREFERRED_RNG);

	if (!BCRYPT_SUCCESS(status32) && !BCRYPT_SUCCESS(status64))
		return -1;
	std::print("0x{:0>8X}\n", value32);
	std::print("0x{:0>16X}\n", value64);
	return 0;
}