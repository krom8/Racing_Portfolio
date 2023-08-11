// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Curves/CurveFloat.h"
#include "CarData.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct RACING_PORTFOLIO_API FCarData : public FTableRowBase
{

	GENERATED_BODY()

	FCarData()
	{
	};

	~FCarData()
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
		USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FLName")
		FName FrontLeftWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FRName")
		FName FrontRightWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RLName")
		FName RearLeftWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RRName")
		FName RearRightWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TorqueCurve")
		UCurveFloat* TorqueCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TorqueCurve")
		UCurveFloat* MaxTorqueCurve;

};
