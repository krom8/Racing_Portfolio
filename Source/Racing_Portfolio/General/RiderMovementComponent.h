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
	float MaxSpeed = 1000.f;

private:
	float CurSpeed = 0;
};
