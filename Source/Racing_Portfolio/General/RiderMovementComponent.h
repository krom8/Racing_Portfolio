// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "RiderMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class RACING_PORTFOLIO_API URiderMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AccelSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecreaseSpeed = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IncreaseSpeed = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackwardSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BreakSpeed = 40.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackwardMaxSpeed = 1000.f;

private:
	float CurSpeed = 0.f;
	FVector CurPawnRotVector = { 1.f, 0.f, 0.f };
	FVector CurVector;

};
