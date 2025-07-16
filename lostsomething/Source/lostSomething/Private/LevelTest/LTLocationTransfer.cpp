// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/LTLocationTransfer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSLocationTransferInterface.h"

// Sets default values
ALTLocationTransfer::ALTLocationTransfer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALTLocationTransfer::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			if (HasAuthority())
			{
				ILSLocationTransferInterface* GameModeQuest = Cast<ILSLocationTransferInterface>(UGameplayStatics::GetGameMode(GetWorld()));
				if (GameModeQuest)
				{
					GameModeQuest->TransferPlayerLocation(FVector(670.f, 260.f, 0), FVector(670.f, 810.f, 0));
				}
			}
		}
	), 10.f, false);
}

// Called every frame
void ALTLocationTransfer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

