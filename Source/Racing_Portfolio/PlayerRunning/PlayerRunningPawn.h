// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "General/Rider.h"
#include "PlayerRunningPawn.generated.h"

UCLASS()
class RACING_PORTFOLIO_API APlayerRunningPawn : public ARider
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerRunningPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
