// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/VendingMachine/LSVendingMachineManager.h"
#include "lostSomething.h"
#include "Puzzle/VendingMachine/LSVendingMachine.h"
#include "Kismet/GameplayStatics.h"

ALSVendingMachineManager::ALSVendingMachineManager()
{
	CurrentPhase = ECurrentPhase::Phase1;
}

void ALSVendingMachineManager::BeginPlay()
{
	AActor* TargetActor = nullptr;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALSVendingMachine::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		int32 CurrentMachine = 0;
		for (AActor* FoundActor : FoundActors)
		{
			ALSVendingMachine* VendingMachine = Cast<ALSVendingMachine>(FoundActor);
			if (VendingMachine)
			{
				VendingMachines.Add(VendingMachine);
				VendingMachine->SetMachineNumber(CurrentMachine);
				VendingMachine->BindOnPhaseChanged(this);
				CurrentMachine++;
			}
		}
		LS_LOG(LogLS, Log, TEXT("VendingMachines Found : %d"), VendingMachines.Num());
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No ALSVendingMachine"));
	}

	if (HasAuthority())
	{
		StartPhase();
	}
}

void ALSVendingMachineManager::StartPhase()
{
	LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	int32 CurrentColor = -1;
	for (int32 Num=0 ; Num < 4; Num++)
	{
		int32 NewColor = FMath::RandRange(0, 2);
		while (NewColor == CurrentColor)
		{
			NewColor = FMath::RandRange(0, 2);
		}
		AnswerColors.Add(static_cast<EVendingMachineColor>(NewColor));
		CurrentColor = NewColor;
	}

	for (EVendingMachineColor Color : AnswerColors)
	{
		FString EnumString = StaticEnum<EVendingMachineColor>()->GetNameByValue(static_cast<int64>(Color)).ToString();
		LS_LOG(LogLS, Log, TEXT("EVendingMachineColor : %s"), *EnumString);
	}

	OnPhaseChanged.Broadcast(AnswerColors[0], FMath::RandRange(0, 5));
}
