// Fill out your copyright notice in the Description page of Project Settings.


#include "General/STBlueprintLibrary.h"
#include "FRealtimeStyleTransferViewExtension.h"

USTBlueprintLibrary::USTBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Constructor logic here
}

USTBlueprintLibrary::~USTBlueprintLibrary()
{
    // Destructor logic here
}
void USTBlueprintLibrary::SetStyle()
{
    FRealtimeStyleTransferViewExtension::SetStyle();
}