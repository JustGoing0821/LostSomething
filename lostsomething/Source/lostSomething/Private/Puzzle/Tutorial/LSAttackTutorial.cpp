// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Tutorial/LSAttackTutorial.h"
#include "lostSomething.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/LSCharacterChoiceInterface.h"
#include "Interface/LSQuestInterface.h"
#include "Quest/LSQuestManager.h"

ALSAttackTutorial::ALSAttackTutorial()
{
	//Collision
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(CPROFILE_LSINTERACTIONACTOR);
	CollisionBox->SetBoxExtent(FVector(50.0f, 40.0f, 100.0f));
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	RootComponent = CollisionBox;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Game/Asset/Map/ModSubwayStation/StaticMeshes/SM_TrashCan.SM_TrashCan"));
	if (MeshRef.Object)
	{
		MeshComponent->SetStaticMesh(MeshRef.Object);
	}

	bReplicates = true;
	CorrectCauserCharacter = ELSCharacterChoice::None;
	PuzzleActivateEnum = ELSInteractionEnum::Quest3;
}

void ALSAttackTutorial::BeginPlay()
{
	if (HasAuthority())
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
				{
					if (PlayerController && !PlayerController->IsLocalController())
					{
						SetOwner(PlayerController);
						break;
					}
				}
			}
		), 1.0f, false, 2.0f);
	}

	if (HasAuthority())
	{
		BindQuestChange();
	}
}

float ALSAttackTutorial::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	ILSCharacterChoiceInterface* CauserController = Cast<ILSCharacterChoiceInterface>(EventInstigator);
	if (CauserController)
	{
		if (CauserController->GetCharacterChoice() == CorrectCauserCharacter)
		{
			OnAttackTutorial.Execute();
			if (HasAuthority())
			{
				MulticastRPCPuzzleDeactivate();
			}
			else
			{
				ServerRPCPuzzleDeactivate();
			}
		}
		else
		{
			LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Not Correct Character"));
		}
	}

	return 0.0f;
}

void ALSAttackTutorial::BindQuestChange()
{
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSAttackTutorial::OnQuestChange);
		}
	}
}

void ALSAttackTutorial::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
	}
}

void ALSAttackTutorial::PuzzleActivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetVisibility(true);
}

void ALSAttackTutorial::PuzzleDeactivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
}

void ALSAttackTutorial::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSAttackTutorial::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}

void ALSAttackTutorial::ServerRPCPuzzleDeactivate_Implementation()
{
	MulticastRPCPuzzleDeactivate();
}
