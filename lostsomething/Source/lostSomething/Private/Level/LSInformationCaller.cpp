// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/LSInformationCaller.h"
#include "lostSomething.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Components/BoxComponent.h"
#include "Physics/LSCollisionProfile.h"
#include "Quest/LSQuestManager.h"
#include "Level/LSInformationData.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LS2DPuzzleGameModeInterface.h"
#include "Interface/LSCombineTutorialInterface.h"

// Sets default values
ALSInformationCaller::ALSInformationCaller()
{
	bIsWaitPlayer = false;
	PuzzleActivateEnum = ELSInteractionEnum::Quest0;

	//Collision
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CPROFILE_LSTRIGGER);
	TriggerBox->SetBoxExtent(FVector(100, 100, 100));
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ALSInformationCaller::OnTriggerBeginOverlap);

	//Script
	DataAssetName = FName(TEXT("LSInfoBase"));
}

void ALSInformationCaller::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}

// Called when the game starts or when spawned
void ALSInformationCaller::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		BindQuestChange();
	}
}

void ALSInformationCaller::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		//ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
		ILSCombineTutorialInterface* LSCharacter = Cast<ILSCombineTutorialInterface>(OtherActor);
		if (LSCharacter)
		{
			InformationActivate();
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ALSInformationCaller::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnInteractionChange.AddUObject(this, &ALSInformationCaller::OnQuestChange);
		}
	}
}

void ALSInformationCaller::InformationActivate()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ILS2DPuzzleGameModeInterface* GameMode = Cast<ILS2DPuzzleGameModeInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
	{
		GameMode->Start2DPuzzle(DataAssetName, FVector2D(0,0));
	}
}

void ALSInformationCaller::OnQuestChange(ELSInteractionEnum InQuestEnum)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	//FString EnumString = StaticEnum<ELSInteractionEnum>()->GetNameByValue(static_cast<int64>(InQuestEnum)).ToString();
	//LS_LOG(LogLS, Log, TEXT("InQuestEnum : %s"), *EnumString);

	if (InQuestEnum == PuzzleActivateEnum)
	{
		if (bIsWaitPlayer)
		{
			TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		else
		{
			InformationActivate();
		}
	}
}