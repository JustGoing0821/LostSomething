// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TimerHandle.h"
#include "Interaction/LSInteractionEnum.h"
#include "Character/Players/LSCharacterChoice.h"
#include "LSLevelEffect.generated.h"

UCLASS()
class LOSTSOMETHING_API ALSLevelEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
    ALSLevelEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
	TObjectPtr<class UNiagaraSystem> Effect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    float LoopDuration = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    bool bIsLooping = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    float Scale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    bool bChangeColor = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    FLinearColor CustomColor = FLinearColor::White;

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    ELSInteractionEnum PuzzleActivateEnum = ELSInteractionEnum::Quest0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    ELSCharacterChoice CharacterChoice = ELSCharacterChoice::None;

    //ArrowEffect
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    TObjectPtr<class UStaticMeshComponent> ArrowMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    TObjectPtr<class UMaterialInstanceDynamic> ArrowMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom")
    bool bArrowVisible = false;

//RPC Section
public:
    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRPCPuzzleActivate();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastRPCPuzzleDeactivate();
};
