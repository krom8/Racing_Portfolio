// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class RACING_PORTFOLIO_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UDefaultGameInstance();

	~UDefaultGameInstance();
	TArray<UStaticMesh*> Arrmesh;
	
};
