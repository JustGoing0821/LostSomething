// Fill out your copyright notice in the Description page of Project Settings.


#include "Puzzle/Train/LSTrain.h"
#include "lostSomething.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Physics/LSCollisionProfile.h"

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

	//Floor
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoColision"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FloorMeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (FloorMeshRef.Object)
	{
		MeshComponent->SetStaticMesh(FloorMeshRef.Object);
	}
	MeshComponent->SetRelativeScale3D(FVector(25.f, 3.f, 0.1f));
	MeshComponent->SetRelativeLocation(FVector(-200.f, 0.f, -150.f));


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
		Car->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		DoorLs.Add(DoorL);

		FName DoorRName = *CarName.ToString().Append(TEXT("DoorR"));
		UStaticMeshComponent* DoorR = CreateDefaultSubobject<UStaticMeshComponent>(DoorRName);
		DoorR->SetupAttachment(Car);
		DoorR->SetStaticMesh(DoorRMeshRef.Object);
		DoorR->SetRelativeLocation(FVector(85, 0, 0));
		DoorR->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
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
		//Crowd->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Crowds.Add(Crowd);

	}

	TimeBeforeGateOpen = 1.0f;
	TimeTrainWait = 5.0f;
	TimeBeforeTrainLeave = 1.0f;
	bisPassengersGettingOff = false;
	GetOnLocation = FVector(80, -40, 0);
	GetOffLocation = FVector(80, 200, 0);
}

void ALSTrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentTrainState== ETrainState::Comming)
	{
		CurrentTrainAlpha += DeltaTime * LerpSpeed;
		CurrentTrainAlpha = FMath::Clamp(CurrentTrainAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, WaitLocation, CurrentTrainAlpha);
		SetActorLocation(NewLocation);

		if (HasAuthority() && (CurrentTrainAlpha ==1.0f))
		{
			CurrentTrainState = ETrainState::Waiting;

			if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
			{
				GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
			}
			GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
				{
					OnTrainArrived.Broadcast();
					CurrentTrainAlpha = 0.0f;
				}
			), TimeBeforeGateOpen, false);

			//for (TObjectPtr<class UBoxComponent> GateTrigger : GateTriggers)
			//{
			//	FVector GateLocation = GateTrigger->GetComponentLocation();
			//	LS_LOG(LogLS, Log, TEXT("GateLocation : %f, %f, %f"), GateLocation.X, GateLocation.Y, GateLocation.Z);
			//}

			
		}
	}
	else if (CurrentTrainState == ETrainState::Waiting)
	{
		if (bisPassengersGettingOff && CurrentPassengersAlpha != 1.0f)
		{
			CurrentPassengersAlpha += DeltaTime * 2.0f;
			CurrentPassengersAlpha = FMath::Clamp(CurrentPassengersAlpha, 0.0f, 1.0f);
			//FVector CurrentLocation = GetActorLocation();
			FVector NewLocation = FMath::Lerp(GetOnLocation, GetOffLocation, CurrentPassengersAlpha);
			//FVector NewLocation = FMath::Lerp(GetOffLocation, GetOnLocation, CurrentPassengersAlpha);

			for (int32 Num = 0; Num < DoorLs.Num(); Num++)
			{
				//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
				if (Num == CorrectDoorIndex)
				{
					//LS_LOG(LogLS, Log, TEXT("Correct Gate"));
					continue;
				}
				else
				{
					Crowds[Num]->SetRelativeLocation(NewLocation);
					LS_LOG(LogLS, Log, TEXT("CurrentPassengersAlpha : %f"), CurrentPassengersAlpha);
					LS_LOG(LogLS, Log, TEXT("Crowd Moved : %f"), NewLocation.Y);
				}
			}
		}
	}
	else if (CurrentTrainState == ETrainState::Leaving)
	{
		CurrentTrainAlpha += DeltaTime * LerpSpeed;
		CurrentTrainAlpha = FMath::Clamp(CurrentTrainAlpha, 0.0f, 1.0f);
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, LeaveLocation, CurrentTrainAlpha);
		SetActorLocation(NewLocation);
	}
}

void ALSTrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALSTrain, CurrentTrainState);
	DOREPLIFETIME(ALSTrain, TimeTrainWait);
	DOREPLIFETIME(ALSTrain, CorrectDoorIndex);
}

void ALSTrain::BeginPlay()
{
	Super::BeginPlay();

	WaitLocation = FVector(-560.0f, 20.0f, 590.0f);
	LeaveLocation = FVector(7000.0f, 20.0f, 590.0f);
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
		DoorL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	for (UStaticMeshComponent* DoorR : DoorRs)
	{
		DoorR->SetVisibility(false);
		DoorR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			//GateClose();
			MulticastGetOnPassengers();
			MulticastRPCGateClose();
		}
	), TimeTrainWait, false);
}

void ALSTrain::GateClose()
{
	for (UStaticMeshComponent* DoorL : DoorLs)
	{
		DoorL->SetVisibility(true);
		DoorL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	for (UStaticMeshComponent* DoorR : DoorRs)
	{
		DoorR->SetVisibility(true);
		DoorR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	//GateTriggers[CurrentOpenGate]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(TrainTimerHandle, FTimerDelegate::CreateLambda([&]
		{
			CurrentTrainState = ETrainState::Leaving;
		}
	), TimeBeforeTrainLeave, false);
}

void ALSTrain::PuzzleCheck(bool bCorrect, int32 InCorrectGate)
{
	//LS_LOG(LogLS, Log, TEXT("InCorrectGate : %d"), InCorrectGate);
	CorrectDoorIndex = InCorrectGate - 1;

	if (bCorrect)
	{
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("True"));
		TimeTrainWait = 10.0f;
		MulticastRPCGateOpen();
		//GetOffPassengers(InCorrectGate - 1);
		CorrectDoorIndex = InCorrectGate - 1;
		MulticastGetOffPassengers(InCorrectGate - 1);
	}
	else
	{
		LS_LOG(LogLS, Log, TEXT("%s"), TEXT("False"));
		TimeTrainWait = 4.0f;
		MulticastRPCGateOpen();
		//GetOffPassengers(-1);
		CorrectDoorIndex = -1;
		MulticastGetOffPassengers(-1);
	}
}

void ALSTrain::GetOffPassengers(int32 InCorrectGate)
{
	for (int32 Num=0 ; Num< DoorLs.Num() ; Num++)
	{
		//LS_LOG(LogLS, Log, TEXT("Num : %d"), Num);
		if (Num == CorrectDoorIndex)
		{
			//LS_LOG(LogLS, Log, TEXT("Correct Gate"));
			Crowds[Num]->SetVisibility(false);
			Crowds[Num]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			continue;
		}
		else
		{
			Crowds[Num]->SetVisibility(true);
			//Crowds[Num]->SetRelativeLocation(FVector(80, 80, 0));
			//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
		}
	}
	CurrentPassengersAlpha = 0.0f;
	bisPassengersGettingOff = true;
}

void ALSTrain::GetOnPassengers()
{
	//LS_LOG(LogLS, Log, TEXT("Begin"));

	for (int32 Num = 0; Num < DoorLs.Num(); Num++)
	{
		Crowds[Num]->SetVisibility(false);
		Crowds[Num]->SetRelativeLocation(FVector(80, -40, 0));
		//LS_LOG(LogLS, Log, TEXT("Crowd Moved"));
	}
	bisPassengersGettingOff = false;
}

void ALSTrain::StopTrain()
{
	//LS_LOG(LogLS, Log, TEXT("%s"), TEXT("Begin"));
	CurrentTrainState = ETrainState::Stop;
	if (GetWorld()->GetTimerManager().IsTimerActive(TrainTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(TrainTimerHandle);
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
