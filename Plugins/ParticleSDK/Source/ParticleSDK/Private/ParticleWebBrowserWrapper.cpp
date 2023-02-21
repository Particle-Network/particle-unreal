// Fill out your copyright notice in the Description page of Project Settings.


#include "ParticleWebBrowserWrapper.h"

void UParticleWebBrowserWrapper::BrowserClosed(const bool FromUser)
{
	OnBrowserClosed.ExecuteIfBound(FromUser);
}

void UParticleWebBrowserWrapper::LoadCompleted()
{
	OnLoadCompleted.ExecuteIfBound();
}