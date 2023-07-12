// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGamePawn.h"

// Sets default values
ATitleGamePawn::ATitleGamePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATitleGamePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATitleGamePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATitleGamePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

