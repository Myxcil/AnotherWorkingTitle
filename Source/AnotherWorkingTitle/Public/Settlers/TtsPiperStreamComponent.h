// (c) 2025 MK

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HAL/PlatformProcess.h"
#include "TtsPiperStreamComponent.generated.h"

class UAudioComponent;
class USoundAttenuation;
class USoundWaveProcedural;

USTRUCT(BlueprintType)
struct FPiperTtsResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) bool bOk = false;
	UPROPERTY(BlueprintReadOnly) int64 RequestId = -1;
	UPROPERTY(BlueprintReadOnly) int32 SampleRate = 0;
	UPROPERTY(BlueprintReadOnly) FString Error;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTtsFinished, const FPiperTtsResult&, Result);

UCLASS(ClassGroup=(Audio), meta=(BlueprintSpawnableComponent))
class ANOTHERWORKINGTITLE_API UTtsPiperStreamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTtsPiperStreamComponent();

	UFUNCTION(BlueprintCallable) void Speak(const FString& Text);
	UFUNCTION(BlueprintCallable) void Stop();

	UPROPERTY(BlueprintAssignable) FOnTtsFinished OnTtsFinished;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void EnsureAudioComponent();
	USceneComponent* ResolveAttachComponent() const;

	bool StartWorker();
	void StopWorker();

	void PumpStdout();
	void HandleStdoutLine(const FString& Line);

	void HandleStart(int64 Id, int32 Sr, int32 Ch);
	void HandleChunk(int64 Id, const FString& B64);
	void HandleDone(int64 Id);
	void HandleError(int64 Id, const FString& Err);

	void StartPlaybackIfBuffered();

private:
	// ---- Paths ----
	UPROPERTY(EditAnywhere, Category="TTS|Paths") FString PythonExePath;
	UPROPERTY(EditAnywhere, Category="TTS|Paths") FString BridgeScriptPath; // piper_stream_bridge.py
	UPROPERTY(EditAnywhere, Category="TTS|Paths") FString ModelOnnxPath;
	UPROPERTY(EditAnywhere, Category="TTS|Paths") FString OutputDirectory; // unused here; can keep empty

	// ---- 3D ----
	UPROPERTY(EditAnywhere, Category="TTS|3D") TObjectPtr<USceneComponent> AttachComponentOverride = nullptr;
	UPROPERTY(EditAnywhere, Category="TTS|3D") FName AttachSocketName = TEXT("head");
	UPROPERTY(EditAnywhere, Category="TTS|3D") TObjectPtr<USoundAttenuation> AttenuationAsset = nullptr;

	// ---- Latency tuning ----
	UPROPERTY(EditAnywhere, Category="TTS|Latency") int32 PrebufferMs = 120; // start playback after this much audio is queued

	// ---- Runtime ----
	FProcHandle ProcHandle;
	void* StdOutRead = nullptr;
	void* StdOutWriteChild = nullptr;
	void* StdInReadChild = nullptr;
	void* StdInWrite = nullptr;
	FString StdoutAccum;

	std::atomic<int64> NextId { 1 };

	UPROPERTY(Transient) 
	UAudioComponent* AudioComp = nullptr;

	// Single speaker rule
	static TWeakObjectPtr<UTtsPiperStreamComponent> ActiveSpeaker;

	// Streaming state (only one active request at a time for your use-case)
	int64 ActiveRequestId = -1;
	int64 Epoch = 0;

	UPROPERTY(Transient) USoundWaveProcedural* ActiveWave = nullptr;
	int32 ActiveSampleRate = 0;
	int32 ActiveNumChannels = 0;
	int32 QueuedBytes = 0;
	bool bPlaying = false;
};
