#include "FRealtimeStyleTransferViewExtension.h"
#include "Modules/ModuleManager.h"
#include "PostProcess/PostProcessMaterial.h"
#include "PostProcess/SceneRenderTargets.h"
#include "PreOpenCVHeaders.h"
#include "OpenCVHelper.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core.hpp"
#include "PostOpenCVHeaders.h"
#include <vector>
#include "Math/PackedVector.h"
#include "ImageUtils.h"  


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
	ViewExtensionIsActive = GDynamicRHI->GetName() == FString(TEXT("D3D12"));

}

//------------------------------------------------------------------------------
TSharedPtr<FMyModelHelper> FRealtimeStyleTransferViewExtension::ModelHelper = nullptr;
//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::SetStyle()
{
	TObjectPtr<UNNEModelData> ManuallyLoadedModelData = LoadObject<UNNEModelData>(GetTransientPackage(), TEXT("/Game/Content_General/DLModels/TestModel3.TestModel3"));
	ModelHelper = MakeShared< FMyModelHelper >();
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
	UWorld* CW = Context.GetWorld();
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
			{
				return;
			}
				
			FRHITexture2D* Texture = SourceTexture->GetRHI()->GetTexture2D();
			Width = Texture->GetSizeX();
			Height = Texture->GetSizeY();
			CopyTextureFromGPUToCPU(RHICmdList, Texture);
			ResizeScreenImageToMatchModel();
			ApplyStyle();
			ResizeModelImageToMatchScreen();
			CopyTextureFromCPUToGPU(RHICmdList, Texture);
		});
	RealtimeStyleTransfer::IsActive = 0;
}


//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::CopyTextureFromCPUToGPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture)
{
	const FUpdateTextureRegion2D TextureRegion2D(0, 0, 0, 0, Width, Height);
	RHICmdList.UpdateTexture2D(Texture, 0, TextureRegion2D, Width * 4, (const uint8*)StylizedImageCPU.GetData());
	Texture;
	int a = 0;
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::CopyTextureFromGPUToCPU(FRHICommandListImmediate& RHICmdList, FRHITexture2D* Texture)
{
	double startSeconds = FPlatformTime::Seconds();

	const int PixelCount = Width * Height;
	if (check != 0)
	{
		SaveTextureToPNGFile(RawImage, Width, Height, TEXT("D:/First.png"));
	}
	check += 1;
	RHICmdList.ReadSurfaceData(
		Texture,
		FIntRect(0, 0, Width, Height),
		RawImage,
		FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX));
	SaveTextureToPNGFile(RawImage, Width, Height, TEXT("D:/Second.png"));
	InputImageCPU.Reset();
	InputImageCPU.SetNumZeroed(PixelCount * 3);

	ParallelFor(RawImage.Num(), [&](int32 Idx) {
		const int i = Idx * 3;
		const FColor& Pixel = RawImage[Idx];

		InputImageCPU[i] = Pixel.R;
		InputImageCPU[i + 1] = Pixel.G;
		InputImageCPU[i + 2] = Pixel.B;
		});

	// PNG 파일로 저장
	SaveTextureToPNGFile(RawImage, Width, Height, TEXT("D:/Test.png"));
	SaveTextureToPNGFile(InputImageCPU, Width, Height, TEXT("D:/TestInput.png"));
	// print time elapsed
	double secondsElapsed = FPlatformTime::Seconds() - startSeconds;

	UE_LOG(LogTemp, Log, TEXT("Read pixel completed in %f."), secondsElapsed)
}

//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::ResizeScreenImageToMatchModel()
{
	// Create image from StylizedImage object
	cv::Mat inputImage(Height, Width, CV_8UC3, InputImageCPU.GetData());
	cv::cvtColor(inputImage, inputImage, cv::COLOR_RGB2BGR);
	imwrite("D:/CVBeforeImage.png", inputImage);
	// Create image to resize for inferencing
	cv::Mat outputImage(224, 224, CV_8UC3);

	// Resize to outputImage
	cv::resize(inputImage, outputImage, cv::Size(224, 224));
	imwrite("D:/CVImage.png", outputImage);
	// Reshape to 1D
	outputImage = outputImage.reshape(1, 1);

	// uint_8, [0, 255] -> float, [0, 1]
	std::vector<float> vec;
	outputImage.convertTo(vec, CV_32FC1, 1. / 255);

	// Height, Width, Channel to Channel, Height, Width
	const int inputSize = 224 * 224 * 3; 
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


	for (size_t i=0; i < inputSize; ++i) {
		if (ModelHelper->InputData[i] != 0.0)
		{
			float k = ModelHelper->InputData[i];
			int q = 0;
		}
	} 


	SaveJsonSerializableArrayToPNGFile(ModelHelper->InputData, 224, 224, TEXT("D:/Input.png"));
	int a = 0;
}


//------------------------------------------------------------------------------
void FRealtimeStyleTransferViewExtension::ResizeModelImageToMatchScreen()
{
	if (ModelHelper->OutputBindings.Num() == 0)
	{
		return;
	}
		
	const int OutputSize = 222 * 222 * 3;
	const int BlockSize = OutputSize / 3;
	TArray<float> ModelOutput = ModelHelper->OutputData;
	std::vector<uint8> vec;
	vec.resize(OutputSize);
	for (size_t i=0; i<BlockSize; ++i)
	{
		uint8 Red = FMath::Clamp<uint8>(FMath::RoundToInt(ModelOutput[i]), 0, 255);
		uint8 Green = FMath::Clamp<uint8>(FMath::RoundToInt(ModelOutput[BlockSize + i]), 0, 255);
		uint8 Blue = FMath::Clamp<uint8>(FMath::RoundToInt(ModelOutput[BlockSize * 2 + i]), 0, 255);
		vec[i * 3] = Red;
		vec[i * 3 + 1] = Green;
		vec[i * 3 + 2] = Blue;
	}

	ModelHelper->OutputData.Reset(); // 없애야 할 수 있음
	ModelHelper->OutputData.SetNumZeroed(OutputSize);
	ModelHelper->OutputBindings[0].Data = ModelHelper->OutputData.GetData();
	ModelHelper->OutputBindings[0].SizeInBytes = ModelHelper->OutputData.Num() * sizeof(float);


	cv::Mat resultImage(222, 222, CV_8UC3, vec.data());
	imwrite("D:/CVoutput.png", resultImage);
	//cv::Mat resultImage(224, 224, CV_32FC1, ModelHelper->OutputBindings.GetData());
	//resultImage.convertTo(resultImage, CV_8UC3, 255);

	
	cv::resize(resultImage, resultImage, cv::Size(Width	, Height));
	imwrite("D:/CVoutputresize.png", resultImage);
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
	//ModelHelper->OutputBindings.Reset(); // 없애야 할 수 있음

	const int inputSize = 224 * 224 * 3;
	const int outputSize = 222 * 222 * 3;


	for (size_t i = 0; i < inputSize; ++i)
	{
		float testvalue = ModelHelper->InputData[i];
		if (testvalue != 0)
		{
			int e = 0;
		}
	}

	if (ModelHelper.IsValid())
	{
		// Example for async inference
			// Process ModelHelper->OutputData from the previous run here
			// Fill in new data into ModelHelper->InputData here
			TSharedPtr<FMyModelHelper> ModelHelperPtr = ModelHelper;
			if (ModelHelperPtr->ModelInstance->RunSync(ModelHelperPtr->InputBindings, ModelHelperPtr->OutputBindings) != 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to run the model"));
			}
	}
	TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelHelper->ModelInstance->GetInputTensorDescs();
	UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[0].GetShape();
	TConstArrayView<UE::NNE::FTensorDesc> OutputTensorDescs = ModelHelper->ModelInstance->GetOutputTensorDescs();
	SaveJsonSerializableArrayToPNGFile2(ModelHelper->OutputData, 222, 222, "D:/modeloutput.png");
	int fewf = 0;

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



void FRealtimeStyleTransferViewExtension::SaveTextureToPNGFile(const TArray<FColor>& InPixels, int32 InWidth, int32 InHeight, const FString& FilePath)
{
	// PNG 파일로 저장
	TArray<uint8> PNGData;
	PNGData.SetNumZeroed(InWidth * InHeight * 3);
	FImageUtils::CompressImageArray(InWidth, InHeight, InPixels, PNGData);

	// Save PNG data to file
	if (FFileHelper::SaveArrayToFile(PNGData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("Saved PNG file: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save PNG file: %s"), *FilePath);
	}
}


void FRealtimeStyleTransferViewExtension::SaveTextureToPNGFile(const TArray<unsigned char, TSizedDefaultAllocator<32>>& InPixels, int32 InWidth, int32 InHeight, const FString& FilePath)
{
	// FColor 배열을 생성하고 InputImageCPU의 값을 복사
	TArray<FColor> InColors;
	InColors.SetNumUninitialized(InPixels.Num() / 3);

	for (int32 i = 0; i < InPixels.Num(); i += 3)
	{
		InColors[i / 3] = FColor(InPixels[i], InPixels[i + 1], InPixels[i + 2]);
	}

	// PNG 파일로 저장할 데이터 압축
	TArray<uint8> PNGData;
	PNGData.SetNumZeroed(InWidth * InHeight * 3);
	FImageUtils::CompressImageArray(InWidth, InHeight, InColors, PNGData);

	// PNG 데이터를 파일로 저장
	if (FFileHelper::SaveArrayToFile(PNGData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("PNG 파일을 저장했습니다: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PNG 파일 저장 실패: %s"), *FilePath);
	}
}


void FRealtimeStyleTransferViewExtension::SaveJsonSerializableArrayToPNGFile(FJsonSerializableArrayFloat ImageArray, int32 InWidth, int32 InHeight, const FString& FilePath)
{
	float* TempArray = ImageArray.GetData();
	TArray<FColor> InColors;
	InColors.SetNumUninitialized(ImageArray.Num() / 3);

	for (int32 i = 0; i < ImageArray.Num(); i += 3)
	{
		float test = TempArray[i];
		uint8 Red = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i+2] * 255.0f), 0, 255);
		uint8 Green = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i + 1] * 255.0f), 0, 255);
		uint8 Blue = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i] * 255.0f), 0, 255);
		InColors[i / 3] = FColor(Red, Green, Blue, 255);
	}
	int fefwfe = 0;
	// 픽셀 데이터를 PNG 형식으로 압축합니다.
	TArray<uint8> PNGData;
	PNGData.SetNumZeroed(InWidth*InHeight*3);
	FImageUtils::ThumbnailCompressImageArray(InWidth, InHeight, InColors, PNGData); //////////여기서 뭔가 벌어진다앙ㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇ 

	// PNG 데이터를 파일로 저장합니다.
	if (FFileHelper::SaveArrayToFile(PNGData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("PNG 파일을 저장했습니다: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PNG 파일 저장 실패: %s"), *FilePath);
	}

}

void FRealtimeStyleTransferViewExtension::SaveJsonSerializableArrayToPNGFile2(FJsonSerializableArrayFloat ImageArray, int32 InWidth, int32 InHeight, const FString& FilePath)
{
	float* TempArray = ImageArray.GetData();
	TArray<FColor> InColors;
	InColors.SetNumUninitialized(ImageArray.Num() / 3);

	for (int32 i = 0; i < ImageArray.Num(); i += 3)
	{
		float test = TempArray[i];
		uint8 Red = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i + 2]), 0, 255);
		uint8 Green = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i + 1]), 0, 255);
		uint8 Blue = FMath::Clamp<uint8>(FMath::RoundToInt(TempArray[i]), 0, 255);
		InColors[i / 3] = FColor(Red, Green, Blue, 255);
	}
	int fefwfe = 0;
	// 픽셀 데이터를 PNG 형식으로 압축합니다.
	TArray<uint8> PNGData;
	PNGData.SetNumZeroed(InWidth * InHeight * 3);
	FImageUtils::ThumbnailCompressImageArray(InWidth, InHeight, InColors, PNGData); //////////여기서 뭔가 벌어진다앙ㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇ 

	// PNG 데이터를 파일로 저장합니다.
	if (FFileHelper::SaveArrayToFile(PNGData, *FilePath))
	{
		UE_LOG(LogTemp, Log, TEXT("PNG 파일을 저장했습니다: %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PNG 파일 저장 실패: %s"), *FilePath);
	}

}





#undef LOCTEXT_NAMESPACE
