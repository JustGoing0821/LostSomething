// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/TestNPCInterface.h"
#include "Interface/TestNPCAttackInterface.h"
#include "LevelTest/Interface/LTTakeDamageInterface.h"
#include "Interface/LSTakeDamageInterface.h"
#include "TestNPC.generated.h"


UCLASS()
class LOSTSOMETHING_API ATestNPC : public ACharacter, public ITestNPCInterface, public ITestNPCAttackInterface, public ILTTakeDamageInterface, public ILSTakeDamageInterface
{
    GENERATED_BODY()

public:
    ATestNPC();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


    //////////////////////////////////Attack Start
    void AttackStart(); // ITestNPCInterface

    // AI 공격 완료 델리게이트
    FAICharacterAttackFinished OnAttackFinished;

    void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished);

    void ComboActionBegin();
    void ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
    void NotifyComboActionEnd();
    void NextComboCheck();

    UFUNCTION(Server, Reliable)
    void ServerAttackStart();
    UFUNCTION(NetMulticast, Reliable)
    void MultiAttackStart();

    virtual void AttackHitCheck() override;       //ITestNPCAttackInterface

    UFUNCTION(Server, Reliable)
    void ServerAttackHitCheck();

    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bIsAttacking;

    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bCanNextCombo;

    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bIsComboCheckWindowOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxComboDistance = 300.0f;

    UPROPERTY()
    AActor* TargetActor;

    const float AttackRange = 40.0f;
    const float AttackRadius = 50.0f;
    const float AttackDamage = 30.0f;

    float MaxHP = 50.0f;
    float CurrentHP;

    FORCEINLINE void SetHP(float NewHP)
    {
        CurrentHP = NewHP;
    }
    FORCEINLINE float GetHP() const { return CurrentHP; }


    ////////////////////////////////////Damage
private:
    // Dynamic Material Instance
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    // 타이머 핸들
    FTimerHandle DamageFlashTimerHandle;
    FTimerHandle FlashUpdateTimerHandle;
    float FlashStartTime = 0.0f;

    // 데미지 플래시 지속 시간
    UPROPERTY(EditAnywhere, Category = "Damage")
    float DamageFlashDuration = 1.0f;

public:
    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bIsDead = false;

    // Getter
    UFUNCTION(BlueprintCallable, Category = "AI")
    FORCEINLINE bool GetIsDead()
    {
        return bIsDead;
    }

    // Setter (인라인)
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetIsDead(bool NewDeadStatus)
    {
        bIsDead = NewDeadStatus;
    }

    void Damage();
    UFUNCTION(Server, Reliable)
    void ServerDamage();

    UFUNCTION(NetMulticast, Reliable)
    void MultiDamage();

    void SetbIsHit();
    UFUNCTION(Server, Reliable)
    void ServerSetbIsHit();

    // 빨간색으로 변경 (모든 클라이언트)
    UFUNCTION(Server, Reliable)
    void ServerFlashDamageColor();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastFlashDamageColor();

    UFUNCTION()
    void UpdateFlashEffect_Timer();

    UFUNCTION(Server, Reliable)
    void ServerUpdateFlashEffect_Timer();

    UFUNCTION(NetMulticast, Reliable)
    void MultiUpdateFlashEffect_Timer();


    ////////////////////////////////////Die
    void SetDespawn();
    UFUNCTION(Server, Reliable)
    void ServerDespawn();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDespawn();


    ////////////////////////////////////Montage
    void CheckShouldStopMontage();
    UFUNCTION(Server, Reliable)
    void ServerStopAttackMontage();
    UFUNCTION(NetMulticast, Reliable)
    void MultiStopAttackMontage();


    ////////////////////////////////////////////Chase
    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bShouldChase = false;

    // Getter
    UFUNCTION(BlueprintCallable, Category = "AI")
    FORCEINLINE bool GetShouldChase()
    {
        return bShouldChase;
    }

    // Setter (인라인)
    UFUNCTION(BlueprintCallable, Category = "AI")
    void SetShouldChase(bool bNewShouldChase)
    {
        if (bNewShouldChase && bSoundPlay)
        {
            TMSoundPlay("Chase");
            bSoundPlay = true;
        }
        else {
            bSoundPlay = false;
        }

        bShouldChase = bNewShouldChase;
    }

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetMaxWalkSpeed(float NewSpeed);

    // Replicated 변수 할당
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
 

    ////////////////////////////////////// Sound

    bool bSoundPlay = false;

    UPROPERTY(EditAnywhere)
    USoundWave* IdleSound;

    UPROPERTY(EditAnywhere)
    USoundWave* AttackSound;

    UPROPERTY(EditAnywhere)
    USoundWave* DamageSound;

    UPROPERTY(EditAnywhere)
    USoundWave* ChaseSound;

    UPROPERTY(EditAnywhere)
    USoundWave* WalkSound;

    UPROPERTY(EditAnywhere)
    USoundWave* RunSound;

    void TMSoundPlay(const FString& SoundType);
    UFUNCTION(Server, Reliable)
    void ServerTMSoundPlay(const FString& SoundType);
    UFUNCTION(NetMulticast, Reliable)
    void MultiTMSoundPlay(const FString& SoundType);
};
