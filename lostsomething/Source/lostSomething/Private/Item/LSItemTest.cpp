// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/LSItemTest.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interaction/LSInteractionScriptData.h"
#include "Engine/AssetManager.h"
#include "LevelTest/Player/LTPlayerController.h"

// Sets default values
ALSItemTest::ALSItemTest()
{
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetCollisionProfileName(TEXT("LSItemTest"));
	CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent = CollisionBox;

	//Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	MeshComponent->SetRelativeLocation(FVector(-50.0f, -50.0f, -50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ItemMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	if (ItemMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(ItemMeshRef.Object);
	}

	//Script Asset
	ScriptAssetName = FName(TEXT("LSInteractionBase"));

	//Interaction
	CurrentQuest = ELSInteractionEnum::Quest1;
	bCanInteraction = false;
}

void ALSItemTest::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Script Section
	UAssetManager& Manager = UAssetManager::Get();

	TArray<FPrimaryAssetId> Assets;
	Manager.GetPrimaryAssetIdList(TEXT("LSScriptData"), Assets);
	ensure(0 < Assets.Num());

	if (0 < Assets.Num())
	{
		for (const FPrimaryAssetId& AssetId : Assets)
		{
			//Test Asset
			if (AssetId.PrimaryAssetName == ScriptAssetName)
			{
				FSoftObjectPtr AssetPtr(Manager.GetPrimaryAssetPath(AssetId));
				//LS_LOG(LogLS, Log, TEXT("Found TestItem at path: %s"), *AssetPtr.ToString());

				if (AssetPtr.IsPending())
				{
					AssetPtr.LoadSynchronous();
				}
				InteractionScriptData = Cast<ULSInteractionScriptData>(AssetPtr.Get());
				ensure(InteractionScriptData);
				bCanInteraction = true;
				break;
			}
		}
		if (!bCanInteraction)
		{
			LS_LOG(LogLS, Error, TEXT("Item Data Not Found"));
		}
	}
}

void ALSItemTest::BeginPlay()
{
	//Mesh Visible
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer)
	{
		ALTPlayerController* LSPlayer = Cast<ALTPlayerController>(LocalPlayer->GetPlayerController(GetWorld()));
		if (LSPlayer)
		{
			if (LSPlayer->CharacterChoice != ECharacterChoice::None)
			{
				FString EnumString = StaticEnum<ECharacterChoice>()->GetNameByValue(static_cast<int64>(LSPlayer->CharacterChoice)).ToString();
				LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
				if (LSPlayer->CharacterChoice == ECharacterChoice::IJae)
				{
					MeshComponent->SetVisibility(true);
				}
				else
				{
					MeshComponent->SetVisibility(false);
				}
			}
			else
			{
				LS_LOG(LogLS, Error, TEXT("CharacterChoice Not Found"));
			}

		}
		else
		{
			LS_LOG(LogLS, Error, TEXT("LocalPlayer Controller Not Found"));
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("LocalPlayer Not Found"));
	}
}

void ALSItemTest::InteractionProcess(APlayerController* InPlayerController)
{
	if (bCanInteraction)
	{
		//Test Log Code
		LS_LOG(LogLS, Log, TEXT("Item Getted."));
		const TArray<FString>& Scripts = InteractionScriptData->GetInteractionScripts(CurrentQuest);
		LS_LOG(LogLS, Log, TEXT("Interaction Script : %s"), *Scripts[FMath::RandRange(0, 2)]);
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("Can't InteractionProcess"));
	}
}
