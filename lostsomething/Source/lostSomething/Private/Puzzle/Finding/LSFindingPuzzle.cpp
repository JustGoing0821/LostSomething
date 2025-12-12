// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Finding/LSFindingPuzzle.h"
#include "lostSomething.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Quest/LSQuestManager.h"
#include "Physics/LSCollisionProfile.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Interface/LSQuestInterface.h"
#include "Interface/LSScriptWidgetInterface.h"
#include "Interface/LSTakeDamageInterface.h"
#include "Interface/LSScriptWidgetInterface.h"


ALSFindingPuzzle::ALSFindingPuzzle()
{
	bReplicates = true;

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
	//MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Game/Asset/Map/MetroPack/Objects/Cartels/SM_Cartel_02.SM_Cartel_02"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	bIsCorrectPuzzle = false;
	PuzzleActivateEnum = ELSInteractionEnum::Quest11;
	DamageAmount = 10.f;

	//Script Asset
	ScriptAssetNameSiJae = FName(TEXT("LSFindingPuzzleSiJae"));
	ScriptAssetNameIJae = FName(TEXT("LSFindingPuzzleIJae"));

	bISTexturesUpdated = false;
}

void ALSFindingPuzzle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSFindingPuzzle, bIsCorrectPuzzle);
	DOREPLIFETIME(ALSFindingPuzzle, CurrentInteractController);
}

void ALSFindingPuzzle::BeginPlay()
{
	Super::BeginPlay();

	//LS_LOG(LogLSls, Log, TEXT("Begin"));

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
						//LS_LOG(LogLSls, Log, TEXT("Owner Setted."));
						break;
					}
				}
			}
		), 2.0f, false);
	}

	if (HasAuthority())
	{
		BindQuestChange();
	}

	UMaterial* BaseMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/Level/Display/Signage/Materials/M_LSCartel.M_LSCartel"));

	if (BaseMaterial && MeshComponent)
	{
		Material = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MeshComponent->SetMaterial(1, Material);

		//PanelMaterial->SetVectorParameterValue(TEXT("BaseColor"), Gray);
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("No BaseMaterial or Material!!"));
	}
}

void ALSFindingPuzzle::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	UAssetManager& Manager = UAssetManager::Get();

	//FPAnswer
	TArray<FPrimaryAssetId> FPAnswerAssets;
	Manager.GetPrimaryAssetIdList(TEXT("FPAnswer"), FPAnswerAssets);
	ensure(0 < FPAnswerAssets.Num());

	for (const FPrimaryAssetId& AssetId : FPAnswerAssets)
	{
		FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
		//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		UTexture2D* Texture  = Cast<UTexture2D>(AssetPtr.Get());
		if (Texture)
		{
			AnswerTextures.Add(Texture);
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("No AnswerTexture!!"));
		}
	}



	//FPWrong
	TArray<FPrimaryAssetId> FPWrongAssets;
	Manager.GetPrimaryAssetIdList(TEXT("FPWrong"), FPWrongAssets);
	ensure(0 < FPWrongAssets.Num());

	for (const FPrimaryAssetId& AssetId : FPWrongAssets)
	{
		FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
		//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		UTexture2D* Texture = Cast<UTexture2D>(AssetPtr.Get());
		if (Texture)
		{
			WrongTextures.Add(Texture);
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("No WrongTexture!!"));
		}

	}



	//FPSiJae
	TArray<FPrimaryAssetId> FPSiJaeAssets;
	Manager.GetPrimaryAssetIdList(TEXT("FPSiJae"), FPSiJaeAssets);
	ensure(0 < FPSiJaeAssets.Num());

	for (const FPrimaryAssetId& AssetId : FPSiJaeAssets)
	{
		FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
		//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

		if (AssetPtr.IsPending())
		{
			AssetPtr.LoadSynchronous();
		}

		UTexture2D* Texture = Cast<UTexture2D>(AssetPtr.Get());
		if (Texture)
		{
			SijaeTextures.Add(Texture);
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("No SijaeTexture!!"));
		}
	}

	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("End"));

	bISTexturesUpdated = true;
}

void ALSFindingPuzzle::InteractionProcess(APlayerController* InPlayerController)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	), 3.0f, false);

	if (HasAuthority())
	{
		SetCurrentInteractController(InPlayerController);
	}
	else
	{
		ServerRPCSetCurrentInteractController(InPlayerController);
	}

	Super::InteractionProcess(InPlayerController);
}

void ALSFindingPuzzle::InteractionProcessSiJae(APlayerController* InPlayerController)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		PuzzleCheck();
	}
	else
	{
		ServerRPCPuzzleCheck();
	}


	//Script
	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	FString Script = "";

	if (bIsCorrectPuzzle)
	{
		Script = InteractionScriptDataSiJae->GetInteractionScripts(PuzzleActivateEnum)[0];
	}
	else
	{
		Script = InteractionScriptDataSiJae->GetInteractionScripts(PuzzleActivateEnum)[1];
	}

	ScriptController->UpdateScriptWidget(Script);


	//Audio
	if (bIsCorrectPuzzle)
	{
		if (CorrectAudioComponent)
		{
			CorrectAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
	else
	{
		if (WrongAudioComponent)
		{
			WrongAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
}

void ALSFindingPuzzle::InteractionProcessIJae(APlayerController* InPlayerController)
{
	ILSScriptWidgetInterface* ScriptController = Cast<ILSScriptWidgetInterface>(InPlayerController);
	FString Script = "";

	if (bIsCorrectPuzzle)
	{
		Script = InteractionScriptDataIJae->GetInteractionScripts(PuzzleActivateEnum)[0];
	}
	else
	{
		Script = InteractionScriptDataIJae->GetInteractionScripts(PuzzleActivateEnum)[1];
	}

	ScriptController->UpdateScriptWidget(Script);


	//Audio
	if (bIsCorrectPuzzle)
	{
		if (BaseAudioComponent)
		{
			BaseAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
	else
	{
		if (BaseAudioComponent)
		{
			BaseAudioComponent->Play();
		}
		else
		{
			LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("No Audio Component!!"));
		}
	}
}

void ALSFindingPuzzle::SetVisibleSiJae()
{
	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	if (!Material)
	{
		//LS_LOG(LogLSls, Error, TEXT("Material is nullptr!"));
		return;
	}

	if (!bISTexturesUpdated)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("bISTexturesUpdated false"));
				SetVisibleSiJae();
			}
		), 2.0f, false);
	}

	int32 MaxCount = SijaeTextures.Num();
	Material->SetTextureParameterValue(TEXT("CartelImage"), SijaeTextures[FMath::RandRange(0, MaxCount - 1)]);
}

void ALSFindingPuzzle::SetVisibleIJae()
{
	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	if (!Material)
	{
		//LS_LOG(LogLSls, Error, TEXT("Material is nullptr!"));
		return;
	}

	if (!bISTexturesUpdated)
	{
		FTimerHandle Handle;
		GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
			{
				LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("bISTexturesUpdated false"));
				SetVisibleSiJae();
			}
		), 2.0f, false);
	}

	if (bIsCorrectPuzzle)
	{
		int32 MaxCount = AnswerTextures.Num();
		Material->SetTextureParameterValue(TEXT("CartelImage"), AnswerTextures[FMath::RandRange(0, MaxCount-1)]);
	}
	else
	{
		int32 MaxCount = WrongTextures.Num();
		Material->SetTextureParameterValue(TEXT("CartelImage"), WrongTextures[FMath::RandRange(0, MaxCount - 1)]);
	}
}

void ALSFindingPuzzle::SetPuzzleAnswer(uint8 bInCorrectPuzzle)
{
	LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (bInCorrectPuzzle)
	{
		bIsCorrectPuzzle = true;
	}
	else
	{
		bIsCorrectPuzzle = false;
	}

	MulticastRPCChangeVisible();
}

void ALSFindingPuzzle::PuzzleCheck()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (bIsCorrectPuzzle)
	{
		OnPuzzleCheck.ExecuteIfBound(true);
	}
	else
	{
		OnPuzzleCheck.ExecuteIfBound(false);
		ApplyDamage();
	}
}

void ALSFindingPuzzle::BindQuestChange()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	if (HasAuthority())
	{
		ILSQuestInterface* GameModeQuest = Cast<ILSQuestInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GameModeQuest)
		{
			GameModeQuest->GetQuestManager()->OnQuestStart.AddUObject(this, &ALSFindingPuzzle::OnQuestChange);
		}
	}
}

void ALSFindingPuzzle::OnQuestChange(FLSQuestData InQuestData, ELSInteractionEnum InQuestEnum)
{
	FString EnumString = StaticEnum<ELSInteractionEnum>()->GetNameByValue(static_cast<int64>(InQuestEnum)).ToString();
	LS_LOG(LogLSls, Log, TEXT("EnumValue : %s"), *EnumString);

	if (InQuestEnum == PuzzleActivateEnum)
	{
		MulticastRPCPuzzleActivate();
		MulticastRPCLSSetMapIcon(true);
	}
	else
	{
		MulticastRPCPuzzleDeactivate();
		MulticastRPCLSSetMapIcon(false);
	}
}

void ALSFindingPuzzle::PuzzleActivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ALSFindingPuzzle::PuzzleDeactivate()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (!Material)
	{
		LS_LOG(LogLSls, Error, TEXT("Material is nullptr!"));
		return;
	}

	if (SijaeTextures.Num() > 0)
	{
		int32 MaxCount = SijaeTextures.Num();
		Material->SetTextureParameterValue(TEXT("CartelImage"), SijaeTextures[FMath::RandRange(0, MaxCount - 1)]);
	}
	else
	{
		LS_LOG(LogLSls, Error, TEXT("No SijaeTextures!"));
	}

}

void ALSFindingPuzzle::ApplyDamage()
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));

	if (CurrentInteractController == nullptr)
	{
		LS_LOG(LogLSls, Error, TEXT("%s"), TEXT("CurrentInteractController is null!!"));
		return;
	}

	ILSTakeDamageInterface* LSPlayer = Cast<ILSTakeDamageInterface>(CurrentInteractController->GetPawn());
	if (LSPlayer)
	{
		FDamageEvent DamageEvent;
		LSPlayer->TakeDamage(DamageAmount, DamageEvent, nullptr, this);
	}
}

void ALSFindingPuzzle::MulticastRPCPuzzleActivate_Implementation()
{
	PuzzleActivate();
}

void ALSFindingPuzzle::MulticastRPCPuzzleDeactivate_Implementation()
{
	PuzzleDeactivate();
}

void ALSFindingPuzzle::ServerRPCSetCurrentInteractController_Implementation(APlayerController* InPlayerController)
{
	//LS_LOG(LogLSls, Log, TEXT("%s"), TEXT("Begin"));
	SetCurrentInteractController(InPlayerController);
}

void ALSFindingPuzzle::ServerRPCPuzzleCheck_Implementation()
{
	PuzzleCheck();
}



