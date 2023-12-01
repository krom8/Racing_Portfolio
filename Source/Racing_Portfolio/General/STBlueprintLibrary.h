// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "STBlueprintLibrary.generated.h"

/**
 *
 */
UCLASS()
class RACING_PORTFOLIO_API USTBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    USTBlueprintLibrary(const FObjectInitializer& ObjectInitializer);
    ~USTBlueprintLibrary();

    UFUNCTION(BlueprintCallable, Category = "YourCategory")
    void SetStyle();
};