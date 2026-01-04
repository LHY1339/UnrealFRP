#pragma once

#include "CoreMinimal.h"
#include "FRPSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FRPLibrary.generated.h"

UCLASS()
class UNREALFRP_API UFRPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//获取FRPSubsystem
	//Get FRPSubsystem
	static UFRPSubsystem* GetSubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UnrealFRP", meta = (WorldContext = "WorldContextObject"))
	static void FlushFRPSessionList(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UnrealFRP", meta = (WorldContext = "WorldContextObject"))
	static void BindOnSessionListUpdate(UObject* WorldContextObject, FOnSessionUpdate Delegate);

	UFUNCTION(BlueprintCallable, Category = "UnrealFRP", meta = (WorldContext = "WorldContextObject"))
	static void UnbindOnSessionListUpdate(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static TArray<FFRPSession> GetSessionList(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetAppAddress(UObject* WorldContextObject, int Port);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetMsgAddress(UObject* WorldContextObject, int Port);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetBaseIP(UObject* WorldContextObject, FString IP);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void OpenSession(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void CloseSession(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetSessionProperty(UObject* WorldContextObject, FString Property);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetSessionPassword(UObject* WorldContextObject, FString Password);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void SetSessionPropertyAppend(UObject* WorldContextObject, FString PropertyAppend);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void AskForAddress(UObject* WorldContextObject, const FString& ID, const FString& Password);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void BindOnAddrUpdate(UObject* WorldContextObject, FOnAddrUpdate Delegate);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void UnbindOnAddrUpdate(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="UnrealFRP", meta=(WorldContext="WorldContextObject"))
	static void ConnectSession(UObject* WorldContextObject, FString Addr);
};
