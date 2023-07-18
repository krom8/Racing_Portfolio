// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Rider.generated.h"

UCLASS()
class RACING_PORTFOLIO_API ARider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARider();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SkeletalMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;


	class Wheel* FrontLeftWheel;

	class Wheel* FrontRightWheel;

	class Wheel* RearLeftWheel;

	class Wheel* RearRightWheel;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Val);

	void MoveBack(float Val);

	void TurnLeft(float Val);

	void TurnRight(float Val);

	void Drift(float Val);

	void Boost(float Val);
};
