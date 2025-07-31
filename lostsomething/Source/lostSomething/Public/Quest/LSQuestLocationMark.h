// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TimerHandle.h"
#include "Interaction/LSInteractionEnum.h"
#include "Character/Players/LSCharacterChoice.h"
#include "LSQuestLocationMark.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSQuestLocationMark : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALSQuestLocationMark();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TObjectPtr<class UNiagaraSystem> LocationMark;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    float LoopDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    bool bIsLooping = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    float Scale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    bool bChangeColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    FLinearColor CustomColor = FLinearColor::Red;

    // 타이머 핸들
    FTimerHandle SpawnTimerHandle;

    // 커스텀 이벤트 함수들
    UFUNCTION()
    void SpawnSystem();

    UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
    void PuzzleActivate();
    void PuzzleDeactivate();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    ELSInteractionEnum PuzzleActivateEnum = ELSInteractionEnum::Quest0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;

//RPC Section
public:
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRPCPuzzleActivate();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRPCPuzzleDeactivate();
};
