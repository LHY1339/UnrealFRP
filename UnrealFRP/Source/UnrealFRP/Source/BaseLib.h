#pragma once

#include "CoreMinimal.h"
#include "BaseLib.generated.h"


UCLASS()
class UNREALFRP_API UBaseLib : public UObject
{
	GENERATED_BODY()

public:
	//分割字节（非string消息）
	//Split bytes (non-string message)
	static bool SplitByte(const uint8* buf, int len, uint8 sep, const uint8*& left, int& left_len, const uint8*& right, int& right_len);

	//分割string
	//Split string
	static TArray<FString> SplitString(const FString& str, const TCHAR spl);
};
