// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/SpecialTile.h"
#include "lostSomething.h"
#include <Interface/LSCharacterChoiceInterface.h>
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "BossNPC/Platform/PlatformGenerator.h"

// Sets default values
ASpecialTile::ASpecialTile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_WorldStatic);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube")
	);
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}
	MeshComp->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.125f));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(MeshComp);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
	TriggerBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	TriggerBox->SetRelativeLocation(FVector::ZeroVector);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpecialTile::OnMeshBeginOverlap);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatSiJae(
		TEXT("Material'/Game/Asset/Asset/Texture/M_Alpha.M_Alpha'")
	);
	if (MatSiJae.Succeeded())
	{
		Material_SiJae = MatSiJae.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatIJae(
		TEXT("Material'/Game/Asset/Asset/Texture/M_StraightBlock.M_StraightBlock'")
	);
	if (MatIJae.Succeeded())
	{
		Material_IJae = MatIJae.Object;
	}

	bOverlap = false;
}

void ASpecialTile::Init(APlatformGenerator* Generator, int32 ColIndex)
{
	PlatformGenerator = Generator;
	ThisColumn = ColIndex;
	bOverlap = false;
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

void ASpecialTile::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LS_LOG(LogLS, Error,TEXT("ASpecialTile::OnMeshBeginOverlap"))
	if (PlatformGenerator && !bOverlap)
	{
		bOverlap = true;
		PlatformGenerator->OnSpecialTileStepped(ThisColumn);
	}
	//Destroy();
}


// Called every frame
void ASpecialTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

