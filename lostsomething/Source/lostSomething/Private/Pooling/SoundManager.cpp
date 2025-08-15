// Fill out your copyright notice in the Description page of Project Settings.


#include "Pooling/SoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"

ASoundManager* ASoundManager::Instance = nullptr;
FCriticalSection ASoundManager::Mutex;
//FEvent* ASoundManager::LockEvent = FPlatformProcess::CreateSynchEvent( true );

ASoundManager::ASoundManager()
{
	PrimaryActorTick.bCanEverTick = false;
	//InitialPoolSize = 10;

	AudioBGM = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio BGM"));
}

void ASoundManager::AudioPlay(UAudioComponent* AudioComponent, USoundBase* SounBase, float Volume, float Pitch)
{
	AudioComponent->SetSound(SounBase);
	AudioComponent->SetVolumeMultiplier(Volume);
	AudioComponent->SetPitchMultiplier(Pitch);
	AudioComponent->Play();
}


void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

	// FFileHelper 클래스를 이용하여 로그 파일 생성
	FString FilePath = FPaths::ProjectLogDir() + TEXT("LogFileName.log");
	FFileHelper::SaveStringToFile(L"ASoundManager::BeginPlay -> Start", *FilePath, FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(), ELogVerbosity::Log);

	PlayBGM(CurrentBackgroundMusic);


	FFileHelper::SaveStringToFile(L"ASoundManager::BeginPlay -> End", *FilePath, FFileHelper::EEncodingOptions::AutoDetect,
		&IFileManager::Get(), ELogVerbosity::Log);

}

void ASoundManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 게임이 종료될 때 사운드 매니저 인스턴스를 삭제
	ASoundManager::DestroyInstance();
}

ASoundManager* ASoundManager::GetInstance(UWorld* World)
{
	if (!Instance)
	{
		Instance = World->SpawnActor<ASoundManager>();
	}

	return Instance;
}

void ASoundManager::DestroyInstance()
{
	if (Instance)
	{
		Instance->RemoveFromRoot(); // GC 해제
		Instance->Destroy();
		Instance = nullptr;
	}
}

void ASoundManager::PlaySoundCue(USoundCue* SoundCue, FVector Location, EEffectSound EffectSound, float Volume, float Pitch)
{
	if (SoundCue)
	{
		PlayCue(SoundCue, EffectSound, Volume, Pitch, false, Location);
	}
}

void ASoundManager::PlaySoundCue(USoundCue* SoundCue, FVector Location, ENPCSound NPCSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, NPCSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundCue(USoundCue* SoundCue, FVector Location, EPlayerSound PlayerSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, PlayerSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundCue(USoundCue* SoundCue, FVector Location, EMonsterSound MonsterSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, MonsterSound, Volume, Pitch, true);
}



void ASoundManager::PlaySoundCue2D(USoundCue* SoundCue, EEffectSound EffectSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, EffectSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundCue2D(USoundCue* SoundCue, ENPCSound NPCSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, NPCSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundCue2D(USoundCue* SoundCue, EPlayerSound PlayerSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, PlayerSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundCue2D(USoundCue* SoundCue, EMonsterSound MonsterSound, float Volume, float Pitch)
{
	if (SoundCue)
		PlayCue(SoundCue, MonsterSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundWave(USoundWave* SoundWave, EEffectSound EffectSound, FVector Location, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, EffectSound, Volume, Pitch, true, Location);
}

void ASoundManager::PlaySoundWave(USoundWave* SoundWave, ENPCSound NPCSound, FVector Location, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, NPCSound, Volume, Pitch, true, Location);
}

void ASoundManager::PlaySoundWave(USoundWave* SoundWave, EPlayerSound PlayerSound, FVector Location, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, PlayerSound, Volume, Pitch, true, Location);
}

void ASoundManager::PlaySoundWave(USoundWave* SoundWave, EMonsterSound MonsterSound, FVector Location, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, MonsterSound, Volume, Pitch, true, Location);
}

void ASoundManager::PlaySoundWave2D(USoundWave* SoundWave, EEffectSound EffectSound, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, EffectSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundWave2D(USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, PlayerSound, Volume, Pitch, true);
}

void ASoundManager::PlaySoundWave2D(USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, MonsterSound, Volume, Pitch, true);
}


void ASoundManager::PlaySoundWave2D(USoundWave* SoundWave, ENPCSound NPCSound, float Volume, float Pitch)
{
	if (SoundWave)
		PlayWave(SoundWave, NPCSound, Volume, Pitch, true);
}

void ASoundManager::PlayBGM(USoundBase* BGM, float Volume)
{
	if (CurrentBackgroundMusic)
		StopBGM();

	CurrentBackgroundMusic = BGM;

	AudioBGM->SetVolumeMultiplier(Volume);
	AudioBGM->SetSound(CurrentBackgroundMusic);
	AudioBGM->Play();
}


void ASoundManager::StopBGM()
{
	if (CurrentBackgroundMusic)
	{
		AudioBGM->Stop();
		AudioBGM->SetSound(nullptr);
	}
}

void ASoundManager::StopAllSound()
{
	StopSoundType(ESoundType::SoundTypeEffect);
	StopSoundType(ESoundType::SoundTypeNPC);
	StopSoundType(ESoundType::SoundTypePlayer);
	StopSoundType(ESoundType::SoundTypeMonster);
}

void ASoundManager::StopSoundType(ESoundType SoundType)
{

	switch (SoundType)
	{

	case ESoundType::SoundTypeEffect:
	{
		for (auto AudioComponent : EffectSoundMap)
		{
			if (AudioComponent.Value)
				AudioComponent.Value->Stop();
		}

		EffectSoundMap.Empty();
	}
	break;
	case ESoundType::SoundTypeNPC:
	{
		for (auto AudioComponent : NPCSoundMap)
		{
			if (AudioComponent.Value)
				AudioComponent.Value->Stop();
		}

		NPCSoundMap.Empty();
	}
	break;
	case ESoundType::SoundTypePlayer:
	{
		for (auto AudioComponent : PlayerSoundMap)
		{
			if (AudioComponent.Value)
				AudioComponent.Value->Stop();
		}

		PlayerSoundMap.Empty();
	}
	break;
	case ESoundType::SoundTypeMonster:
	{
		for (auto AudioComponent : MonsterSoundMap)
		{
			if (AudioComponent.Value)
				AudioComponent.Value->Stop();
		}

		MonsterSoundMap.Empty();
	}
	break;

	}
}

void ASoundManager::StopSound(EEffectSound EffectSound)
{
	UAudioComponent** AudioComponent = EffectSoundMap.Find(EffectSound);

	if (AudioComponent)
	{
		if ((*AudioComponent))
		{
			(*AudioComponent)->Stop();
			EffectSoundMap.Remove(EffectSound);
		}

	}
}

void ASoundManager::StopSound(ENPCSound NPCSound)
{
	UAudioComponent** AudioComponent = NPCSoundMap.Find(NPCSound);

	if (AudioComponent)
	{
		if ((*AudioComponent))
		{
			(*AudioComponent)->Stop();
			NPCSoundMap.Remove(NPCSound);
		}

	}
}

void ASoundManager::StopSound(EPlayerSound PlayerSound)
{
	UAudioComponent** AudioComponent = PlayerSoundMap.Find(PlayerSound);

	if (AudioComponent)
	{
		if ((*AudioComponent))
		{
			(*AudioComponent)->Stop();
			PlayerSoundMap.Remove(PlayerSound);
		}

	}
}

void ASoundManager::StopSound(EMonsterSound MonsterSound)
{
	UAudioComponent** AudioComponent = MonsterSoundMap.Find(MonsterSound);

	if (AudioComponent)
	{
		if ((*AudioComponent))
		{
			(*AudioComponent)->Stop();
			MonsterSoundMap.Remove(MonsterSound);
		}

	}
}


void ASoundManager::OnSoundFinishedEffect()
{
	for (auto& iter : EffectSoundMap)
	{
		UAudioComponent* Audio = iter.Value;

		if (!Audio->IsPlaying())
		{
			EffectSoundMap.Remove(iter.Key);
			return;
		}

	}
}


void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EEffectSound EffectSound, float Volume, float Pitch)
{

	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (EffectSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = EffectSoundMap.Find(EffectSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			EffectSoundMap[EffectSound] = AudioComponent;
		}
		else
			EffectSoundMap.Add(EffectSound, AudioComponent);
	}
	else
		EffectSoundMap.Add(EffectSound, AudioComponent);

	AudioPlay(AudioComponent, SoundCue, Volume, Pitch);

	//AudioComponent->OnAudioFinished.AddDynamic( this , &ASoundManager::OnSoundFinishedEffect );
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, ENPCSound NPCSound, float Volume,
	float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (NPCSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = NPCSoundMap.Find(NPCSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			NPCSoundMap[NPCSound] = AudioComponent;
		}
		else
			NPCSoundMap.Add(NPCSound, AudioComponent);
	}
	else
		NPCSoundMap.Add(NPCSound, AudioComponent);

	AudioPlay(AudioComponent, SoundCue, Volume, Pitch);
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EPlayerSound PlayerSound,
	float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (PlayerSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = PlayerSoundMap.Find(PlayerSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			PlayerSoundMap[PlayerSound] = AudioComponent;
		}
		else
			PlayerSoundMap.Add(PlayerSound, AudioComponent);
	}
	else
		PlayerSoundMap.Add(PlayerSound, AudioComponent);

	AudioPlay(AudioComponent, SoundCue, Volume, Pitch);
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundCue* SoundCue, EMonsterSound MonsterSound,
	float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (MonsterSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = MonsterSoundMap.Find(MonsterSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			MonsterSoundMap[MonsterSound] = AudioComponent;
		}
		else
			MonsterSoundMap.Add(MonsterSound, AudioComponent);
	}
	else
		MonsterSoundMap.Add(MonsterSound, AudioComponent);

	AudioPlay(AudioComponent, SoundCue, Volume, Pitch);
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EEffectSound EffectSound,
	float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (EffectSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = EffectSoundMap.Find(EffectSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			EffectSoundMap[EffectSound] = AudioComponent;
		}
		else
			EffectSoundMap.Add(EffectSound, AudioComponent);
	}
	else
		EffectSoundMap.Add(EffectSound, AudioComponent);

	AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, ENPCSound NPCSound, float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (NPCSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = NPCSoundMap.Find(NPCSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			NPCSoundMap[NPCSound] = AudioComponent;
		}
		else
			NPCSoundMap.Add(NPCSound, AudioComponent);
	}
	else
		NPCSoundMap.Add(NPCSound, AudioComponent);

	AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
}
void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (PlayerSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = PlayerSoundMap.Find(PlayerSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			PlayerSoundMap[PlayerSound] = AudioComponent;
		}
		else
			PlayerSoundMap.Add(PlayerSound, AudioComponent);
	}
	else
		PlayerSoundMap.Add(PlayerSound, AudioComponent);

	AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
}

void ASoundManager::SetupSound(UAudioComponent* AudioComponent, USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume, float Pitch)
{
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASoundManager::SetupSound -> AudioComponent nullptr"));
		return;
	}

	if (MonsterSoundMap.Num() > 0)
	{
		if (UAudioComponent** PlayedAudioComponent = MonsterSoundMap.Find(MonsterSound))
		{
			if ((*PlayedAudioComponent))
				(*PlayedAudioComponent)->Stop();

			MonsterSoundMap[MonsterSound] = AudioComponent;
		}
		else
			MonsterSoundMap.Add(MonsterSound, AudioComponent);
	}
	else
		MonsterSoundMap.Add(MonsterSound, AudioComponent);

	AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
}



void ASoundManager::PlayCue(USoundCue* SoundCue, EEffectSound EffectSound, float Volume, float Pitch, bool IsSound2D, FVector Location)
{
	if (SoundCue)
	{
		UAudioComponent** AudioComponent = SoundCuePool.Find(SoundCue);

		if (AudioComponent)
		{
			if (*AudioComponent)
				SetupSound(*AudioComponent, SoundCue, EffectSound, Volume, Pitch);
		}
		else
		{
			IsSound2D
				? *AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundCue, Volume, Pitch)
				: *AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundCue, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(*AudioComponent, SoundCue, EffectSound, Volume, Pitch);
				SoundCuePool.Add(SoundCue, *AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayCue(USoundCue* SoundCue, ENPCSound NPCSound, float Volume, float Pitch, bool IsSound2D,
	FVector Location)
{
	if (SoundCue)
	{
		UAudioComponent** AudioComponent = SoundCuePool.Find(SoundCue);

		if (AudioComponent)
		{
			if (*AudioComponent)
				SetupSound(*AudioComponent, SoundCue, NPCSound, Volume, Pitch);
		}
		else
		{
			IsSound2D
				? *AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundCue, Volume, Pitch)
				: *AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundCue, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(*AudioComponent, SoundCue, NPCSound, Volume, Pitch);
				SoundCuePool.Add(SoundCue, *AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayCue(USoundCue* SoundCue, EPlayerSound PlayerSound, float Volume, float Pitch, bool IsSound2D,
	FVector Location)
{
	if (SoundCue)
	{
		UAudioComponent** AudioComponent = SoundCuePool.Find(SoundCue);

		if (AudioComponent)
		{
			if (*AudioComponent)
				SetupSound(*AudioComponent, SoundCue, PlayerSound, Volume, Pitch);
		}
		else
		{
			IsSound2D
				? *AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundCue, Volume, Pitch)
				: *AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundCue, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(*AudioComponent, SoundCue, PlayerSound, Volume, Pitch);
				SoundCuePool.Add(SoundCue, *AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayCue(USoundCue* SoundCue, EMonsterSound MonsterSound, float Volume, float Pitch, bool IsSound2D,
	FVector Location)
{
	if (SoundCue)
	{
		UAudioComponent** AudioComponent = SoundCuePool.Find(SoundCue);

		if (AudioComponent)
		{
			if (*AudioComponent)
				SetupSound(*AudioComponent, SoundCue, MonsterSound, Volume, Pitch);
		}
		else
		{
			IsSound2D
				? *AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundCue, Volume, Pitch)
				: *AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundCue, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(*AudioComponent, SoundCue, MonsterSound, Volume, Pitch);
				SoundCuePool.Add(SoundCue, *AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayWave(USoundWave* SoundWave, EEffectSound EffectSound, float Volume, float Pitch, bool IsSound2D,
	FVector Location)
{
	if (SoundWave)
	{

		UAudioComponent** FindWavePool = SoundWavePool.Find(SoundWave);
		UAudioComponent* AudioComponent = nullptr;

		if (FindWavePool)
		{
			AudioComponent = *FindWavePool;

			if (AudioComponent)
			{
				AudioComponent->Stop();
				SetupSound(AudioComponent, SoundWave, EffectSound, Volume, Pitch);
				AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
			}
			else
			{
				IsSound2D
					? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
					: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

				if (AudioComponent)
				{
					SetupSound(AudioComponent, SoundWave, EffectSound, Volume, Pitch);
					SoundWavePool.Add(SoundWave, AudioComponent);
				}
			}
		}
		else
		{

			IsSound2D
				? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
				: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(AudioComponent, SoundWave, EffectSound, Volume, Pitch);
				SoundWavePool.Add(SoundWave, AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayWave(USoundWave* SoundWave, EPlayerSound PlayerSound, float Volume, float Pitch, bool IsSound2D,
	FVector Location)
{
	if (SoundWave)
	{

		UAudioComponent** FindWavePool = SoundWavePool.Find(SoundWave);
		UAudioComponent* AudioComponent = nullptr;

		if (FindWavePool)
		{
			AudioComponent = *FindWavePool;

			if (AudioComponent)
			{
				AudioComponent->Stop();
				SetupSound(AudioComponent, SoundWave, PlayerSound, Volume, Pitch);
				AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
			}
			else
			{
				IsSound2D
					? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
					: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

				if (AudioComponent)
				{
					SetupSound(AudioComponent, SoundWave, PlayerSound, Volume, Pitch);
					SoundWavePool.Add(SoundWave, AudioComponent);
				}
			}

		}
		else
		{

			IsSound2D
				? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
				: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(AudioComponent, SoundWave, PlayerSound, Volume, Pitch);
				SoundWavePool.Add(SoundWave, AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayWave(USoundWave* SoundWave, EMonsterSound MonsterSound, float Volume, float Pitch,
	bool IsSound2D, FVector Location)
{
	if (SoundWave)
	{

		UAudioComponent** FindWavePool = SoundWavePool.Find(SoundWave);
		UAudioComponent* AudioComponent = nullptr;

		if (FindWavePool)
		{
			AudioComponent = *FindWavePool;

			if (AudioComponent)
			{
				AudioComponent->Stop();
				SetupSound(AudioComponent, SoundWave, MonsterSound, Volume, Pitch);
				AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
			}
			else
			{
				IsSound2D
					? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
					: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

				if (AudioComponent)
				{
					SetupSound(AudioComponent, SoundWave, MonsterSound, Volume, Pitch);
					SoundWavePool.Add(SoundWave, AudioComponent);
				}
			}

		}
		else
		{

			IsSound2D
				? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
				: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(AudioComponent, SoundWave, MonsterSound, Volume, Pitch);
				SoundWavePool.Add(SoundWave, AudioComponent);
			}
		}
	}
}

void ASoundManager::PlayWave(USoundWave* SoundWave, ENPCSound NPCSound, float Volume, float Pitch,
	bool IsSound2D, FVector Location)
{
	if (SoundWave)
	{

		UAudioComponent** FindWavePool = SoundWavePool.Find(SoundWave);
		UAudioComponent* AudioComponent = nullptr;

		if (FindWavePool)
		{
			AudioComponent = *FindWavePool;

			if (AudioComponent)
			{
				AudioComponent->Stop();
				SetupSound(AudioComponent, SoundWave, NPCSound, Volume, Pitch);
				AudioPlay(AudioComponent, SoundWave, Volume, Pitch);
			}
			else
			{
				IsSound2D
					? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
					: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

				if (AudioComponent)
				{
					SetupSound(AudioComponent, SoundWave, NPCSound, Volume, Pitch);
					SoundWavePool.Add(SoundWave, AudioComponent);
				}
			}

		}
		else
		{
			IsSound2D
				? AudioComponent = UGameplayStatics::SpawnSound2D(this, SoundWave, Volume, Pitch)
				: AudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, SoundWave, Location, FRotator::ZeroRotator, Volume, Pitch);

			if (AudioComponent)
			{
				SetupSound(AudioComponent, SoundWave, NPCSound, Volume, Pitch);
				SoundWavePool.Add(SoundWave, AudioComponent);
			}
		}
	}
}
