#include "ParticleSDKTypes.h"


// Evm
const FString FSignMethod::Eth_SendTransaction = TEXT("eth_sendTransaction");
const FString FSignMethod::Eth_SignTypedData = TEXT("eth_signTypedData");
const FString FSignMethod::Eth_signTypedData_v1 = TEXT("eth_signTypedData_v1");
const FString FSignMethod::Eth_SignTypedData_v3 = TEXT("eth_signTypedData_v3");
const FString FSignMethod::Eth_SignTypedData_v4 = TEXT("eth_signTypedData_v4");
const FString FSignMethod::Personal_Sign = TEXT("personal_sign");

// Solana
const FString FSignMethod::SignTransaction = TEXT("signTransaction");
const FString FSignMethod::SignAndSendTransaction = TEXT("signAndSendTransaction");
const FString FSignMethod::SignMessage = TEXT("signMessage");
const FString FSignMethod::SignAllTransactions = TEXT("signAllTransactions");
