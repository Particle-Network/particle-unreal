#pragma once

#include "FParticleParams.generated.h"


UCLASS(BlueprintType)
class UParticleConfigSecurityAccount final : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	int PromptSettingWhenSign;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	int PromptMasterPasswordSettingWhenLogin;
};

UCLASS(BlueprintType)
class UParticleConfigWallet final : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	bool DisplayWalletEntry;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	FVector2D DefaultWalletEntryPosition;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	TArray<FString> SupportChains;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	FString CustomStyle;
};

UCLASS(BlueprintType)
class UParticleConfig final : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	FString ProjectId;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	FString ClientKey;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	FString AppId;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	UParticleConfigSecurityAccount* SecurityAccount;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Config")
	UParticleConfigWallet* Wallet;

	UFUNCTION(BlueprintCallable)
	FString ToJsonString() const;
};


UCLASS(BlueprintType)
class UParticleTheme final: public  UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category="Particle | Theme")
	FString UiMode;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Theme")
	bool DisplayCloseButton;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Theme")
	bool DisplayWallet;
	UPROPERTY(BlueprintReadWrite, Category="Particle | Theme")
	int ModalBorderRadius;

	FString ToJsonString() const;
};
