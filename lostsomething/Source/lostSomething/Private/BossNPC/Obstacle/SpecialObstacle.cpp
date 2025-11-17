// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Obstacle/SpecialObstacle.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include <Interface/LSTakeDamageInterface.h>
#include "Engine/DamageEvents.h"
#include <Components/CapsuleComponent.h>
#include "lostSomething.h"
#include <Interface/LSCharacterChoiceInterface.h>
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "BossNPC/Platform/PlatformGenerator.h"

// Sets default values
ASpecialObstacle::ASpecialObstacle()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));
	//CollisionComp->InitSphereRadius(16.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComp;

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASpecialObstacle::OnOverlapBegin);
	
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
	ObstacleMesh->SetupAttachment(CollisionComp);
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시 자체 충돌 제거

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Asset/Map/ModSubwayStation/StaticMeshes/SM_ScreenStanding.SM_ScreenStanding"));
	if (MeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(MeshAsset.Object);
	}

	if (MeshAsset.Succeeded())
	{
		ObstacleMesh->SetStaticMesh(MeshAsset.Object);
		ObstacleMesh->SetRelativeScale3D(FVector(1.0f));
		ObstacleMesh->SetRelativeLocation(FVector(-5.0f, 40.0f, -127.0f)); // Z축으로 50만큼 올림
		ObstacleMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatIJae(
		TEXT("Material'/Game/Asset/Asset/Texture/M_Alpha.M_Alpha'")
	);
	if (MatIJae.Succeeded())
	{
		Material_IJae = MatIJae.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatSiJae1(
		TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_Metal_Steel_Brushed.MI_Metal_Steel_Brushed'")
	);
	if (MatSiJae1.Succeeded())
	{
		Material1_SiJae = MatSiJae1.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatSiJae2(
		TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_Screen.MI_Screen'")
	);
	if (MatSiJae1.Succeeded())
	{
		Material2_SiJae = MatSiJae2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatSiJae3(
		TEXT("Material'/Game/Asset/Map/ModSubwayStation/Materials/MI_Emissive_Poster03.MI_Emissive_Poster03'")
	);
	if (MatSiJae3.Succeeded())
	{
		Material3_SiJae = MatSiJae3.Object;
	}

}

// Called when the game starts or when spawned
void ASpecialObstacle::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.5f);

	// 시작 시 정면 방향을 기억 (Z는 고정)
	MoveDirection = GetActorForwardVector();

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
	SO_ChangeVisible();
	
}

void ASpecialObstacle::SO_ChangeVisible()
{
	ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	ILSCharacterChoiceInterface* LSCharacterChoice = Cast<ILSCharacterChoiceInterface>(LocalPlayer->GetPlayerController(GetWorld()));
	if (LSCharacterChoice)
	{
		FString EnumString = StaticEnum<ELSCharacterChoice>()->GetNameByValue(static_cast<int64>(LSCharacterChoice->GetCharacterChoice())).ToString();
		//LS_LOG(LogLS, Log, TEXT("Character Choice : %s"), *EnumString);
		if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::SiJae)
		{
			SO_SetVisibleSiJae();
		}
		else if (LSCharacterChoice->GetCharacterChoice() == ELSCharacterChoice::IJae)
		{
			SO_SetVisibleIJae();
		}
	}
	else
	{
		LS_LOG(LogLS, Error, TEXT("No Character Choice"));
	}
}

void ASpecialObstacle::SO_SetVisibleSiJae()
{
	if (Material1_SiJae && Material2_SiJae && Material3_SiJae)
	{
		ObstacleMesh->SetMaterial(0, Material1_SiJae);
		ObstacleMesh->SetMaterial(1, Material2_SiJae);
		ObstacleMesh->SetMaterial(2, Material3_SiJae);
		//LS_LOG(LogLS, Log, TEXT("SetVisibleSiJae() : Material applied"));
	}
}

void ASpecialObstacle::SO_SetVisibleIJae()
{
	if (Material_IJae)
	{
		ObstacleMesh->SetMaterial(0, Material_IJae);
		ObstacleMesh->SetMaterial(1, Material_IJae);
		ObstacleMesh->SetMaterial(2, Material_IJae);
		//LS_LOG(LogLS, Log, TEXT("SetVisibleIJae() : Material applied"));
	}
}

// Called every frame
void ASpecialObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + MoveDirection * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);

}

void ASpecialObstacle::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (OtherActor && OtherActor != this)
	{
		if (OtherActor && OtherActor != this)
		{
			ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OtherActor);
			if (HitResult) // 꼭 체크!
			{
				FDamageEvent DamageEvent;
				HitResult->TakeDamage(AttackDamage, DamageEvent, GetInstigatorController(), this);
			}
		}
	}*/
	
}