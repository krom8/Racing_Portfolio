// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "GameSkeletalMeshData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct RACING_PORTFOLIO_API FGameSkeletalMeshData : public FTableRowBase
{

	GENERATED_BODY()

	FGameSkeletalMeshData()
	{
	};

	~FGameSkeletalMeshData()
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	USkeletalMesh* Mesh;

};
