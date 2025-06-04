// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSAttackTutorial.h"
#include "lostSomething.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSQuestInterface.h"

ALSAttackTutorial::ALSAttackTutorial()
{
	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(50.0f, 40.0f, 100.0f));
	RootComponent = CollisionBox;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/SpendingMachines/SM_SpendingMachine_02.SM_SpendingMachine_02"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	//Material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMaterialRef(TEXT("/Game/Level/InteractionActor/Materials/M_Blue.M_Blue"));
	if (BlueMaterialRef.Object)
	{
		MeshComponent->SetMaterial(0, BlueMaterialRef.Object);
	}

	CorrectCauserCharacter = ELSCharacterChoice::None;
}

float ALSAttackTutorial::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ILSCharacterChoiceInterface* CauserController = Cast<ILSCharacterChoiceInterface>(EventInstigator);
	if (CauserController)
	{
		if (CauserController->GetCharacterChoice() == CorrectCauserCharacter)
		{
			if (HasAuthority())
			{
				QuestClear();
			}
			else
			{
				ServerRPCQuestClear();
			}
		}
		else
		{
			LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Not Correct Character"));
		}
	}

	return 0.0f;
}

void ALSAttackTutorial::QuestClear()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->QuestComplete();
		}
	}
}

void ALSAttackTutorial::ServerRPCQuestClear_Implementation()
{
	QuestClear();
}
