// Copyright 2023 Gradess Games. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

class ISlateStyle;
class FSlateStyleSet;

/**
 * Actor Locker style that defines icon locations, style brushes, etc.
 */
class FActorLockerStyle
{
private:
	static TSharedPtr<FSlateStyleSet> StyleInstance;
	
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:
	static TSharedRef<FSlateStyleSet> Create();
};


