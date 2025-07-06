#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/LSTakeDamageInterface.h"
#include "AOE/ACircleAOE.h"
#include "Engine/TimerHandle.h"
#include "Net/UnrealNetwork.h"
#include "BossNPC.generated.h"

UCLASS()
class LOSTSOMETHING_API ABossNPC : public ACharacter, public ILSTakeDamageInterface
{
    GENERATED_BODY()

    public:
    ABossNPC();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obstacle")
    TArray<USceneComponent*> ObstacleSpawnPoints;

    void SetHP(float NewHP);
    FORCEINLINE float GetHP() const { return CurrentHP; }

    // 페이즈 진입 함수들
    UFUNCTION(BlueprintCallable, Category = "Boss Phase")
    void EnterPhase1();
    UFUNCTION(BlueprintCallable, Category = "Boss Phase")
    void EnterPhase2();
    UFUNCTION(BlueprintCallable, Category = "Boss Phase")
    void EnterPhase3();

    // 몽타주 시작 함수


    void AOEMontagePlay();
    UFUNCTION(Server, Reliable)
    void ServerAOEMontagePlay();
    UFUNCTION(NetMulticast, Reliable)
    void MultiAOEMontagePlay();

    void ObsMontagePlay();
    UFUNCTION(Server, Reliable)
    void ServerObsMontagePlay();
    UFUNCTION(NetMulticast, Reliable)
    void MultiObsMontagePlay();

    void MazeMontagePlay();
    UFUNCTION(Server, Reliable)
    void ServerMazeMontagePlay();
    UFUNCTION(NetMulticast, Reliable)
    void MultiMazeMontagePlay();

    void DamageMontagePlay();
    UFUNCTION(Server, Reliable)
    void ServerDamageMontagePlay();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDamageMontagePlay();

    void  DieMontagePlay();
    UFUNCTION(Server, Reliable)
    void ServerDieMontagePlay();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDieMontagePlay();

    FORCEINLINE void SetPhaseStatus(bool PhaseStatus) { bIsPhaseChanging = PhaseStatus; }

protected:
    virtual void BeginPlay() override;

    float MaxHP = 120.0f;
    float CurrentHP;

    bool bIsPhaseChanging;

    //AOE
    void SpawnSingleAOE(FVector SpawnLocation, FString AOEType = TEXT("AOE"));
    UFUNCTION(Server, Reliable)
    void ServerSpawnSingleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("AOE"));

    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnSingleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("AOE"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    int32 RandomAOECount = 2; // 추가 랜덤 AOE 개수

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    float PlayerTargetOffset_Min = 100.0f; // 플레이어 타겟팅 AOE 최소 거리

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    float PlayerTargetOffset_Max = 200.0f; // 플레이어 타겟팅 AOE 최대 거리

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    TSubclassOf<class ACircleAOE> CircleAOEClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    float AOEAttackInterval;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    int32 MaxAOECount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE")
    float AOESpawnRadius;

    // 현재 페이즈
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Boss")
    int32 CurrentPhase = 1;

    // 타이머 핸들들
    FTimerHandle AOEPatternTimerHandle;
    FTimerHandle ObstaclePatternTimerHandle;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // 장애물 관련
    void SpawnObstacles();
    UFUNCTION(Server, Reliable)
    void ServerSpawnObstacles();
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnObstacles();

    // 플랫폼 관련
    void SpawnPlatform();
    UFUNCTION(Server, Reliable)
    void ServerSpawnPlatform();

    UPROPERTY()
    class APlatformGenerator* PlatformGenerator;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<APlatformGenerator> PlatformGeneratorClass;

private:
    // AOE 관련 함수들
    void StartAOEAttackPattern();
    void SpawnAOEAttack();
    void StopAOEPattern();
    void StopObstaclePattern();
    //FVector GetRandomAOELocation();
    FVector GetRandomLocationAroundBoss();  // ← 이거 추가!

    // 네트워크 RPC 함수들
    UFUNCTION(Server, Reliable)
    void ServerSpawnAOEAttack();
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnAOEAttack();
};