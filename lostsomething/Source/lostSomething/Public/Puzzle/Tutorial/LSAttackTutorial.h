// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/LSInteractionActorBase.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Character/Players/LSCharacterChoice.h"
#include "Interaction/LSInteractionEnum.h"
#include "LSAttackTutorial.generated.h"

DECLARE_DELEGATE(FOnAttackTutorialDelegate);

/**
 * 
 */
UCLASS()
class LOSTSOMETHING_API ALSAttackTutorial : public ALSInteractionActorBase, public ILSTakeDamageInterface
{
	GENERATED_BODY()
	
public:
	ALSAttackTutorial();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Puzzle)
	ELSCharacterChoice CorrectCauserCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Puzzle, Meta = (AllowPrivateAccess = "true"))
	ELSInteractionEnum PuzzleActivateEnum;
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FOnAttackTutorialDelegate OnAttackTutorial;

protected:
	void BindQuestChange();
	UFUNCTION()
	void OnQuestChange(struct FLSQuestData InQuestData, enum ELSInteractionEnum InQuestEnum);
	void PuzzleActivate();
	void PuzzleDeactivate();

	virtual void InteractionProcess(APlayerController* InPlayerController) override;


//Effect
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	TObjectPtr<class UNiagaraSystem> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
	float LoopDuration = 1.0f;

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


//RPC
public:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleActivate();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCPuzzleDeactivate();

	UFUNCTION(Server, Unreliable)
	void ServerRPCPuzzleDeactivate();
};
