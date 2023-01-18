// Copyright 2023 Gradess Games. All Rights Reserved.

#include "ActorLockerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FActorLockerStyle::StyleInstance = nullptr;

void FActorLockerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FActorLockerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FActorLockerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FActorLockerStyle::Get()
{
	return *StyleInstance;
}

FName FActorLockerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SceneOutlinerStyle"));
	return StyleSetName;
}

TSharedRef<FSlateStyleSet> FActorLockerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ActorLockerStyle"));
	const auto Plugin = IPluginManager::Get().FindPlugin("ActorLocker");
	const auto ResourceDir = Plugin->GetBaseDir() / TEXT("Resources");
	
	Style->SetContentRoot(ResourceDir);

	Style->Set("SceneOutliner.Lock", new IMAGE_BRUSH(TEXT("Lock"), FVector2D(20.f)));
	Style->Set("SceneOutliner.Lock.Small", new IMAGE_BRUSH(TEXT("Lock"), FVector2D(16.f)));
	Style->Set("SceneOutliner.Unlock", new IMAGE_BRUSH(TEXT("Unlock"), FVector2D(20.f)));
	Style->Set("SceneOutliner.Unlock.Small", new IMAGE_BRUSH(TEXT("Unlock"), FVector2D(16.f)));

	return Style;
}

#undef IMAGE_BRUSH