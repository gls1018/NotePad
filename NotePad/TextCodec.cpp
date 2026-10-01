#include "TextCodec.h"

TextCodec::Encoding TextCodec::Detect(const std::vector<uint8_t>& data)
{
	// 先检测是不是 UTF32
	if (data.size() >= 4)
	{
		if (data[0] == 0xFF &&
			data[1] == 0xFE &&
			data[2] == 0x00 &&
			data[3] == 0x00)
		{
			return TextCodec::Encoding::UTF32_LE;
		}

		if (data[0] == 0x00 &&
			data[1] == 0x00 &&
			data[2] == 0xFE &&
			data[3] == 0xFF)
		{
			return Encoding::UTF32_BE;
		}
	}

	// 检测是不是UTF8_BOM
	if (data.size() >= 3)
	{
		if (data[0] == 0xEF &&
			data[1] == 0xBB &&
			data[2] == 0xBF)
		{
			return Encoding::UTF8_BOM;
		}
	}

	// 检测是不是UTF16
	if (data.size() >= 2)
	{
		if (data[0] == 0xFF &&
			data[1] == 0xFE)
		{
			return Encoding::UTF16_LE;
		}

		if (data[0] == 0xFE &&
			data[1] == 0xFF)
		{
			return Encoding::UTF16_BE;
		}
	}

    return Encoding();
}
