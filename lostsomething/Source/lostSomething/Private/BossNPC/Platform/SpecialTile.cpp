// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/SpecialTile.h"
#include "lostSomething.h"
#include <Interface/LSCharacterChoiceInterface.h>
#include "EngineUtils.h"

// Sets default values
ASpecialTile::ASpecialTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}

	MeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.125f));

	//생성자에서 머티리얼 미리 로딩
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatSiJae(TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_FloorTile.MI_FloorTile'"));
	if (MatSiJae.Succeeded())
	{
		Material_SiJae = MatSiJae.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatIJae(TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_Emissive_Green.MI_Emissive_Green'"));
	if (MatIJae.Succeeded())
	{
		Material_IJae = MatIJae.Object;
	}

}

// Called when the game starts or when spawned
void ASpecialTile::BeginPlay()
{
	Super::BeginPlay();
	//주로 BeginPlay에 구현
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
						//LS_LOG(LogLS, Log, TEXT("Owner Setted."));
						break;
					}
				}
			}
		), 1.0f, false, 2.0f);
	}

	ChangeVisible();
}

void ASpecialTile::ChangeVisible()
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
		if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			SetVisibleSiJae();
		}
		else if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			SetVisibleIJae();
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}
}

void ASpecialTile::SetVisibleSiJae()
{
	if (Material_SiJae)
	{
		MeshComp->SetMaterial(0, Material_SiJae);
		//LS_LOG(LogLS, Log, TEXT("SetVisibleSiJae() : Material applied"));
	}
}

void ASpecialTile::SetVisibleIJae()
{
	if (Material_IJae)
	{
		MeshComp->SetMaterial(0, Material_IJae);
		//LS_LOG(LogLS, Log, TEXT("SetVisibleIJae() : Material applied"));
	}
}


// Called every frame
void ASpecialTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

