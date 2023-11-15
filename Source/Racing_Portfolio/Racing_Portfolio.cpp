// Copyright Epic Games, Inc. All Rights Reserved.

#include "Racing_Portfolio.h"
#include "Modules/ModuleManager.h"
#include "General/FRealtimeStyleTransferViewExtension.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FNNEStyleTransferTestModule, Racing_Portfolio, "Racing_Portfolio" );


void FNNEStyleTransferTestModule::StartupModule()
{
	RealtimeStyleTransferViewExtension = FSceneViewExtensions::NewExtension<FRealtimeStyleTransferViewExtension>();
}