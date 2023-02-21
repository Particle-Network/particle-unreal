// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ParticleSdkTypes.h"

#include "ParticleSubsystem.generated.h"

class UParticleWebBrowserWrapper;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLogin, const FString&, JsonString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSign, const FString&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignMessage, const FString&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignAndSendTransaction, const FString&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignTypedData, const FString&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignTransaction, const FString&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignAllTransactions, const FString&, Result);

UCLASS()
class UParticleSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UParticleSubsystem();

	// Begin USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem


	UFUNCTION(BlueprintCallable, Category = "Particle | Init", meta = (WorldContext = "WorldContextObject"))
	void Init(
		UObject* WorldContextObject,
		TSubclassOf<UUserWidget> DefaultBrowserWidget,
		FString InConfig,
		FString InTheme,
		FString InLanguage,
		FString InChainName,
		int64 InChainId
	);

	/**
	 * @brief Login
	 * @param PreferredAuthType you can pass "phone" or "email".
	 * @param Account you can pass phone number or email address.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | Login")
	void Login(const FString& PreferredAuthType, const FString& Account) const;

	UFUNCTION(BlueprintCallable, Category = "Particle | OnLogin")
	void OnLogin(const FString& JsonString);

	UFUNCTION(BlueprintCallable, Category = "Particle | OnSign")
	void OnSign(const FString& JsonString) const;

	/**
	 * @brief Sign message, support both evm and solana.
	 * @param Message in evm, request plain text, like "hello world", in solana, request base58 string.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | SignMessage")
	void SignMessage(const FString Message = "") const;

	/**
	 * @brief Sign and send transaction, support both evm and solana.
	 * @param Transaction in evm, request json string, in solana, request base58 string.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | SignAndSendTransaction")
	void SignAndSendTransaction(const FString& Transaction) const;

	/**
	 * @brief Sign typed data, only support evm.
	 * @param Message typed data string.
	 * @param Version support v1, v3, v4, default is v4.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | SignTypedData")
	void SignTypedData(const FString Message, const SignTypedDataVersion Version) const;

	/**
	 * @brief Sign transaction, only support solana.
	 * @param Transaction request base58 string.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | SignTrsnaction")
	void SignTransaction(const FString& Transaction);

	/**
	 * @brief Sign all transactions, only support solana.
	 * @param Transactions request base58 string array.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | SignAllTrsnactions")
	void SignAllTransactions(const TArray<FString> Transactions);
	
	UFUNCTION(BlueprintCallable, Category = "Particle | ClearCache")
	void ClearCache();

	UFUNCTION(BlueprintCallable, Category= "Particle | MakeEvmTransaction")
	FString MakeEvmTransaction(FString From, FString To, FString Data, FString Value) const;

	/**
	 * @brief Update chain inf
	 * @param InChainName Chain name, like "Ethereum", "Polygon", "Solana" and so on.
	 * @param InChainId  Chain id, like 1, 5, 137 and so on.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | UpdateChain")
	void UpdateChain(FString InChainName, int64 InChainId);

	/**
	 * @brief Get current chain name.
	 * @return current chain name.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | GetChainName")
	FString GetChainName();

	/**
	 * @brief Get current chain id.
	 * @return current chain id.
	 */
	UFUNCTION(BlueprintCallable, Category = "Particle | GetChainId")
	int64 GetChainId() const;

public:
	/**
	 * @brief OnLoginEvent, broadcast when get Login result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnLoginEvent")
	FOnLogin OnLoginEvent;

	/**
	 * @brief OnSignMessageEvent, broadcast when get SignMessage result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnSignMessageEvent")
	FOnSignMessage OnSignMessageEvent;

	/**
	 * @brief OnSignAndSendTransactionEvent, broadcast when get SignAndSendTransaction result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnSignAndSendTransactionEvent")
	FOnSignAndSendTransaction OnSignAndSendTransactionEvent;

	/**
	 * @brief OnSignTypedDataEvent, broadcast when get SignTypedDataEvent result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnSignTypedDataEvent")
	FOnSignTypedData OnSignTypedDataEvent;

	/**
	 * @brief OnSignTransactionEvent, broadcast when get SignTransactionEvent result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnSignTransactionEvent")
	FOnSignTransaction OnSignTransactionEvent;

	/**
	* @brief OnSignAllTransactionsEvent, broadcast when get SignAllTransactionsEvent result
	 */
	UPROPERTY(BlueprintAssignable, Category = "Particle | OnSignAllTransactionsEvent")
	FOnSignAllTransactions OnSignAllTransactionsEvent;

private:
	FString WalletURL;
	FString Config;
	FString Theme;
	FString Language;
	FString ChainName;
	int64 ChainId;

	UParticleWebBrowserWrapper* MyBrowser;

protected:

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> DefaultBrowserWidgetClass;
};
