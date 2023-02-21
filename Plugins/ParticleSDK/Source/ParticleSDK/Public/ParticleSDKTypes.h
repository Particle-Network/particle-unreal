#pragma once

#include "ParticleSDKTypes.generated.h"


UENUM(BlueprintType)
enum class SignTypedDataVersion : uint8
{
	Default UMETA(DisplayName = "Default"),
	V1 UMETA(DisplayName = "V1"),
	V3 UMETA(DIsplayName = "V3"),
	V4 UMETA(DIsplayName = "V4"),
};

class FSignMethod
{
public:
	// Evm
	static const FString Eth_SendTransaction;
	static const FString Eth_SignTypedData;
	static const FString Eth_SignTypedData1;
	static const FString Eth_SignTypedData3;
	static const FString Eth_SignTypedData4;
	static const FString Personal_Sign;

	// Solana
	static const FString SignTransaction;
	static const FString SignAndSendTransaction;
	static const FString SignMessage;
	static const FString SignAllTransactions;
};


USTRUCT(BlueprintType)
struct FOnSignResultError
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Particle | Result")
	int Code;

	UPROPERTY(BlueprintReadWrite, Category="Particle | Result")
	FString Message;
};

USTRUCT(BlueprintType)
struct FOnSignResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Particle | Result")
	FString Method;

	UPROPERTY(BlueprintReadWrite, Category="Particle | Result")
	FString Signature;

	UPROPERTY(BlueprintReadWrite, Category="Particle | Result")
	FOnSignResultError Error;
};
