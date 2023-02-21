// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ParticleWebBrowserWrapper.generated.h"

/**
 *
 */

DECLARE_DELEGATE_OneParam(FOnBrowserClosed, const bool);
DECLARE_DELEGATE(FOnLoadCompleted);
DECLARE_DELEGATE(FOnLoadError);

UCLASS()
class UParticleWebBrowserWrapper : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Particle | Browser")
		void LoadUrl(const FString& Url);

	UFUNCTION(BlueprintCallable, Category = "Particle | Browser")
		void BrowserClosed(const bool FromUser);

	UFUNCTION(BlueprintCallable, Category = "Particle | Browser")
		void LoadCompleted();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Particle | Browser")
		void ExecuteJavascript(const FString& ScriptText);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Particle | Browser")
		void ClearCache();

	FOnBrowserClosed OnBrowserClosed;
	FOnLoadCompleted OnLoadCompleted;
	FOnLoadError OnLoadError;
};
