// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "General/FRealtimeStyleTransferViewExtension.h"

class FNNEStyleTransferTestModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
protected:
	TSharedPtr<FRealtimeStyleTransferViewExtension, ESPMode::ThreadSafe> RealtimeStyleTransferViewExtension;
};