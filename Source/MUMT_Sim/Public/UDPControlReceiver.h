#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "UDPControlReceiver.generated.h"

UCLASS()
class MUMT_SIM_API AUDPControlReceiver : public AActor
{
    GENERATED_BODY()

public:
    AUDPControlReceiver();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    // ===== ?섏떊 =====
    bool StartUDPReceiver();
    void StopUDPReceiver();
    void ReceiveUDPData();
    void ParseCommand(const FString& Message);

    // ===== ?≪떊 =====
    bool StartUDPSender();
    void StopUDPSender();
    void SendStateToPython();

    // ===== Pawn 泥섎━ =====
    APawn* FindTargetPawn();
    bool SetBlueprintNumber(APawn* Pawn, const FName VarName, double Value);

private:
    // ?섏떊???뚯폆
    FSocket* ListenSocket = nullptr;

    // ?≪떊???뚯폆
    FSocket* SendSocket = nullptr;

    // Python 二쇱냼
    TSharedPtr<FInternetAddr> PythonAddr;

    // 罹먯떆?????Pawn
    APawn* CachedTargetPawn = nullptr;

    // ?곹깭 ?≪떊 ??대㉧
    float StateSendAccumulator = 0.0f;

    // ?띾룄 怨꾩궛???댁쟾 ?곹깭
    FVector PrevLocation = FVector::ZeroVector;
    bool bHasPrevLocation = false;
    double PrevStateSendTime = 0.0;

public:
    // ===== ?섏떊 ?ㅼ젙 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP|Receiver")
    int32 ListenPort = 5005;

    // ===== ?≪떊 ?ㅼ젙 =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP|Sender")
    FString PythonIP = TEXT("127.0.0.1");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP|Sender")
    int32 PythonStatePort = 5006;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP|Sender")
    float StateSendInterval = 0.05f; // 20Hz

    // ===== ?쒖뼱 ???Pawn =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP|Target")
    FString TargetPawnName = TEXT("F16_UAV");

    // ===== ?섏떊??議곗쥌媛?=====
    UPROPERTY(BlueprintReadOnly, Category = "UDP|Control")
    float Roll = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "UDP|Control")
    float Pitch = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "UDP|Control")
    float Yaw = 0.0f;

    UPROPERTY(BlueprintReadOnly, Category = "UDP|Control")
    float Throttle = 0.0f;
};
