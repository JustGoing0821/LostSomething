// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/LSScriptTester.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Blueprint/UserWidget.h"
#include "Character/UI/LSScriptWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ALSScriptTester::ALSScriptTester()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = CollisionBox;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        MeshComponent->SetStaticMesh(CubeMeshAsset.Object);
        MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
        MeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> BlueMaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BlueMaterialAsset.Succeeded())
    {
        MeshComponent->SetMaterial(0, BlueMaterialAsset.Object);
    }

    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Players/UI/WBP_ScriptDialog.WBP_ScriptDialog_C"));
    if (WidgetClassFinder.Succeeded())
    {
        ScriptWidgetClass = WidgetClassFinder.Class;
    }
}

// Called when the game starts or when spawned
void ALSScriptTester::BeginPlay()
{
    Super::BeginPlay();

    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ALSScriptTester::OnOverlapBegin);

    if (!HasAuthority() || IsNetMode(NM_Standalone))
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && ScriptWidgetClass && !ScriptWidget)
        {
            ScriptWidget = CreateWidget<ULSScriptWidget>(PC, ScriptWidgetClass);
            LS_LOG(LogLS, Log, TEXT("Script widget created in BeginPlay"));
        }
    }
}

// Called every frame
void ALSScriptTester::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALSScriptTester::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (GetNetMode() == NM_DedicatedServer)
    {
        LS_LOG(LogLS, Log, TEXT("Dedicated Server: Overlap detected, skipping UI"));
        return;
    }

    APawn* OverlappingPawn = Cast<APawn>(OtherActor);
    if (!OverlappingPawn)
    {
        LS_LOG(LogLS, Warning, TEXT("Not a pawn that overlapped"));
        return;
    }

    APlayerController* PC = Cast<APlayerController>(OverlappingPawn->GetController());
    if (!PC)
    {
        LS_LOG(LogLS, Warning, TEXT("No player controller for the overlapping pawn"));
        return;
    }

    if (!PC->IsLocalPlayerController())
    {
        LS_LOG(LogLS, Warning, TEXT("Not a local player controller, skipping UI"));
        return;
    }

    LS_LOG(LogLS, Log, TEXT("Local player (or server player) entered trigger zone"));

    if (!ScriptWidget && ScriptWidgetClass)
    {
        ScriptWidget = CreateWidget<ULSScriptWidget>(PC, ScriptWidgetClass);
        LS_LOG(LogLS, Log, TEXT("Created script widget on overlap"));
    }

    if (ScriptWidget)
    {
        //ScriptWidget->SetScriptText(ScriptText);
        //ScriptWidget->AddToViewport();
        //ScriptWidget->ShowScriptWidget();
        LS_LOG(LogLS, Log, TEXT("Showing script: %s"), *ScriptText);
    }
    else
    {
        LS_LOG(LogLS, Error, TEXT("Failed to create or find script widget"));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            FString::Printf(TEXT("스크립트 위젯이 없음: %s"), *ScriptText));
    }
}