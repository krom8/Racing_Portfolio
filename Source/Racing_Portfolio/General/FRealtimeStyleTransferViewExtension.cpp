#include "FRealtimeStyleTransferViewExtension.h"
#include "Modules/ModuleManager.h"
#include "PostProcess/PostProcessMaterial.h"
#include "PostProcess/SceneRenderTargets.h"
// STNeuiralNetwork
#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"
#include "PostOpenCVHeaders.h"
#include <vector>

#include "Math/PackedVector.h"

TSharedPtr<FMyModelHelper> FRealtimeStyleTransferViewExtension::ModelHelper = nullptr;
bool FRealtimeStyleTransferViewExtension::ViewExtensionIsActive = false;

DEFINE_LOG_CATEGORY_STATIC(LogRealtimeStyleTransfer, Log, All);
void RenderMyTest(FRHICommandList& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, const FLinearColor& Color);

namespace RealtimeStyleTransfer
{
	static int32 IsActive = 0;
	static FAutoConsoleVariableRef CVarStyleTransferIsActive(
		TEXT("r.RealtimeStyleTransfer.Enable"),
		IsActive,
		TEXT("Allows an additional rendering pass that will apply a neural style to the frame.\n")
		TEXT("=0:off (default), >0: on"), 
		ECVF_Cheat | ECVF_RenderThreadSafe);
}
//------------------------------------------------------------------------------
FRealtimeStyleTransferViewExtension::FRealtimeStyleTransferViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::SetStyle()
{
	ViewExtensionIsActive = GDynamicRHI->GetName() == FString(TEXT("D3D12"));

	TObjectPtr<UNNEModelData> ManuallyLoadedModelData = LoadObject<UNNEModelData>(GetTransientPackage(), TEXT("/Game/Content_General/DLModels/TestModel.TestModel"));
	TSharedPtr< FMyModelHelper > ModelHelper = MakeShared< FMyModelHelper >();
	if (ManuallyLoadedModelData)
	{
		UE_LOG(LogTemp, Display, TEXT("ManuallyLoadedModelData loaded %s"), *ManuallyLoadedModelData->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ManuallyLoadedModelData is not loaded, please check the static path to your asset"));
	}
	if (ManuallyLoadedModelData)
	{
		// Example for model creation
		TWeakInterfacePtr<INNERuntimeCPU> Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));
		if (Runtime.IsValid())
		{
			TUniquePtr<UE::NNE::IModelCPU> Model = Runtime->CreateModel(ManuallyLoadedModelData);
			if (Model.IsValid())
			{
				ModelHelper->ModelInstance = Model->CreateModelInstance();
				if (ModelHelper->ModelInstance.IsValid())
				{
					ModelHelper->bIsRunning = false;

					// Example for querying and testing in- and outputs
					TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelHelper->ModelInstance->GetInputTensorDescs();
					checkf(InputTensorDescs.Num() == 1, TEXT("The current example supports only models with a single input tensor"));
					UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[0].GetShape();
					checkf(SymbolicInputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable input tensor dimensions"));
					TArray<UE::NNE::FTensorShape> InputTensorShapes = { UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape) };

					ModelHelper->ModelInstance->SetInputTensorShapes(InputTensorShapes);

					TConstArrayView<UE::NNE::FTensorDesc> OutputTensorDescs = ModelHelper->ModelInstance->GetOutputTensorDescs();
					checkf(OutputTensorDescs.Num() == 1, TEXT("The current example supports only models with a single output tensor"));
					UE::NNE::FSymbolicTensorShape SymbolicOutputTensorShape = OutputTensorDescs[0].GetShape();
					checkf(SymbolicOutputTensorShape.IsConcrete(), TEXT("The current example supports only models without variable output tensor dimensions"));
					TArray<UE::NNE::FTensorShape> OutputTensorShapes = { UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicOutputTensorShape) };

					// Example for creating in- and outputs
					ModelHelper->InputData.SetNumZeroed(InputTensorShapes[0].Volume());
					ModelHelper->InputBindings.SetNumZeroed(1);
					ModelHelper->InputBindings[0].Data = ModelHelper->InputData.GetData();
					ModelHelper->InputBindings[0].SizeInBytes = ModelHelper->InputData.Num() * sizeof(float);

					ModelHelper->OutputData.SetNumZeroed(OutputTensorShapes[0].Volume());
					ModelHelper->OutputBindings.SetNumZeroed(1);
					ModelHelper->OutputBindings[0].Data = ModelHelper->OutputData.GetData();
					ModelHelper->OutputBindings[0].SizeInBytes = ModelHelper->OutputData.Num() * sizeof(float);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create the model instance"));
					ModelHelper.Reset();
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to create the model"));
				ModelHelper.Reset();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find runtime NNERuntimeORTCpu, please enable the corresponding plugin"));
		}
	}
}

//------------------------------------------------------------------------------
bool FRealtimeStyleTransferViewExtension::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return ViewExtensionIsActive;
}

//------------------------------------------------------------------------------
BEGIN_SHADER_PARAMETER_STRUCT(FStylePassParameters, )
RDG_TEXTURE_ACCESS(Source, ERHIAccess::CPURead)
END_SHADER_PARAMETER_STRUCT()

void FRealtimeStyleTransferViewExtension::AddStylePass_RenderThread(
	FRDGBuilder& GraphBuilder,
	FRDGTextureRef SourceTexture)
{
	if (SourceTexture == nullptr)
	{
		UE_LOG(LogRealtimeStyleTransfer, Warning, TEXT("Skipping null texture"));
		return;
	}

	FStylePassParameters* Parameters = GraphBuilder.AllocParameters<FStylePassParameters>();
	Parameters->Source = SourceTexture;

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("RealtimeStyleTransfer"),
		Parameters,
		ERDGPassFlags::Readback,
		[this, SourceTexture](FRHICommandListImmediate& RHICmdList) {
			if (ModelHelper == nullptr)
				return;

			FRHITexture2D* Texture = SourceTexture->GetRHI()->GetTexture2D();
			Width = Texture->GetSizeX();
			Height = Texture->GetSizeY();
			CopyTextureFromGPUToCPU(RHICmdList, Texture);
			ResizeScreenImageToMatchModel();
			ApplyStyle();
			ResizeModelImageToMatchScreen();
			CopyTextureFromCPUToGPU(RHICmdList, Texture);
		});
}


//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::CopyTextureFromCPUToGPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture)
{
	const FUpdateTextureRegion2D TextureRegion2D(0, 0, 0, 0, Width, Height);
	RHICmdList.UpdateTexture2D(Texture, 0, TextureRegion2D, Width * 4, (const uint8*)StylizedImageCPU.GetData());
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::CopyTextureFromGPUToCPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture)
{
	double startSeconds = FPlatformTime::Seconds();

	const int PixelCount = Width * Height;

	RHICmdList.ReadSurfaceData(
		Texture,
		FIntRect(0, 0, Width, Height),
		RawImage,
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX));

	InputImageCPU.Reset();
	InputImageCPU.SetNumZeroed(PixelCount * 3);

	ParallelFor(RawImage.Num(), [&](int32 Idx) {
		const int i = Idx * 3;
		const FColor& Pixel = RawImage[Idx];

		InputImageCPU[i] = Pixel.R;
		InputImageCPU[i + 1] = Pixel.G;
		InputImageCPU[i + 2] = Pixel.B;
		});

	// print time elapsed
	double secondsElapsed = FPlatformTime::Seconds() - startSeconds;

	UE_LOG(LogTemp, Log, TEXT("Read pixel completed in %f."), secondsElapsed)
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::ResizeScreenImageToMatchModel()
{
	// Create image from StylizedImage object
	cv::Mat inputImage(Height, Width, CV_8UC3, InputImageCPU.GetData());

	// Create image to resize for inferencing
	cv::Mat outputImage(224, 224, CV_8UC3);

	// Resize to outputImage
	cv::resize(inputImage, outputImage, cv::Size(224, 224));

	// Reshape to 1D
	outputImage = outputImage.reshape(1, 1);

	// uint_8, [0, 255] -> float, [0, 1]
	std::vector<float> vec;
	outputImage.convertTo(vec, CV_32FC1, 1. / 255);

	// Height, Width, Channel to Channel, Height, Width
	const int inputSize = 224 * 224 * 3; // 모델에 맞게 수정해야함
	ModelHelper->InputData.Reset();
	ModelHelper->InputData.SetNumZeroed(inputSize);

	for (size_t ch = 0; ch < 3; ++ch) {
		const int blockSize = inputSize / 3;

		ParallelFor(blockSize, [&](int32 Idx) {
			const int i = (Idx * 3) + ch;

			const int stride = ch * blockSize;

			ModelHelper->InputData[Idx + stride] = vec[i];
			});
	}
	ModelHelper->InputBindings[0].Data = ModelHelper->InputData.GetData();
	ModelHelper->InputBindings[0].SizeInBytes = ModelHelper->InputData.Num() * sizeof(float);
}


//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::ResizeModelImageToMatchScreen()
{
	if (ModelHelper->OutputBindings.Num() == 0)
		return;

	cv::Mat resultImage(224, 224, CV_8UC3, ModelHelper->OutputBindings.GetData());
	cv::resize(resultImage, resultImage, cv::Size(Width, Height));

	const uint8* RawPixel = resultImage.data;
	const int PixelCount = Width * Height;
	StylizedImageCPU.Reset();
	StylizedImageCPU.SetNumZeroed(PixelCount);

	ParallelFor(PixelCount, [&](int32 Idx) {
		const int i = Idx * 3;

		uint32 R, G, B;

		R = RawPixel[i];
		G = RawPixel[i + 1];
		B = RawPixel[i + 2];
		uint32 color = (R << 22) | (G << 12) | (B << 2) | 3;

		StylizedImageCPU[Idx] = color;
		});
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::ApplyStyle()
{
	// create network and run model
	//ModelHelper->OutputData.Reset(); // 없애야 할 수 있음
	//ModelHelper->OutputBindings.Reset(); // 없애야 할 수 있음

	if (ModelHelper.IsValid())
	{
		// Example for async inference
		if (!ModelHelper->bIsRunning)
		{
			// Process ModelHelper->OutputData from the previous run here
			// Fill in new data into ModelHelper->InputData here

			ModelHelper->bIsRunning = true;
			TSharedPtr<FMyModelHelper> ModelHelperPtr = ModelHelper;
			AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [ModelHelperPtr]()
				{
					if (ModelHelperPtr->ModelInstance->RunSync(ModelHelperPtr->InputBindings, ModelHelperPtr->OutputBindings) != 0)
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to run the model"));
					}
					AsyncTask(ENamedThreads::GameThread, [ModelHelperPtr]()
						{
							ModelHelperPtr->bIsRunning = false;
						});
				});

		}
	}
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::SubscribeToPostProcessingPass(EPostProcessingPass PassId, FAfterPassCallbackDelegateArray& InOutPassCallbacks, bool bIsPassEnabled)
{
	if (!RealtimeStyleTransfer::IsActive)
		return;

	if (!bIsPassEnabled)
		return;

	if (PassId == EPostProcessingPass::Tonemap)
		InOutPassCallbacks.Add(FAfterPassCallbackDelegate::CreateRaw(this, &FRealtimeStyleTransferViewExtension::AfterTonemap_RenderThread));
}

//------------------------------------------------------------------------------
FScreenPassTexture FRealtimeStyleTransferViewExtension::ApplyStyleTransfer(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs, const FString& DDSFileName)
{
	FRDGTextureRef SaveTexture = nullptr;
	FScreenPassTexture ReturnTexture;

	if (InOutInputs.OverrideOutput.IsValid())
	{
		SaveTexture = InOutInputs.OverrideOutput.Texture;
		ReturnTexture = InOutInputs.OverrideOutput;
	}
	else
	{
		SaveTexture = InOutInputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor].Texture;
		ReturnTexture = const_cast<FScreenPassTexture&>(InOutInputs.Textures[(uint32)EPostProcessMaterialInput::SceneColor]);
	}

	AddStylePass_RenderThread(GraphBuilder, SaveTexture);

	return ReturnTexture;
}

//------------------------------------------------------------------------------
FScreenPassTexture FRealtimeStyleTransferViewExtension::AfterTonemap_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& InOutInputs)
{
	RDG_EVENT_SCOPE(GraphBuilder, "RealtimeStyleTransfer_AfterTonemap");
	return ApplyStyleTransfer(GraphBuilder, View, InOutInputs, FString::Printf(TEXT("After%02dTonemap"), EPostProcessingPass::Tonemap));
}


#undef LOCTEXT_NAMESPACE