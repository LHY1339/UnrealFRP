#include "BaseLib.h"

bool UBaseLib::SplitByte(const uint8* buf, int len, uint8 sep, const uint8*& left, int& left_len, const uint8*& right, int& right_len)
{
	//遍历字符查找sep，如果找到则分割成两段然后返回true
	//Traverse bytes to find sep; if found, split into two parts and return true
	for (int i = 0; i < len; i++)
	{
		if (buf[i] == sep)
		{
			left = buf;
			left_len = i;
			right = buf + i + 1;
			right_len = len - i - 1;
			return true;
		}
	}
	return false;
}


TArray<FString> UBaseLib::SplitString(const FString& str, const TCHAR spl)
{
	//将str按照spl分割进TArray
	//Split str by spl into TArray
	TArray<FString> value;
	FString delim;
	delim.AppendChar(spl);
	str.ParseIntoArray(value, *delim, true);
	return value;
}
