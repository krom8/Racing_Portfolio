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
	FVector Gravity = {0.f, 0.f, 500.f};

private:

	// From Car
	float BaseTurnRate;
	float MaxSpeed;
	float AccelSpeed;
	float DecreaseSpeed;
	float IncreaseSpeed;
	float TurnDecreaseSpeed;
	float BackwardSpeed;
	float BreakSpeed;
	float BackwardMaxSpeed;


	float MoveDeltaTime;
	FVector InputVector;

	AController* Con;
	float CurSpeed = 0.f;
	FVector CurPawnRotVector;
	FVector CurVector;
	bool Drift = false;
	bool Boost = false;

	void DecreaseMove();
	void ForwardMove();
	void BackwardMove();
	void DecreaseTurnMove(bool Right = true);
	void ForwardTurnMove(bool Right = true);
	void BackwardTurnMove(bool Right = true);
	void Rotate(bool Right=true, bool Backward=false);
	void Move();
	void Print(FVector A);

};
