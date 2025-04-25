// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerCharacterSiJae.h"
#include "lostSomething.h"

ALTPlayerCharacterSiJae::ALTPlayerCharacterSiJae()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
}

void ALTPlayerCharacterSiJae::PossessedBy(AController* NewController)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	Super::PossessedBy(NewController);

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
}
