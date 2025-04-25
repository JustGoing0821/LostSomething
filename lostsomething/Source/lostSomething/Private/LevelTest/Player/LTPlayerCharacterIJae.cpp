// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTest/Player/LTPlayerCharacterIJae.h"
#include "lostSomething.h"

ALTPlayerCharacterIJae::ALTPlayerCharacterIJae()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}
}

void ALTPlayerCharacterIJae::PossessedBy(AController* NewController)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	Super::PossessedBy(NewController);

	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("End"));
}
