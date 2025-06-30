// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/LSQuestLocationMark.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Quest/LSQuestManager.h"

// Sets default values
ALSQuestLocationMark::ALSQuestLocationMark()
{
    bReplicates = true;
}

// Called when the game starts or when spawned
void ALSQuestLocationMark::BeginPlay()
{
	Super::BeginPlay();


    if (HasAuthority())
    {
        ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GameModeQuest)
        {
            GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSQuestLocationMark::OnQuestChange);
        }
    }
}

void ALSQuestLocationMark::SpawnSystem()
{
    if (!LocationMark)
    {
        UE_LOG(LogTemp, Warning, TEXT("LocationMark is null!"));
        return;
    }

    // Get Actor Location & Rotation
    FVector ActorLocation = GetActorLocation();
    FRotator ActorRotation = GetActorRotation();

    // Spawn System at Location
    UNiagaraComponent* SpawnedSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        this,
        LocationMark,
        ActorLocation,
        ActorRotation,
        FVector(Scale), // Scale을 FVector로 변환
        true,  // Auto Destroy
        true,  // Auto Activate
        ENCPoolMethod::None,
        true   // Pre Cull Check
    );

    if (SpawnedSystem)
    {
        // Set Niagara Variable (Float) - Scale 설정
        SpawnedSystem->SetNiagaraVariableFloat(FString("Scale"), Scale);

        // Branch 노드 구현 - Change Color 체크
        if (bChangeColor)
        {
            // Set Niagara Variable (LinearColor) - MainColor 설정
            SpawnedSystem->SetNiagaraVariableLinearColor(FString("MainColor"), CustomColor);
        }
    }
}

void ALSQuestLocationMark::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
    if (InQuestEnum == PuzzleActivateEnum)
    {
        MulticastRPCPuzzleActivate();
    }
    else
    {
        MulticastRPCPuzzleDeactivate();
    }
}

void ALSQuestLocationMark::PuzzleActivate()
{
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
    if (LSCharacterChoice)
    {
        FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
        //LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
        if (LSCharacterChoice->GetCharacterChoice() != CharacterChoice && CharacterChoice != ELSCharacterChoice::None) return;
    }
    else
    {
        LS_LOG(LogLS, Error, TEXT("No Character Choice"));
        return;
    }

    // SpawnSystem 커스텀 이벤트 호출
    SpawnSystem();

    // Set Timer by Function Name 노드 구현
    if (bIsLooping)
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ALSQuestLocationMark::SpawnSystem, LoopDuration, true);
    }
}

void ALSQuestLocationMark::PuzzleDeactivate()
{
    ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
    if (LSCharacterChoice)
    {
        FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
        //LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
        if (LSCharacterChoice->GetCharacterChoice() != CharacterChoice && CharacterChoice != ELSCharacterChoice::None) return;
    }
    else
    {
        LS_LOG(LogLS, Error, TEXT("No Character Choice"));
        return;
    }

    if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}

void ALSQuestLocationMark::MulticastRPCPuzzleActivate_Implementation()
{
    PuzzleActivate();
}

void ALSQuestLocationMark::MulticastRPCPuzzleDeactivate_Implementation()
{
    PuzzleDeactivate();
}
