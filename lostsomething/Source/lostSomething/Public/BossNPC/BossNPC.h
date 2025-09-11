#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/LSTakeDamageInterface.h"
#include "AOE/ACircleAOE.h"
#include "Engine/TimerHandle.h"
#include "Net/UnrealNetwork.h"
#include "Obstacle/BossObstacle.h"
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

    void Despawn();
    UFUNCTION(Server, Reliable)
    void ServerDespawn();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDespawn();

    FORCEINLINE void SetPhaseStatus(bool PhaseStatus) { bIsPhaseChanging = PhaseStatus; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TArray<FVector> PlayerSpawnLocations;

    FORCEINLINE FVector GetPlayerSpawnLocation(int32 Index) const
    {
        return PlayerSpawnLocations.IsValidIndex(Index)
            ? PlayerSpawnLocations[Index]
            : FVector::ZeroVector;
    }

    void BMSoundPlay(const FString& SoundType);
    UFUNCTION(Server, Reliable)
    void ServerBSoundPlay(const FString& SoundType);
    UFUNCTION(NetMulticast, Reliable)
    void MultiBMSoundPlay(const FString& SoundType);

    UPROPERTY(EditAnywhere)
    USoundWave* DieSound;

    UPROPERTY(EditAnywhere)
    USoundWave* DamageSound;

    UPROPERTY(EditAnywhere)
    USoundWave* Phase1Sound;

    UPROPERTY(EditAnywhere)
    USoundWave* Phase2Sound;

    UPROPERTY(EditAnywhere)
    USoundWave* Phase3Sound;

protected:
    virtual void BeginPlay() override;

    float MaxHP = 120.0f;
    float CurrentHP;

    bool bIsPhaseChanging;

    void SpawnSingleAOE(FVector SpawnLocation, FString AOEType = TEXT("AOE"));
    UFUNCTION(Server, Reliable)
    void ServerSpawnSingleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("AOE"));
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnSingleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("AOE"));

    void SpawnSingleCircleAOE(FVector SpawnLocation, FString AOEType = TEXT("Circle AOE"));
    UFUNCTION(Server, Reliable)
    void ServerSpawnSingleCircleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Circle AOE"));
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnSingleCircleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Circle AOE"));

    void SpawnSingleShareAOE(FVector SpawnLocation, FString AOEType = TEXT("Share AOE"));
    UFUNCTION(Server, Reliable)
    void ServerSpawnSingleShareAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Share AOE"));
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnSingleShareAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Share AOE"));

    void SpawnSingleLargeCircleAOE(FVector SpawnLocation, FString AOEType = TEXT("Large Circle AOE"));
    UFUNCTION(Server, Reliable)
    void ServerSpawnSingleLargeCircleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Large Circle AOE"));
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnSingleLargeCircleAOE(FVector SpawnLocation, const FString& AOEType = TEXT("Large Circle AOE"));

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    int32 RandomAOECount = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    float PlayerTargetOffset_Min = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AOE", meta = (AllowPrivateAccess = "true"))
    float PlayerTargetOffset_Max = 200.0f;

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

    void DestroyObstacles();
    UFUNCTION(Server, Reliable)
    void ServerDestroyObstacles();
    UFUNCTION(NetMulticast, Reliable)
    void MultiDestroyObstacles();

    UPROPERTY()
    TArray<ABossObstacle*> SpawnedObstacles;

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
    FVector GetRandomLocationAroundBoss();

    FVector GetSafeRandomLocationAroundBoss();

    TArray<AActor*> GetNearbyPlayers(float MaxDistance = 800.0f);

    // 네트워크 RPC 함수들
    UFUNCTION(Server, Reliable)
    void ServerSpawnAOEAttack();
    UFUNCTION(NetMulticast, Reliable)
    void MultiSpawnAOEAttack();
};