#include "FParticleParams.h"

#include "JsonObjectConverter.h"


FString UParticleConfig::ToJsonString() const 
{
	const TSharedRef<FJsonObject> FooJsonObj = MakeShareable(new FJsonObject());
	FJsonObjectConverter::UStructToJsonObject(this->GetClass(), this, FooJsonObj, 0, 0);
	
	FString JsonStr;
	const TSharedRef<TJsonWriter<TCHAR> > JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonStr);
	FJsonSerializer::Serialize(FooJsonObj, JsonWriter);

	return JsonStr;
}



FString UParticleTheme::ToJsonString() const
{
	const TSharedRef<FJsonObject> FooJsonObj = MakeShareable(new FJsonObject());
	FJsonObjectConverter::UStructToJsonObject(this->GetClass(), this, FooJsonObj, 0, 0);
	
	FString JsonStr;
	const TSharedRef<TJsonWriter<TCHAR> > JsonWriter = TJsonWriterFactory<TCHAR>::Create(&JsonStr);
	FJsonSerializer::Serialize(FooJsonObj, JsonWriter);

	return JsonStr;
}

