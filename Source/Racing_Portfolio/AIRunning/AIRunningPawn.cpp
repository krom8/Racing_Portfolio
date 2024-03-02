// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRunningPawn.h"

// Sets default values
AAIRunningPawn::AAIRunningPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIRunningPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAIRunningPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIRunningPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

