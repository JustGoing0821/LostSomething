// Fill out your copyright notice in the Description page of Project Settings.


#include "BossNPC/Platform/NormalTile.h"
#include <Interface/LSTakeDamageInterface.h>
#include "Engine/DamageEvents.h"
#include <Game/LSGameMode.h>
#include <BossNPC/BossNPC.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
ANormalTile::ANormalTile()
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

	ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/Asset/Asset/Texture/M_Alpha.M_Alpha'"));
	if (MaterialFinder.Succeeded())
	{
		MeshComp->SetMaterial(0, MaterialFinder.Object);
		//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("SetVisibleSiJae() : Material"));
	}

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 또는 QueryAndPhysics
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	MeshComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap); // Pawn과만 오버랩
	MeshComp->SetGenerateOverlapEvents(true);


}

// Called when the game starts or when spawned
void ANormalTile::BeginPlay()
{
	Super::BeginPlay();
	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ANormalTile::OnMeshBeginOverlap);
}

// Called every frame
void ANormalTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANormalTile::OnMeshBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor && OtherActor != this)
	{
		if (OtherActor && OtherActor != this)
		{
			ILSTakeDamageInterface* HitResult = Cast<ILSTakeDamageInterface>(OtherActor);
			if (HitResult) // 꼭 체크!
			{
				FDamageEvent DamageEvent;
				HitResult->TakeDamage(AttackDamage, DamageEvent, GetInstigatorController(), this);

				ABossNPC* BossNPC = Cast<ABossNPC>(
					UGameplayStatics::GetActorOfClass(GetWorld(), ABossNPC::StaticClass())
				);

				if (BossNPC)
				{
					FVector Loc1 = BossNPC->GetPlayerSpawnLocation(0);
					FVector Loc2 = BossNPC->GetPlayerSpawnLocation(1);

					if (auto gm = Cast<ALSGameMode>(GetWorld()->GetAuthGameMode()))
					{
						gm->TransferPlayerLocation(Loc1, Loc2);
					}
				}
			}
		}
	}
}

