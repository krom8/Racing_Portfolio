#pragma once
#include "SceneViewExtension.h"
#include "NNE.h"
#include "NNERuntimeCPU.h"
#include "NNEModelData.h"


class FMyModelHelper
{
public:
	TUniquePtr<UE::NNE::IModelInstanceCPU> ModelInstance;
	TArray<float> InputData;
	TArray<float> OutputData;
	TArray<UE::NNE::FTensorBindingCPU> InputBindings;
	TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
	bool bIsRunning;
};


class FRealtimeStyleTransferViewExtension: public FSceneViewExtensionBase
{
public:
	FRealtimeStyleTransferViewExtension(const FAutoRegister& AutoRegister);

	static void SetStyle();

	//~ ISceneViewExtension interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;

	FScreenPassTexture AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs);

private:
	static bool ViewExtensionIsActive;
	TArray<FColor> RawImage;
	TArray<uint8> InputImageCPU;
	TArray<uint32> StylizedImageCPU;
	//TArray<float> ModelInputImage;
	//TArray<uint8> ModelOutputImage; 
	int Width;
	int Height;
	void AddStylePass_RenderThread(FRDGBuilder& GraphBuilder, FRDGTextureRef SourceTexture);
	void CopyTextureFromGPUToCPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture);
	void CopyTextureFromCPUToGPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture);
	void ResizeScreenImageToMatchModel();
	void ResizeModelImageToMatchScreen();
	void ApplyStyle();

	void SaveTextureToPNGFile(const TArray<FColor>& InPixels, int32 InWidth, int32 InHeight, const FString& FilePath);
	void SaveTextureToPNGFile(const TArray<unsigned char, TSizedDefaultAllocator<32>>& InPixels, int32 InWidth, int32 InHeight, const FString& FilePath);
	void SaveJsonSerializableArrayToPNGFile(FJsonSerializableArrayFloat ImageArray, int32 InWidth, int32 InHeight, const FString& FilePath);
	void SaveJsonSerializableArrayToPNGFile2(FJsonSerializableArrayFloat ImageArray, int32 InWidth, int32 InHeight, const FString& FilePath);
	static TSharedPtr<FMyModelHelper> ModelHelper;




protected:
	FScreenPassTexture ApplyStyleTransfer(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs, const FString& DDSFileName);
};

