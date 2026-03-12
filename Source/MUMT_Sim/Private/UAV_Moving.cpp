#include "UAV_Moving.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

#include "JSBSimMovementComponent.h"

UUAV_Moving::UUAV_Moving()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUAV_Moving::BeginPlay()
{
	Super::BeginPlay();

	AcquireRefs();

	if (!OwnerPawn || !LeaderPawn || !JSBSim)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAV refs missing"));
		return;
	}

	// ===== UE Velocity 복사 (공중 정지 방지 핵심) =====

	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(OwnerPawn->GetRootComponent()))
	{
		const FVector LeaderVel = LeaderPawn->GetVelocity();
		Root->SetPhysicsLinearVelocity(LeaderVel);

		if (bDebugLog)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAV Initial Velocity Injected"));
		}
	}

	EnsureEngineInitialized();
}

void UUAV_Moving::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerPawn || !LeaderPawn || !JSBSim)
	{
		AcquireRefs();
	}

	if (!OwnerPawn || !LeaderPawn || !JSBSim)
		return;

	if (!bEngineInitialized)
	{
		EnsureEngineInitialized();
	}

	ApplySpeedFollow(DeltaTime);
	ApplyFormationControl(DeltaTime);
}

void UUAV_Moving::AcquireRefs()
{
	OwnerPawn = Cast<APawn>(GetOwner());

	if (OwnerPawn && !JSBSim)
	{
		JSBSim = OwnerPawn->FindComponentByClass<UJSBSimMovementComponent>();
	}

	if (!LeaderPawn)
	{
		LeaderPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
}

void UUAV_Moving::EnsureEngineInitialized()
{
	if (!JSBSim) return;

	if (JSBSim->EngineCommands.Num() == 0)
		return;

	auto& Engine = JSBSim->EngineCommands[0];

	Engine.Mixture = 1.0f;
	Engine.Running = true;
	Engine.Starter = true;
	Engine.Throttle = CruiseThrottle;

	bEngineInitialized = true;

	UE_LOG(LogTemp, Warning, TEXT("UAV Engine Initialized"));
}

void UUAV_Moving::ApplySpeedFollow(float DeltaTime)
{
	if (JSBSim->EngineCommands.Num() == 0) return;

	float LeaderSpeed = LeaderPawn->GetVelocity().Size();
	float MySpeed = OwnerPawn->GetVelocity().Size();

	float Error = (LeaderSpeed - MySpeed) * 0.01f;

	float TargetThrottle =
		CruiseThrottle + Error * SpeedFollowGain;

	TargetThrottle =
		FMath::Clamp(TargetThrottle, MinThrottle, MaxThrottle);

	JSBSim->EngineCommands[0].Throttle = TargetThrottle;
}

void UUAV_Moving::ApplyFormationControl(float DeltaTime)
{
	AActor* UAV = GetOwner();

	const FVector TargetPos =
		LeaderPawn->GetActorLocation() +
		LeaderPawn->GetActorRotation().RotateVector(FormationOffset);

	const FVector Error = TargetPos - UAV->GetActorLocation();

	const FVector Right = UAV->GetActorRightVector();
	const FVector Up = UAV->GetActorUpVector();

	float LateralError = FVector::DotProduct(Right, Error);
	float VerticalError = FVector::DotProduct(Up, Error);

	JSBSim->Commands.Aileron =
		FMath::Clamp(LateralError * LateralGain, -1.f, 1.f);

	JSBSim->Commands.Elevator =
		FMath::Clamp(-VerticalError * VerticalGain, -1.f, 1.f);
}
