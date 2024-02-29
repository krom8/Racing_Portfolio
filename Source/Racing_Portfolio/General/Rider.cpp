// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider.h"
#include "ChaosVehicleMovementComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "FrontWheel.h"
#include "RearWheel.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerInput.h"
#include "DefaultGameInstance.h"
#include "Curves/CurveFloat.h"
#include "General/CarData.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"


ARider::ARider()
{
	PrimaryActorTick.bCanEverTick = true;

	FName CarName = TEXT("DefaultCar");
	FString DataTablePath = TEXT("/Script/Engine.DataTable'/Game/BP_General/CarDataTable.CarDataTable'");
	ConstructorHelpers::FObjectFinder<UDataTable> DataTable(*DataTablePath);
	if (DataTable.Succeeded() == false)
	{
		return;
	}
	UDataTable* Datas = DataTable.Object;
	FCarData* AllCarData = Datas->FindRow<FCarData>(CarName, CarName.ToString());

	USkeletalMesh* DefaultCar = AllCarData->Mesh;
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSkeletalMesh(DefaultCar);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(-400.f, 0.f, 300.f));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

	UChaosWheeledVehicleMovementComponent* WheeledMovementComponent = Cast<UChaosWheeledVehicleMovementComponent>(GetMovementComponent());
	FChaosWheelSetup FLWheelSetup;
	FLWheelSetup.WheelClass = UFrontWheel::StaticClass();
	FLWheelSetup.BoneName = AllCarData->FrontLeftWheel;
	WheeledMovementComponent->WheelSetups.Add(FLWheelSetup);

	FChaosWheelSetup FRWheelSetup;
	FRWheelSetup.WheelClass = UFrontWheel::StaticClass();
	FRWheelSetup.BoneName = AllCarData->FrontRightWheel;
	WheeledMovementComponent->WheelSetups.Add(FRWheelSetup);

	FChaosWheelSetup RLWheelSetup;
	RLWheelSetup.WheelClass = URearWheel::StaticClass();
	RLWheelSetup.BoneName = AllCarData->RearLeftWheel;
	WheeledMovementComponent->WheelSetups.Add(RLWheelSetup);

	FChaosWheelSetup RRWheelSetup;
	RRWheelSetup.WheelClass = URearWheel::StaticClass();
	RRWheelSetup.BoneName = AllCarData->RearRightWheel;
	WheeledMovementComponent->WheelSetups.Add(RRWheelSetup);

	UCurveFloat* CurveData = AllCarData->TorqueCurve;
	WheeledMovementComponent->EngineSetup.TorqueCurve.ExternalCurve = CurveData;

	BoosterComp1 = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("Booster1"));
	BoosterComp1->SetupAttachment(RootComponent);
	FTransform BoosterTransform1 = { FRotator(0.f, 0.f, 180.f), FVector(-230.f, -65.f, 40.f), FVector(1, 1, 1) };
	BoosterComp1->SetRelativeTransform(BoosterTransform1);
	BoosterComp1->ThrustStrength = 1000000.f;
	//BoosterComp1->SetActorRelativeLocation({-230.f, -65.f, 40.f});


	BoosterComp2 = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("Booster2"));
	BoosterComp2->SetupAttachment(RootComponent);
	FTransform BoosterTransform2 = FTransform(FRotator(0.f, 0.f, 180.f), FVector(-230.f, 65.f, 40.f), FVector(1, 1, 1));
	BoosterComp2->SetRelativeTransform(BoosterTransform2);
	BoosterComp1->ThrustStrength = 1000000.f;
	//BoosterComp2->SetActorRelativeLocation({ 0.f, 130.f, 0.f });

	MeshComp->SetSimulatePhysics(true);
}


void ARider::BeginPlay()
{
	Super::BeginPlay();
}

void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveBackward", EKeys::S, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::A, -1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::D, 1.f));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Drift", EKeys::LeftShift));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Drift", EKeys::RightShift));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Boost", EKeys::SpaceBar));

	PlayerInputComponent->BindAxis("MoveForward", this, &ARider::MoveForward);
	PlayerInputComponent->BindAxis("MoveBackward", this, &ARider::MoveBackward);
	PlayerInputComponent->BindAxis("Turn", this, &ARider::Turn);
	PlayerInputComponent->BindAction("DriftOn", IE_Pressed, this, &ARider::DriftOn);
	PlayerInputComponent->BindAction("DriftOff", IE_Released, this, &ARider::DriftOff);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ARider::Boost);

}

void ARider::MoveForward(float Value)
{
	GetVehicleMovementComponent()->SetThrottleInput(Value);
}

void ARider::MoveBackward(float Value)
{
	GetVehicleMovementComponent()->SetBrakeInput(Value);
}


void ARider::Turn(float Value)
{
	GetVehicleMovementComponent()->SetSteeringInput(Value);
}

void ARider::DriftOn()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(true);
}

void ARider::DriftOff()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
}

void ARider::Boost()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"), CanBoost));
	if (CanBoost)
	{
		CanBoost = false;
		BoosterComp1->Activate();
		BoosterComp2->Activate();
		GetWorld()->GetTimerManager().SetTimer(BoostTimerHandle, this, &ARider::BoostOff, MaxBoostTime, false);
	}
	
}

void ARider::BoostOff()
{
	BoosterComp1->Deactivate();
	BoosterComp2->Deactivate();
	CanBoost = true;
	GetWorld()->GetTimerManager().ClearTimer(BoostTimerHandle);
}

