// (c) 2025 MK

#include "Settlers/TtsPiperStreamComponent.h"

#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Dom/JsonObject.h"
#include "Misc/Base64.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundWaveProcedural.h"

TWeakObjectPtr<UTtsPiperStreamComponent> UTtsPiperStreamComponent::ActiveSpeaker;

UTtsPiperStreamComponent::UTtsPiperStreamComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTtsPiperStreamComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureAudioComponent();
}

void UTtsPiperStreamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ActiveSpeaker.Get() == this)
	{
		ActiveSpeaker = nullptr;
	}
	StopWorker();
	Super::EndPlay(EndPlayReason);
}

void UTtsPiperStreamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PumpStdout();
}

void UTtsPiperStreamComponent::EnsureAudioComponent()
{
	if (AudioComp) 
		return;

	AudioComp = NewObject<UAudioComponent>(GetOwner());
	AudioComp->bAutoActivate = false;
	AudioComp->RegisterComponent();
	AudioComp->bAllowSpatialization = true;

	if (AttenuationAsset)
	{
		AudioComp->AttenuationSettings = AttenuationAsset;
	}

	if (USceneComponent* AttachComp = ResolveAttachComponent())
	{
		AudioComp->AttachToComponent(
			AttachComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AttachSocketName
		);
	}
	else if (GetOwner() && GetOwner()->GetRootComponent())
	{
		AudioComp->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

USceneComponent* UTtsPiperStreamComponent::ResolveAttachComponent() const
{
	if (AttachComponentOverride)
	{
		return AttachComponentOverride;
	}

	TArray<USkeletalMeshComponent*> SkelMeshes;
	GetOwner()->GetComponents<USkeletalMeshComponent>(SkelMeshes);

	for (USkeletalMeshComponent* Skel : SkelMeshes)
	{
		if (Skel && Skel->DoesSocketExist(AttachSocketName))
		{
			return Skel;
		}
	}
	for (USkeletalMeshComponent* Skel : SkelMeshes)
	{
		if (Skel) 
			return Skel;
	}
	return nullptr;
}

bool UTtsPiperStreamComponent::StartWorker()
{
	if (ProcHandle.IsValid()) 
		return true;

	if (PythonExePath.IsEmpty() || BridgeScriptPath.IsEmpty() || ModelOnnxPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[TTS] Missing paths (PythonExePath / BridgeScriptPath / ModelOnnxPath)."));
		return false;
	}

	// stdout: parent reads, child writes  -> child write must be inheritable
	if (!FPlatformProcess::CreatePipe(StdOutRead, StdOutWriteChild, /*bWritePipeLocal*/ false))
	{
		return false;
	}

	// stdin: child reads, parent writes -> parent write should be local
	if (!FPlatformProcess::CreatePipe(StdInReadChild, StdInWrite, /*bWritePipeLocal*/ true))
	{
		FPlatformProcess::ClosePipe(StdOutRead, StdOutWriteChild);
		StdOutRead = StdOutWriteChild = nullptr;
		return false;
	}

	const FString Args = FString::Printf(TEXT("-u \"%s\" \"%s\""), *BridgeScriptPath, *ModelOnnxPath);
	//const FString Args = TEXT("-u -c \"import sys; print('READY', flush=True); sys.stdout.flush(); line=sys.stdin.readline(); print('ECHO:'+line.strip(), flush=True)\"");

	uint32 ProcId = 0;
	ProcHandle = FPlatformProcess::CreateProc(
		*PythonExePath,
		*Args,
		false, true, true,
		&ProcId,
		0,
		nullptr,
		StdOutWriteChild,  // child stdout write (must be inheritable)
		StdInReadChild,    // child stdin read,
		StdOutWriteChild
	);

	if (!ProcHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[TTS] Failed to start worker process."));
		StopWorker();
		return false;
	}

	if (!FPlatformProcess::IsProcRunning(ProcHandle))
	{
		int32 RC = 0;
		const bool bHasRC = FPlatformProcess::GetProcReturnCode(ProcHandle, &RC);
		UE_LOG(LogTemp, Error, TEXT("[TTS] Worker not running. hasRC=%d rc=%d"), bHasRC ? 1 : 0, RC);
	}

	return true;
}

void UTtsPiperStreamComponent::StopWorker()
{
	if (ProcHandle.IsValid())
	{
		FPlatformProcess::TerminateProc(ProcHandle, true);
		FPlatformProcess::CloseProc(ProcHandle);
		ProcHandle.Reset();
	}

	if (StdOutRead || StdOutWriteChild)
	{
		FPlatformProcess::ClosePipe(StdOutRead, StdOutWriteChild);
		StdOutRead = StdOutWriteChild = nullptr;
	}
	if (StdInReadChild || StdInWrite)
	{
		FPlatformProcess::ClosePipe(StdInReadChild, StdInWrite);
		StdInReadChild = StdInWrite = nullptr;
	}

	StdoutAccum.Empty();
}

void UTtsPiperStreamComponent::Speak(const FString& Text)
{
	EnsureAudioComponent();

	// Single-speaker rule
	if (UTtsPiperStreamComponent* Prev = ActiveSpeaker.Get())
	{
		if (Prev != this)
		{
			Prev->Stop();
		}
	}
	ActiveSpeaker = this;

	if (!StartWorker())
		return;

	// Invalidate previous streaming playback
	Stop();

	const int64 Id = NextId.fetch_add(1);
	ActiveRequestId = Id;

	TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
	Obj->SetNumberField(TEXT("id"), (double)Id);
	Obj->SetStringField(TEXT("text"), Text);

	FString Line;
	{
		TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
			TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Line);
		FJsonSerializer::Serialize(Obj, Writer);
	}

	// Safety: keine echten Newlines im Payload (sollte mit condensed eh nicht vorkommen)
	Line.ReplaceInline(TEXT("\r"), TEXT(""));
	Line.ReplaceInline(TEXT("\n"), TEXT(""));

	Line += TEXT("\n");
	FPlatformProcess::WritePipe(StdInWrite, Line);
}

void UTtsPiperStreamComponent::Stop()
{
	if (AudioComp)
	{
		AudioComp->Stop();
	}

	// bump epoch -> ignore any in-flight lines from old request
	Epoch++;

	ActiveRequestId = -1;
	ActiveWave = nullptr;
	ActiveSampleRate = 0;
	ActiveNumChannels = 0;
	QueuedBytes = 0;
	bPlaying = false;
}

void UTtsPiperStreamComponent::PumpStdout()
{
	if (!ProcHandle.IsValid() || !StdOutRead)
		return;

	const FString Chunk = FPlatformProcess::ReadPipe(StdOutRead);
	if (Chunk.IsEmpty())
		return;
	
	StdoutAccum += Chunk;

	int32 NewlineIdx = INDEX_NONE;
	while (StdoutAccum.FindChar(TEXT('\n'), NewlineIdx))
	{
		const FString Line = StdoutAccum.Left(NewlineIdx).TrimStartAndEnd();
		StdoutAccum = StdoutAccum.Mid(NewlineIdx + 1);

		if (!Line.IsEmpty())
		{
			HandleStdoutLine(Line);
		}
	}
}

void UTtsPiperStreamComponent::HandleStdoutLine(const FString& Line)
{
	TSharedPtr<FJsonObject> Obj;
	{
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Line);
		if (!FJsonSerializer::Deserialize(Reader, Obj) || !Obj.IsValid())
			return;
	}

	const FString Type = Obj->GetStringField(TEXT("type"));
	const int64 Id = (int64)Obj->GetNumberField(TEXT("id"));

	// Only accept current request; ignore stale
	// (we also use Epoch for hard invalidation via Stop())
	if (ActiveRequestId != Id)
		return;

	if (Type == TEXT("start"))
	{
		const int32 Sr = (int32)Obj->GetNumberField(TEXT("sr"));
		const int32 Ch = (int32)Obj->GetNumberField(TEXT("ch"));
		HandleStart(Id, Sr, Ch);
	}
	else if (Type == TEXT("chunk"))
	{
		const FString B64 = Obj->GetStringField(TEXT("b64"));
		HandleChunk(Id, B64);
	}
	else if (Type == TEXT("done"))
	{
		HandleDone(Id);
	}
	else if (Type == TEXT("error"))
	{
		FString Err;
		Obj->TryGetStringField(TEXT("error"), Err);
		HandleError(Id, Err);
	}
}

void UTtsPiperStreamComponent::HandleStart(int64 Id, int32 Sr, int32 Ch)
{
	ActiveSampleRate = Sr;
	ActiveNumChannels = Ch;
	QueuedBytes = 0;
	bPlaying = false;

	ActiveWave = NewObject<USoundWaveProcedural>(this);
	ActiveWave->bProcedural = true;
	ActiveWave->NumChannels = Ch;
	ActiveWave->SetSampleRate(Sr);

	AudioComp->SetSound(ActiveWave);
	// Play later once buffered
}

void UTtsPiperStreamComponent::HandleChunk(int64 Id, const FString& B64)
{
	if (!ActiveWave) return;

	TArray<uint8> PcmBytes;
	if (!FBase64::Decode(B64, PcmBytes))
		return;

	if (PcmBytes.Num() == 0) return;

	ActiveWave->QueueAudio(PcmBytes.GetData(), PcmBytes.Num());
	QueuedBytes += PcmBytes.Num();

	StartPlaybackIfBuffered();
}

void UTtsPiperStreamComponent::StartPlaybackIfBuffered()
{
	if (bPlaying || !AudioComp || !ActiveWave) return;
	if (ActiveSampleRate <= 0 || ActiveNumChannels <= 0) return;

	const int32 BytesPerFrame = 2 * ActiveNumChannels; // int16
	const int32 NeededBytes = (ActiveSampleRate * BytesPerFrame * PrebufferMs) / 1000;

	if (QueuedBytes >= NeededBytes)
	{
		AudioComp->Play();
		bPlaying = true;
	}
}

void UTtsPiperStreamComponent::HandleDone(int64 Id)
{
	FPiperTtsResult R;
	R.bOk = true;
	R.RequestId = Id;
	R.SampleRate = ActiveSampleRate;

	OnTtsFinished.Broadcast(R);

	// Keep worker alive; reset active request state
	ActiveRequestId = -1;
}

void UTtsPiperStreamComponent::HandleError(int64 Id, const FString& Err)
{
	FPiperTtsResult R;
	R.bOk = false;
	R.RequestId = Id;
	R.Error = Err;

	OnTtsFinished.Broadcast(R);

	ActiveRequestId = -1;
}
