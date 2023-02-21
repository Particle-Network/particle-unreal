# Particle UE SDK
Particle Unreal Engine SDK

## 🚀 Begin
try our demo first

## 📜 Docs
https://docs.particle.network/auth-service/sdks/unreal


## 🔠 Support method
```cpp
/**
 * @brief Login
 * @param PreferredAuthType you can pass "phone" or "email".
 * @param Account you can pass phone number or email address.
 */
UFUNCTION(BlueprintCallable, Category = "Particle | Login")
void Login(const FString& PreferredAuthType, const FString& Account) const;

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
    
```

## 💼 Feedback
Please report bugs or issues to [particle-unreal/issues](https://github.com/Particle-Network/particle-unreal/issues).

You can also join our [Discord](https://discord.gg/2y44qr6CR2).




