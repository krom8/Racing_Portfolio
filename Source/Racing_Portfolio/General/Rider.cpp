// Fill out your copyright notice in the Description page of Project Settings.


#include "Rider.h"
#include "DefaultGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerInput.h"
#include "General/RiderMovementComponent.h"



// Sets default values
ARider::ARider()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	CapsuleComp->SetCapsuleSize(100.f, 50.f);
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

	MoveComp = CreateDefaultSubobject<URiderMovementComponent>(TEXT("RiderMove"));
	MoveComp->UpdatedComponent = RootComponent;


	int a = 0;




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
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Drift", EKeys::LeftShift, 1.f));
	UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Drift", EKeys::RightShift, 1.f));
	UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Boost", EKeys::SpaceBar));

	PlayerInputComponent->BindAxis("MoveForward", this, &ARider::MoveForward);
	PlayerInputComponent->BindAxis("TurnRight", this, &ARider::TurnRight);
	PlayerInputComponent->BindAxis("TurnLeft", this, &ARider::TurnLeft);
	PlayerInputComponent->BindAxis("MoveBack", this, &ARider::MoveBack);
	PlayerInputComponent->BindAxis("Drift", this, &ARider::Drift);
	PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ARider::Boost);

}


void ARider::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		FRotator const ControlSpaceRot = Controller->GetControlRotation();
		AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), Val);
	}
}

void ARider::MoveBack(float Val)
{
	if (Val != 0.f)
	{
		FRotator const ControlSpaceRot = Controller->GetControlRotation();
		AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::X), -Val);
	}
}
 
void ARider::TurnLeft(float Val)
{
	double YawInput = -Val * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	if (Val != 0.f)
	{
		AddControllerYawInput(YawInput);
		FRotator ControllerRot = GetController()->GetControlRotation();
		SetActorRotation(ControllerRot);
	}

}

void ARider::TurnRight(float Val)
{
	double YawInput = Val * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
	if (Val != 0.f)
	{
		AddControllerYawInput(YawInput);
		FRotator ControllerRot = GetController()->GetControlRotation();
		SetActorRotation(ControllerRot);
	}

	FRotator Check = GetActorRotation();
	FRotator Check2 = GetController()->GetControlRotation();
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("ActorYaw%f"), Check.Yaw));
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString::Printf(TEXT("ContorllerYaw%f"), Check2.Yaw));

}

void ARider::Drift(float Val)
{

}

void ARider::Boost()
{

}

