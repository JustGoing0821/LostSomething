// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/SpecialTile.h"
#include "lostSomething.h"
#include <Interface/LSCharacterChoiceInterface.h>

// Sets default values
ASpecialTile::ASpecialTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

}

// Called when the game starts or when spawned
void ASpecialTile::BeginPlay()
{
	Super::BeginPlay();
	
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
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_FloorTile.MI_FloorTile'"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("SetVisibleSiJae() : Material"));
	}
}

void ASpecialTile::SetVisibleIJae()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_Emissive_Green.MI_Emissive_Green"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("SetVisibleIJae() : Material"));
	}
}


// Called every frame
void ASpecialTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

