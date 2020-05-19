// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "HAL/Runnable.h"

#include <random>

#include "UTM30LX.generated.h"


UCLASS()
class HOKUYO_API AUTM30LX : public AActor, public FRunnable
{
    GENERATED_BODY()

public:
    AUTM30LX(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void BeginPlay() override final;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override final;

public:
    virtual void Tick(float DeltaTime) override final;
    virtual bool Init() override final;
    virtual uint32 Run() override final;
    virtual void Stop() override final;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        UStaticMeshComponent* UTM30LX;

private:

    // Used for building scan lines
    FRunnableThread* Thread = nullptr;
    // Unique thread name
    FString ThreadName = StaticClass()->GetName()
        + "_" + FString::FromInt(this->GetUniqueID());
    // Thread control flag
    FThreadSafeBool bThreadIsRunning = false;
    // Enable noise simulation
    UPROPERTY(EditAnywhere, Category = "Scanning")
    bool bApplyNoise = false;
    // Scanning thread mutex for data synchronization
    FCriticalSection Mutex;
    // Weight in kilograms
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float Weight = 0.21f;
    // Minimum detection range in centimeters.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float MinRange = 10.f;
    // Maximum detection range in centimeters.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float MaxRange = 200000.f;
    // Angular resolution of the sensor's output along the x axis in degrees.
    UPROPERTY(EditAnywhere, Category = "Scanning", meta = (ClampMin = "0.06", ClampMax = "1.0", UIMin = "0.06", UIMax = "1.0"))
    float AzimuthAngularResolution = 0.06f;
    // Angular resolution of the sensor's output along the y axis in degrees.
    UPROPERTY(EditAnywhere, Category = "Scanning", meta = (ClampMin = "0.06", ClampMax = "1.0", UIMin = "0.06", UIMax = "1.0"))
    float ElevationAngularResolution = 0.46875f;
    // Usable horizontal field-of-view of the sensor in degrees where 0 degrees is directly outwards from sensor.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float HorizontalFieldOfView = 120.0f;
    // Usable vertical field-of-view of the sensor in degrees where 0 degrees is the horizon.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float VerticalFieldOfView = 30.0f;
    // Measurement's output resolution in millimeters.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float OutputResolution = 1.f;
    // Time to complete a single scan in seconds.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float ScanSpeed = 0.1;
    // Mean used in noise calculation of normal distribution.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float NoiseDistributionMean = 0.f;
    // Standard deviation used in noise normal distribution.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float NoiseDistributionStdev = 2.f;
    // Repeated (distance) accuracy of the sensor in centimeters.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float TypicalAccuracy = 2.f;
    // Sensor light reception height offset (z) in centimeters.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float HeightOffset = 5.2f;
    // Starting offset from center in degrees.
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float RotationStartOffset = 300.f;
    // Point Size
    UPROPERTY(EditAnywhere, Category = "Scanning")
    float PointSize = 1.0f;
    // Number of horizontal measurement steps in a single scan (frame).
    //static uint32 NumHorizontalSteps = (HorizontalFieldOfView / AzimuthAngularResolution);
    // Number of vertical measurement steps in a single scan (frame).
    //static uint32 NumVerticalSteps = (VerticalFieldOfView / ElevationAngularResolution);

    std::random_device rd;     // used once to initialize (seed) engine
    std::mt19937 rng;    // random-number engine Mersenne-Twister
    std::normal_distribution<float> uni;

    //typedef TArray<FVector, TInlineAllocator<NumHorizontalSteps * NumVerticalSteps>> UTM30LXScan;
    using UTM30LXScan = TArray<FVector, TInlineAllocator<2000 * 500>>;
    UTM30LXScan CurrentScan;
};