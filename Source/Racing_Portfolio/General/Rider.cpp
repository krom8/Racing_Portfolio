// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider.h"
#include "DefaultGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerInput.h"


// Sets default values
ARider::ARider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	RootComponent = CapsuleComp;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComp->SetupAttachment(RootComponent);

	FString DataPath = TEXT("/Script/Engine.SkeletalMesh'/Game/Content_General/AssetsvilleTown/Meshes/Vehicles/SK_veh_SportClassic_01.SK_veh_SportClassic_01'");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshLoader(*DataPath);
	if (SkeletalMeshLoader.Succeeded() == false)
	{
		return;
	}
	USkeletalMesh* DefaultCar = SkeletalMeshLoader.Object;
	SkeletalMeshComp->SetSkeletalMesh(DefaultCar);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(-400.f, 0.f, 300.f));



	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp);

}

// Called when the game starts or when spawned
void ARider::BeginPlay()
{
	Super::BeginPlay();




}

// Called every frame
void ARider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveBack", EKeys::S, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnLeft", EKeys::A, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRight", EKeys::D, 1.f));

	PlayerInputComponent->BindAxis("MoveForward", this, &ARider::MoveForward);
	PlayerInputComponent->BindAxis("TurnRight", this, &ARider::TurnRight);
	PlayerInputComponent->BindAxis("TurnLeft", this, &ARider::TurnLeft);
	PlayerInputComponent->BindAxis("MoveBack", this, &ARider::MoveBack);

}


void ARider::MoveForward(float Val)
{

}

void ARider::MoveBack(float Val)
{

}

void ARider::TurnLeft(float Val)
{

}

void ARider::TurnRight(float Val)
{

}
