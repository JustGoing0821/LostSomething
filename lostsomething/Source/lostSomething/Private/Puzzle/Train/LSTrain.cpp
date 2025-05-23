// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrain.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"
#include "Puzzle/Train/LSTrainSpawnGimmick.h"

ALSTrain::ALSTrain()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Replication
	bReplicates = true;

	// Train Trigger Section
	TrainTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TrainTrigger"));
	RootComponent = TrainTrigger;
	TrainTrigger->SetBoxExtent(FVector(1450.0f, 150.0f, 150.0f));
	TrainTrigger->SetCollisionProfileName(CPROFILE_LSPAWN);

	// Moving Location
	WaitLocation = FVector(700, 50, 300);
	LeaveLocation = FVector(700, 12050, 300);

	// Mesh Ref
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_ext_wall_100_01.SM_ext_wall_100_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorFrameMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_wall_doorway_01.SM_wall_doorway_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RoofMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_ext_roof_200_01.SM_ext_roof_200_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorLMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_door_wall_01.SM_door_wall_01"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorRMeshRef(TEXT("/Game/Asset/Map/CitySubwayTrainModuler/Meshes/Structure/SM_door_wall_02.SM_door_wall_02"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CrowdMeshRef(TEXT("/Game/Level/Puzzle/Train/SM_SM_Crowd.SM_SM_Crowd"));
	static ConstructorHelpers::FObjectFinder<UMaterial> CrowdMaterialRef(TEXT("/Game/Level/Puzzle/Train/MALE.MALE"));
	static FName CarNames[] = { TEXT("Car1") , TEXT("Car2"), TEXT("Car3"), TEXT("Car4"), TEXT("Car5"), TEXT("Car6") };

	// Car Generate
	FVector CarLocation = FVector(-1350, 150, -150);

	for (FName CarName : CarNames)
	{
		UStaticMeshComponent* Car = CreateDefaultSubobject<UStaticMeshComponent>(CarName);
		Car->SetupAttachment(RootComponent);
		Car->SetRelativeLocation(CarLocation);
		CarLocation += FVector(400, 0, 0);

		FName Wall1Name = *CarName.ToString().Append(TEXT("Wall1"));
		UStaticMeshComponent* Wall1 = CreateDefaultSubobject<UStaticMeshComponent>(Wall1Name);
		Wall1->SetupAttachment(Car);
		Wall1->SetStaticMesh(WallMeshRef.Object);

		FName Wall2Name = *CarName.ToString().Append(TEXT("Wall2"));
		UStaticMeshComponent* Wall2 = CreateDefaultSubobject<UStaticMeshComponent>(Wall2Name);
		Wall2->SetupAttachment(Car);
		Wall2->SetStaticMesh(DoorFrameMeshRef.Object);
		Wall2->SetRelativeLocation(FVector(160, 0, 0));
		Wall2->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		Wall2->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		FName Wall3Name = *CarName.ToString().Append(TEXT("Wall3"));
		UStaticMeshComponent* Wall3 = CreateDefaultSubobject<UStaticMeshComponent>(Wall3Name);
		Wall3->SetupAttachment(Car);
		Wall3->SetStaticMesh(WallMeshRef.Object);
		Wall3->SetRelativeLocation(FVector(270, 0, 0));

		FName Roof1Name = *CarName.ToString().Append(TEXT("Roof1"));
		UStaticMeshComponent* Roof1 = CreateDefaultSubobject<UStaticMeshComponent>(Roof1Name);
		Roof1->SetupAttachment(Car);
		Roof1->SetStaticMesh(RoofMeshRef.Object);
		Roof1->SetRelativeLocation(FVector(-100, -300, 0));

		FName Roof2Name = *CarName.ToString().Append(TEXT("Roof2"));
		UStaticMeshComponent* Roof2 = CreateDefaultSubobject<UStaticMeshComponent>(Roof2Name);
		Roof2->SetupAttachment(Car);
		Roof2->SetStaticMesh(RoofMeshRef.Object);
		Roof2->SetRelativeLocation(FVector(70, -300, 0));

		FName DoorLName = *CarName.ToString().Append(TEXT("DoorL"));
		UStaticMeshComponent* DoorL = CreateDefaultSubobject<UStaticMeshComponent>(DoorLName);
		DoorL->SetupAttachment(Car);
		DoorL->SetStaticMesh(DoorLMeshRef.Object);
		DoorL->SetRelativeLocation(FVector(85, 0, 0));
		DoorL->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		DoorL->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
		DoorLs.Add(DoorL);

		FName DoorRName = *CarName.ToString().Append(TEXT("DoorR"));
		UStaticMeshComponent* DoorR = CreateDefaultSubobject<UStaticMeshComponent>(DoorRName);
		DoorR->SetupAttachment(Car);
		DoorR->SetStaticMesh(DoorRMeshRef.Object);
		DoorR->SetRelativeLocation(FVector(85, 0, 0));
		DoorR->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
		DoorR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DoorRs.Add(DoorR);

		//FName GateTriggerName = *CarName.ToString().Append(TEXT("GateTrigger"));
		//UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(GateTriggerName);
		//GateTrigger->SetupAttachment(Car);
		//GateTrigger->SetBoxExtent(FVector(80.0f, 20.0f, 110.0f));
		//GateTrigger->SetRelativeLocation(FVector(85, 20, 110));
		//GateTrigger->SetCollisionProfileName(CPROFILE_LSTRIGGER);
		//GateTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALSTrain::OnGateTriggerBeginOverlap);
		//GateTrigger->ComponentTags.Add(GateTriggerName);
		//GateTriggers.Add(GateTrigger);

		FName CrowdName = *CarName.ToString().Append(TEXT("Crowd"));
		UStaticMeshComponent* Crowd = CreateDefaultSubobject<UStaticMeshComponent>(CrowdName);
		Crowd->SetupAttachment(Car);
		Crowd->SetStaticMesh(CrowdMeshRef.Object);
		Crowd->SetRelativeLocationAndRotation(FVector(80,-40, 0), FRotator(0,90,0));
		Crowd->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
		Crowd->SetVisibility(false);
		Crowd->SetMaterial(0, CrowdMaterialRef.Object);
		Crowds.Add(Crowd);

	}
}

void ALSTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTrainState== ETrainState::Comming)
	{
		CurrentAlpha += DeltaTime * LerpSpeed;
		CurrentAlpha = FMath::Clamp(CurrentAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, WaitLocation, CurrentAlpha);
		SetActorLocation(NewLocation);

		if (HasAuthority() && (CurrentAlpha==1.0f))
		{

			FTimerHandle Handle1;
			GetWorld()->GetTimerManager().SetTimer(Handle1, FTimerDelegate::CreateLambda([&]
				{
					OnTrainArrived.Broadcast();
					CurrentAlpha = 0.0f;
				}
			), 1, false, 1);

			//for (TObjectPtr<class UBoxComponent> GateTrigger : GateTriggers)
			//{
			//	FVector GateLocation = GateTrigger->GetComponentLocation();
			//	LS_LOG(LogLS, Log, TEXT("GateLocation : %f, %f, %f"), GateLocation.X, GateLocation.Y, GateLocation.Z);
			//}

			CurrentTrainState = ETrainState::Waiting;

			FTimerHandle Handle2;
			GetWorld()->GetTimerManager().SetTimer(Handle2, FTimerDelegate::CreateLambda([&]
				{
					CurrentTrainState = ETrainState::Leaving;
				}
			), 1.f, false, 6.0f);
		}
	}
	else if (CurrentTrainState == ETrainState::Waiting)
	{
	}
	else if (CurrentTrainState == ETrainState::Leaving)
	{
		CurrentAlpha += DeltaTime * LerpSpeed;
		CurrentAlpha = FMath::Clamp(CurrentAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, LeaveLocation, CurrentAlpha);
		SetActorLocation(NewLocation);
	}
}

void ALSTrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrain, CurrentTrainState);
}

void ALSTrain::BeginPlay()
{
	Super::BeginPlay();
}

void ALSTrain::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));
}

void ALSTrain::GateOpen()
{
	for (UStaticMeshComponent* DoorL : DoorLs)
	{
		DoorL->SetVisibility(false);
	}
	for (UStaticMeshComponent* DoorR : DoorRs)
	{
		DoorR->SetVisibility(false);
	}
	//GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		{
			//GateClose();
			MulticastGetOnPassengers();
			MulticastRPCGateClose();
		}
	), 1.f, false, 4.0f);
}

void ALSTrain::GateClose()
{
	for (UStaticMeshComponent* DoorL : DoorLs)
	{
		DoorL->SetVisibility(true);
	}
	for (UStaticMeshComponent* DoorR : DoorRs)
	{
		DoorR->SetVisibility(true);
	}
	//GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALSTrain::DelegateBind(ALSTrainSpawnGimmick* InGimmickClass)
{
	InGimmickClass->OnPuzzleCheck.AddUObject(this, &ALSTrain::PuzzleCheck);
}

void ALSTrain::PuzzleCheck(bool bCorrect, int32 InCorrectGate)
{
	//LS_LOG(LogLS, Log, TEXT("InCorrectGate : %d"), InCorrectGate);

	if (bCorrect)
	{
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("True"));
		MulticastRPCGateOpen();
		//GetOffPassengers(InCorrectGate - 1);
		MulticastGetOffPassengers(InCorrectGate - 1);
	}
	else
	{
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("False"));
		MulticastRPCGateOpen();
		//GetOffPassengers(-1);
		MulticastGetOffPassengers(-1);
	}
}

void ALSTrain::GetOffPassengers(int32 InCorrectGate)
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));

	for (int32 Num=0 ; Num< DoorLs.Num() ; Num++)
	{
		//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
		if (Num == InCorrectGate)
		{
			//LS_LOG(LogLS, Log, TEXT("Correct Gate"));
			continue;
		}
		else
		{
			Crowds[Num]->SetVisibility(true);
			Crowds[Num]->SetRelativeLocation(FVector(80, 80, 0));
			//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
		}
	}
}

void ALSTrain::GetOnPassengers()
{
	for (int32 Num = 0; Num < DoorLs.Num(); Num++)
	{
		Crowds[Num]->SetVisibility(false);
		Crowds[Num]->SetRelativeLocation(FVector(80, -40, 0));
		//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
	}
}

void ALSTrain::MulticastRPCGateOpen_Implementation()
{
	GateOpen();
}

void ALSTrain::MulticastRPCGateClose_Implementation()
{
	GateClose();
}

void ALSTrain::MulticastGetOffPassengers_Implementation(int32 InCorrectGate)
{
	GetOffPassengers(InCorrectGate);
}

void ALSTrain::MulticastGetOnPassengers_Implementation()
{
	GetOnPassengers();
}
