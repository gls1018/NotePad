#include <windows.h>
#include <iostream>
#include <print>

#pragma optimize(off)

void func()
{
	throw("Runtime Error, JJJ\n");
}

int main()
{
	std::wcout << 0xE4B8A5 << std::endl;
	return 0;
}
#pragma optimize(on)