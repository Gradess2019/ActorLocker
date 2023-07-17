// Copyright 2023 Gradess Games. All Rights Reserved.


#include "ActorLockerPythonCommand.h"
#include "Subsystems/UnrealEditorSubsystem.h"

UWorld* UActorLockerPythonCommand::GetWorld() const
{
	return GEditor->GetEditorSubsystem<UUnrealEditorSubsystem>()->GetEditorWorld();
}

void UActorLockerPythonCommand::AddObjectToRoot()
{
	AddToRoot();
}

void UActorLockerPythonCommand::RemoveObjectFromRoot()
{
	RemoveFromRoot();
}

void UActorLockerPythonCommand::SetEditorTimer(UObject* Object, FString FunctionName, float Time, bool bLooping, float InitialStartDelay)
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(Object, FName(*FunctionName));
	GEditor->GetTimerManager()->SetTimer(TimerHandle, TimerDelegate, Time, bLooping, InitialStartDelay);
}


