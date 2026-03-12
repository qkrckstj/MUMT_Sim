#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UAV_Moving.generated.h"

class UJSBSimMovementComponent;
class APawn;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUMT_SIM_API UUAV_Moving : public UActorComponent
{
	GENERATED_BODY()

public:
	UUAV_Moving();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	void AcquireRefs();
	void EnsureEngineInitialized();
	void ApplySpeedFollow(float DeltaTime);
	void ApplyFormationControl(float DeltaTime);

private:
	UPROPERTY()
	UJSBSimMovementComponent* JSBSim = nullptr;

	UPROPERTY()
	APawn* LeaderPawn = nullptr;

	UPROPERTY()
	APawn* OwnerPawn = nullptr;

	bool bEngineInitialized = false;

public:
	// ====== Tuning ======

	UPROPERTY(EditAnywhere, Category = "UAV")
	FVector FormationOffset = FVector(-300000, 0, 50000);

	UPROPERTY(EditAnywhere, Category = "UAV")
	float CruiseThrottle = 0.75f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	float SpeedFollowGain = 0.002f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	float LateralGain = 0.000005f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	float VerticalGain = 0.000005f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	float MaxThrottle = 1.0f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	float MinThrottle = 0.1f;

	UPROPERTY(EditAnywhere, Category = "UAV")
	bool bDebugLog = true;
};
