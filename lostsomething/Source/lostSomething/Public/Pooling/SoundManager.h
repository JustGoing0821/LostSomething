// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

class USoundBase;
class USoundCue;
class USoundWave;
class UAudioComponent;

UENUM()
enum class ESoundType : uint8
{
	SoundTypeEffect,
	SoundTypeNPC,
	SoundTypePlayer,
	SoundTypeMonster,
	SoundEnd
};

UENUM(BlueprintType)
enum class EEffectSound : uint8
{
	EffectSound1,
	EffectSound2,
	EffectSound3,
	EffectSound4,
	EffectSound5,
	EffectSound6,
	EffectSound7,
	EffectSound8,
	EffectSound9,
	EffectSoundEnd
};

UENUM()
enum class ENPCSound : uint8
{
	NPCSound1, // -> NPC Robot Default1
	NPCSound2, // -> Click NPC ButtonSound
	NPCSound3,
	NPCSound4,
	NPCSound5,
	NPCSound6,
	NPCSound7,
	NPCSound8,
	NPCSound9,
	NPCSoundEnd
};

UENUM()
enum class EPlayerSound : uint8
{
	PlayerSound1,
	PlayerSound2,
	PlayerSound3,
	PlayerSound4,
	PlayerSound5,
	PlayerSound6,
	PlayerSound7,
	PlayerSound8,
	PlayerSound9, // -> SystemCall Message
	PlayerSoundEnd
};

UENUM()
enum class EMonsterSound : uint8
{
	MonsterSound1,
	MonsterSound2,
	MonsterSound3,
	MonsterSound4,
	MonsterSound5,
	MonsterSound6,
	MonsterSound7,
	MonsterSound8,
	MonsterSound9, // -> Boss FloorAttack
	MonsterSoundEnd
};

USTRUCT()
struct FAudioSource
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	class UAudioComponent* Audio;

	ESoundType SoundType;
	uint8 SoundKey;

	FDelegateHandle Handle;

};


UCLASS()
class LOSTSOMETHING_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static ASoundManager* GetInstance(UWorld* World);
	static void DestroyInstance();

	void PlaySoundCue(USoundCue* SoundCue, FVector Location, EEffectSound EffectSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue(USoundCue* SoundCue, FVector Location, ENPCSound NPCSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue(USoundCue* SoundCue, FVector Location, EPlayerSound PlayerSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue(USoundCue* SoundCue, FVector Location, EMonsterSound MonsterSound, float Volume = 1.f, float Pitch = 1.f);

	void PlaySoundCue2D(USoundCue* SoundCue, EEffectSound EffectSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue2D(USoundCue* SoundCue, ENPCSound NPCSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue2D(USoundCue* SoundCue, EPlayerSound PlayerSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundCue2D(USoundCue* SoundCue, EMonsterSound MonsterSound, float Volume = 1.f, float Pitch = 1.f);

	void PlaySoundWave(USoundWave* SoundWave, EEffectSound EffectSound, FVector Location, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave(USoundWave* SoundWave, ENPCSound NPCSound, FVector Location, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave(USoundWave* SoundWave, EPlayerSound PlayerSound, FVector Location, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave(USoundWave* SoundWave, EMonsterSound MonsterSound, FVector Location, float Volume = 1.f, float Pitch = 1.f);

	void PlaySoundWave2D(USoundWave* SoundWave, EEffectSound EffectSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave2D(USoundWave* SoundWave, ENPCSound NPCSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave2D(USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume = 1.f, float Pitch = 1.f);
	void PlaySoundWave2D(USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume = 1.f, float Pitch = 1.f);

	UFUNCTION(BlueprintCallable)
	void PlayBGM(USoundBase* BGM, float Volume = 1.f);
	void StopBGM();

	void StopAllSound();
	void StopSoundType(ESoundType SoundType);

	void StopSound(EEffectSound EffectSound);
	void StopSound(ENPCSound NPCSound);
	void StopSound(EPlayerSound PlayerSound);
	void StopSound(EMonsterSound MonsterSound);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


private:
	ASoundManager();

	void AudioPlay(UAudioComponent* AudioComponent, USoundBase* SounBase, float Volume, float Pitch);

	UFUNCTION()
	void OnSoundFinishedEffect();

	void SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EEffectSound EffectSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, ENPCSound NPCSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EPlayerSound PlayerSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EMonsterSound MonsterSound, float Volume, float Pitch);

	void SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EEffectSound EffectSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, ENPCSound NPCSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume, float Pitch);
	void SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume, float Pitch);

	void PlayCue(USoundCue* SoundCue, EEffectSound EffectSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayCue(USoundCue* SoundCue, ENPCSound NPCSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayCue(USoundCue* SoundCue, EPlayerSound PlayerSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayCue(USoundCue* SoundCue, EMonsterSound MonsterSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));

	void PlayWave(USoundWave* SoundWave, EEffectSound EffectSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayWave(USoundWave* SoundWave, ENPCSound NPCSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayWave(USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));
	void PlayWave(USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume, float Pitch, bool IsSound2D, FVector Location = FVector(0.0, 0.0, 0.0));

	static ASoundManager* Instance;
	static FCriticalSection Mutex;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	int32 InitialPoolSize;

	UPROPERTY()
	TArray<USoundBase*> SoundPool;

	UPROPERTY()
	TArray<USoundBase*> CurrentSound;

	UPROPERTY(EditAnywhere)
	UAudioComponent* AudioBGM;

	UPROPERTY(EditAnywhere)
	USoundBase* CurrentBackgroundMusic;

	UPROPERTY()
	TMap<EEffectSound, UAudioComponent*> EffectSoundMap;

	UPROPERTY()
	TMap<ENPCSound, UAudioComponent*> NPCSoundMap;

	UPROPERTY()
	TMap<EPlayerSound, UAudioComponent*> PlayerSoundMap;

	UPROPERTY()
	TMap<EMonsterSound, UAudioComponent*> MonsterSoundMap;

	UPROPERTY()
	TMap<USoundCue*, UAudioComponent*> SoundCuePool;

	UPROPERTY()
	TMap<USoundWave*, UAudioComponent*> SoundWavePool;

};
