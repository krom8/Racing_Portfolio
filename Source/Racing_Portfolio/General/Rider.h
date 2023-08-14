// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Rider.generated.h"

/**
 *
 */
UCLASS()
class RACING_PORTFOLIO_API ARider : public AWheeledVehiclePawn
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	ARider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void MoveForward(float Value);

	UFUNCTION(BlueprintCallable)
		void MoveBackward(float Value);

	UFUNCTION(BlueprintCallable)
		void Turn(float Value);

	UFUNCTION(BlueprintCallable)
		void DriftOn();

	UFUNCTION(BlueprintCallable)
		void DriftOff();

	UFUNCTION(BlueprintCallable)
		void Boost();

	UFUNCTION(BlueprintCallable)
		void BoostOff();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPhysicsThrusterComponent* BoosterComp1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPhysicsThrusterComponent* BoosterComp2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxBoostTime = 2.f;

private:

	bool CanBoost = true;
	FTimerHandle BoostTimerHandle;
};