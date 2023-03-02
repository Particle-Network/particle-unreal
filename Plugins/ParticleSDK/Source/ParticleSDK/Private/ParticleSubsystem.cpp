// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticleSubsystem.h"
#include "JsonObjectConverter.h"
#include "ParticleWebBrowserWrapper.h"
#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

UParticleSubsystem::UParticleSubsystem()
	: UGameInstanceSubsystem()
{
}

void UParticleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UParticleSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UParticleSubsystem::Init(
	UObject* WorldContextObject,
	TSubclassOf<UUserWidget> DefaultBrowserWidget,
	FString InConfig,
	FString InTheme,
	FString InLanguage,
	FString InChainName,
	int64 InChainId
)
{
	Config = InConfig;
	Theme = InTheme;
	ChainName = InChainName;
	ChainId = InChainId;
	Language = InLanguage;

	UE_LOG(LogTemp, Log, TEXT("Particle SDK init, Config = %s, ChainName = %s, ChainId = %lld, Language = %s, Theme = %s"),
	       *Config, *ChainName, ChainId, *Language, *Theme);

	this->DefaultBrowserWidgetClass = DefaultBrowserWidget;
	
	WalletURL = "https://auth-bridge.particle.network/";
	
	UE_LOG(LogTemp, Log, TEXT("Particle subsystem initialized Url=%s"), *WalletURL);

	if (MyBrowser == nullptr || !MyBrowser->IsValidLowLevel() || !MyBrowser->GetIsEnabled())
	{
		MyBrowser = CreateWidget<UParticleWebBrowserWrapper>(WorldContextObject->GetWorld(), DefaultBrowserWidget);
		MyBrowser->AddToViewport(100000);

		MyBrowser->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UParticleSubsystem::UpdateChain(FString InChainName, int64 InChainId)
{
	ChainName = InChainName;
	ChainId = InChainId;
}

FString UParticleSubsystem::GetChainName()
{
	return ChainName;
}

int64 UParticleSubsystem::GetChainId() const
{
	return ChainId;
}

void UParticleSubsystem::Login(const FString& PreferredAuthType, const FString& Account) const
{
	UE_LOG(LogTemp, Log, TEXT("Particle login"));
	if (MyBrowser == nullptr)
		return;

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&preferredAuthType=%s&account=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *PreferredAuthType, *Account);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "login";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle login URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}

void UParticleSubsystem::OnLogin(const FString& JsonString)
{
	if (MyBrowser == nullptr)
		return;

	OnLoginEvent.Broadcast(JsonString);
	MyBrowser->BrowserClosed(true);

	MyBrowser->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Log, TEXT("Particle OnLogin JsonString = %s"), *JsonString);
}

void UParticleSubsystem::SignMessage(const FString Message) const
{
	UE_LOG(LogTemp, Log, TEXT("Particle SignMessage"));
	if (MyBrowser == nullptr)
		return;

	FString Method;
	if (ChainName.ToLower() == "solana" && (ChainId == 101 || ChainId == 102 || ChainId == 103))
	{
		Method = FSignMethod::SignMessage;
	}
	else
	{
		Method = FSignMethod::Personal_Sign;
	}

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&method=%s&message=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *Method, *Message);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "sign";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle SignMessage URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}

void UParticleSubsystem::OnSign(const FString& JsonString) const
{
	if (MyBrowser == nullptr)
		return;
	FOnSignResult Result;
	FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &Result, 0, 0);
	
	if (Result.Method == FSignMethod::Personal_Sign || Result.Method == FSignMethod::SignMessage)
	{
		OnSignMessageEvent.Broadcast(JsonString);
	}
	else if (Result.Method == FSignMethod::Eth_SignTypedData
		|| Result.Method == FSignMethod::Eth_signTypedData_v1
		|| Result.Method == FSignMethod::Eth_SignTypedData_v3
		|| Result.Method == FSignMethod::Eth_SignTypedData_v4)
	{
		OnSignTypedDataEvent.Broadcast(JsonString);
	}
	else if (Result.Method == FSignMethod::Eth_SendTransaction || Result.Method == FSignMethod::SignAndSendTransaction)
	{
		OnSignAndSendTransactionEvent.Broadcast(JsonString);
	}
	else if (Result.Method == FSignMethod::SignTransaction)
	{
		OnSignTransactionEvent.Broadcast(JsonString);
	}
	else if (Result.Method == FSignMethod::SignAllTransactions)
	{
		OnSignAllTransactionsEvent.Broadcast(JsonString);
	}

	MyBrowser->BrowserClosed(true);

	MyBrowser->SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Log, TEXT("Signature:%s"), *JsonString);
}


void UParticleSubsystem::SignAndSendTransaction(const FString& Transaction) const
{
	UE_LOG(LogTemp, Log, TEXT("Particle SignAndSendTransaction"));
	if (MyBrowser == nullptr)
		return;

	FString Method;
	if (ChainName.ToLower() == "solana" && (ChainId == 101 || ChainId == 102 || ChainId == 103))
	{
		Method = FSignMethod::SignAndSendTransaction;
	}
	else
	{
		Method = FSignMethod::Eth_SendTransaction;
	}

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&method=%s&message=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *Method, *Transaction);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "sign";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle SignAndSendTransaction URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}


void UParticleSubsystem::SignTypedData(const FString Message, const SignTypedDataVersion Version) const
{
	UE_LOG(LogTemp, Log, TEXT("Particle SignTypedData"));
	if (MyBrowser == nullptr)
		return;
	FString Method;
	switch (Version)
	{
	case SignTypedDataVersion::Default:
		Method = FSignMethod::Eth_SignTypedData;
		break;
	case SignTypedDataVersion::V1:
		Method = FSignMethod::Eth_signTypedData_v1;
		break;
	case SignTypedDataVersion::V3:
		Method = FSignMethod::Eth_SignTypedData_v3;
		break;
	case SignTypedDataVersion::V4:
		Method = FSignMethod::Eth_SignTypedData_v4;
		break;
	}

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&method=%s&message=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *Method, *Message);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "sign";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle SignTypedData URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}

void UParticleSubsystem::SignTransaction(const FString& Transaction)
{
	UE_LOG(LogTemp, Log, TEXT("Particle SignTransaction"));
	if (MyBrowser == nullptr)
		return;
	const FString Method = FSignMethod::SignTransaction;

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&method=%s&message=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *Method, *Transaction);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "sign";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle SignTransaction URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}


void UParticleSubsystem::SignAllTransactions(const TArray<FString> Transactions)
{
	UE_LOG(LogTemp, Log, TEXT("Particle SignAllTransactions"));
	if (MyBrowser == nullptr)
		return;
	const FString Method = FSignMethod::SignAllTransactions;

	FString Message;
	TArray<TSharedPtr<FJsonValue>> Items;
	for (int i = 0; i < Transactions.Num(); ++i)
	{
		Items.Add(MakeShareable(new FJsonValueString(Transactions[i])));
	}

	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Message);
	FJsonSerializer::Serialize(Items, Writer);

	const FString QueryStr = FString::Printf(
		TEXT("config=%s&theme=%s&language=%s&chainName=%s&chainId=%lld&method=%s&message=%s"),
		*Config, *Theme, *Language, *ChainName, ChainId, *Method, *Message);
	const FString Temp = FGenericPlatformHttp::UrlEncode(QueryStr);
	const FString Path = "sign";
	const FString URI = FString::Printf(TEXT("%s%s?%s"), *WalletURL, *Path, *Temp);

	UE_LOG(LogTemp, Log, TEXT("Particle SignAllTransactions URI = %s"), *URI);

	MyBrowser->LoadUrl(URI);

	MyBrowser->SetVisibility(ESlateVisibility::Visible);
}


void UParticleSubsystem::ClearCache()
{
	if (MyBrowser == nullptr)
		return;
	MyBrowser->ClearCache();
}


FString UParticleSubsystem::MakeEvmTransaction(FString From, FString To, FString Data, FString Value) const
{
	const TSharedPtr<FJsonObject> TransactionJson = MakeShareable(new FJsonObject());
	TransactionJson->SetStringField(TEXT("from"), From);
	TransactionJson->SetStringField(TEXT("to"), To);
	TransactionJson->SetStringField(TEXT("value"), Value);
	TransactionJson->SetStringField(TEXT("data"), Data);
	TransactionJson->SetNumberField(TEXT("chainId"), ChainId);

	FString Content;

	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(TransactionJson.ToSharedRef(), Writer);

	return Content;
}
