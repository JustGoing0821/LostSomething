// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Players/LSPlayerCharacterLeeJae.h"
#include "lostSomething.h"
#include "GameFramework/CharacterMovementComponent.h"

ALSPlayerCharacterLeeJae::ALSPlayerCharacterLeeJae()
{
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));
    if (CharacterMeshRef.Object)
    {
        GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
    }

    GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}

void ALSPlayerCharacterLeeJae::PossessedBy(AController* NewController)
{
    LS_LOG(LogLS, Log, TEXT("%s"), TEXT("LeeJae Begin"));
    Super::PossessedBy(NewController);
    LS_LOG(LogLS, Log, TEXT("%s"), TEXT("LeeJae End"));
}