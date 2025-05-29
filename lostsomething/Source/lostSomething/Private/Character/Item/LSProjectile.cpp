// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Item/LSProjectile.h"
#include "Engine/DamageEvents.h"
#include "lostSomething.h" 

// Sets default values
ALSProjectile::ALSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 충돌 컴포넌트 설정
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(15.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentHit.AddDynamic(this, &ALSProjectile::OnHit);

    // 루트 컴포넌트로 설정
    RootComponent = CollisionComponent;


    // 프로젝타일 메시 설정
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 프로젝타일 무브먼트 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 800.0f;
    ProjectileMovement->MaxSpeed = 800.0f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f;

   
   
    //n초 후 자동 제거
    InitialLifeSpan = 5.0f;

    LS_LOG(LogLS, Log, TEXT("ALSProjectile: Constructor called"));

}

// Called when the game starts or when spawned
void ALSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        LS_LOG(LogLS, Warning, TEXT("ALSProjectile: Hit detected with %s"), *OtherActor->GetName());

        // 데미지 인터페이스 확인
        ILSTakeDamageInterface* DamageInterface = Cast<ILSTakeDamageInterface>(OtherActor);
        if (DamageInterface)
        {
            FDamageEvent DamageEvent;
            DamageInterface->TakeDamage(Damage, DamageEvent, GetInstigatorController(), this);

            LS_LOG(LogLS, Warning, TEXT("ALSProjectile: Applied %.1f damage to %s"), Damage, *OtherActor->GetName());
        }
        else
        {
            LS_LOG(LogLS, Warning, TEXT("ALSProjectile: Actor %s does not implement LSTakeDamageInterface"), *OtherActor->GetName());
        }

        // 프로젝타일 제거
        LS_LOG(LogLS, Log, TEXT("ALSProjectile: Destroying projectile after hit"));
        Destroy();
    }
}

void ALSProjectile::FireInDirection(const FVector& Direction)
{
    ProjectileMovement->Velocity = Direction * InitialSpeed;
    LS_LOG(LogLS, Log, TEXT("ALSProjectile: Fired in direction %s with speed %.1f"), *Direction.ToString(), InitialSpeed);
}