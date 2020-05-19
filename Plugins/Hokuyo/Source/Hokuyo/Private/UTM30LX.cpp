// Fill out your copyright notice in the Description page of Project Settings.

#include "UTM30LX.h"


AUTM30LX::AUTM30LX(const FObjectInitializer& ObjectInitializer) :
    Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    UTM30LX = CreateDefaultSubobject<UStaticMeshComponent>
        (TEXT("UTM30LX"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sensor
    (TEXT("StaticMesh'/Hokuyo/UTM30LX/Meshes/Hokuyo_UTM-30LX'"));
    if (Sensor.Succeeded())
    {
        UTM30LX->SetStaticMesh(Sensor.Object);
    }
    UTM30LX->AttachToComponent(RootComponent,
        FAttachmentTransformRules::KeepRelativeTransform);
    UTM30LX->SetSimulatePhysics(true);
    UTM30LX->BodyInstance.SetMassOverride(Weight);
}


void AUTM30LX::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(HokuyoLog, Warning, TEXT("Thread %s Created"), *ThreadName);
    Thread = FRunnableThread::Create(this, *ThreadName);
    rng = std::mt19937(rd());
    uni = std::normal_distribution<float>(
        NoiseDistributionMean,
        NoiseDistributionStdev);
}


void AUTM30LX::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    Stop();
}


void AUTM30LX::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    Mutex.Lock();
    auto Start = UTM30LX->GetComponentLocation();
    Start += UTM30LX->GetUpVector() * HeightOffset;
    for (const auto& Point : CurrentScan)
    {
#if 0
        DrawDebugLine(
            GetWorld(),
            Start,
            Point,
            FColor::Red);
#endif
        DrawDebugPoint(
            GetWorld(),
            Point,
            PointSize,
            FColor::Red,
            0.0f);
    }
    Mutex.Unlock();
}


bool AUTM30LX::Init()
{
    UE_LOG(HokuyoLog, Warning, TEXT("Thread %s Started"), *ThreadName);
    bThreadIsRunning = true;
    return true;
}


uint32 AUTM30LX::Run()
{
    while (bThreadIsRunning)
    {
        auto HitResult = FHitResult();
        FCollisionQueryParams CollisionParams;
        CollisionParams.AddIgnoredActor(this);
        Mutex.Lock();
        CurrentScan.Empty(CurrentScan.Max());
        auto Start = UTM30LX->GetComponentLocation();
        Start += UTM30LX->GetUpVector() * HeightOffset;
        int pointCount = 0;
        for (auto i = -VerticalFieldOfView / 2.0f; i < VerticalFieldOfView / 2.0f; i += ElevationAngularResolution)
        {
            const auto yMeasurement = UTM30LX->GetForwardVector().RotateAngleAxis(
                i, UTM30LX->GetRightVector());
            for (auto j = 0.0f; j < HorizontalFieldOfView; j += AzimuthAngularResolution)
            {
                const auto Noise = uni(rng);
                const auto xMeasurement = UTM30LX->GetForwardVector().RotateAngleAxis(
                    j + RotationStartOffset, UTM30LX->GetUpVector());
                auto Measurement = xMeasurement;
                Measurement.Z = yMeasurement.Z;
                Measurement *= MaxRange;
                if (GetWorld()->LineTraceSingleByChannel(
                    HitResult,
                    Start,
                    Measurement,
                    ECollisionChannel::ECC_Visibility,
                    CollisionParams))
                {
                    Measurement = HitResult.Location;
                    if (bApplyNoise)
                    {
                        Measurement.X += Noise;
                        Measurement.Z += Noise;
                    }
                    auto Distance = FVector::Dist(Start, Measurement);
                    if (Distance < MaxRange && Distance > MinRange)
                    {
                        CurrentScan.Add(Measurement);
                    }
                }
                else
                {
                    CurrentScan.Add(Measurement);
                }
                pointCount++;
            }
        }
        Mutex.Unlock();
        UE_LOG(HokuyoLog, Log, TEXT("PointCloud Size: %d"), pointCount);
        FPlatformProcess::Sleep(ScanSpeed);
    }
    return 0;
}


void AUTM30LX::Stop()
{
    bThreadIsRunning = false;
    UE_LOG(HokuyoLog, Warning, TEXT("Thread %s Stopped"), *ThreadName);
}