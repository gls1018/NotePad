#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <print>
#include <bitset>
#include <memory>

class TextCodec
{
	enum class Encoding{
		UnKnown,
		UTF8,
		UTF8_BOM,
		UTF16_LE,
		UTF16_BE,
		UTF32_LE,
		UTF32_BE,
	};


public:
	static Encoding Detect(const std::vector<uint8_t>& Data);

	static Encoding DetectFromFile(const std::wstring& filePath);
};