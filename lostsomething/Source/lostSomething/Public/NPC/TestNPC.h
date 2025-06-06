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
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // AI 관련 함수
    void AttackByAI(); // ITestNPCInterface
    void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished);

    void Damage();
    UFUNCTION(Server, Reliable)
    void ServerDamage();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDamage();

    void SetDespawn();

    UFUNCTION(Server, Reliable)
    void ServerDespawn();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDespawn();

    // 공격 관련 함수
    void ComboActionBegin();
    void ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded);
    void NotifyComboActionEnd();
    void NextComboCheck();

    UFUNCTION(Server, Reliable)
    void ServerAttack();
    UFUNCTION(NetMulticast, Reliable)
    void MultiAttack();

    // 몽타주 멈춤 함수

    void CheckShouldStopMontage();

    UFUNCTION(Server, Reliable)
    void ServerStopAttackMontage();

    UFUNCTION(NetMulticast, Reliable)
    void MultiStopAttackMontage();


    // 공격 콜리전 생성
    virtual void AttackHitCheck() override;       //ITestNPCAttackInterface

    UFUNCTION(Server, Reliable)
    void ServerAttackHitCheck();
    UFUNCTION(NetMulticast, Reliable)
    void MultiAttackHitCheck();

    // 공격 관련 변수
    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bIsAttacking;

    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bCanNextCombo;

    UPROPERTY(Replicated, Meta = (AllowPrivateAccess = true))
    bool bIsComboCheckWindowOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxComboDistance = 300.0f;

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
        bShouldChase = bNewShouldChase;
    }

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetMaxWalkSpeed(float NewSpeed);

    UPROPERTY()
    AActor* TargetActor;

    // AI 공격 완료 델리게이트
    FAICharacterAttackFinished OnAttackFinished;

    // Replicated 변수 할당
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    const float AttackRange = 40.0f;
    const float AttackRadius = 50.0f;
    const float AttackDamage = 30.0f;

};
