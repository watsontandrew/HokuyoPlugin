// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Hokuyo.h"
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"

#define LOCTEXT_NAMESPACE "FHokuyoModule"

DEFINE_LOG_CATEGORY(HokuyoLog);

void FHokuyoModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    UE_LOG(HokuyoLog, Warning, TEXT("Hokuyo Module Loaded!"));
}

void FHokuyoModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
    UE_LOG(HokuyoLog, Warning, TEXT("Hokuyo Module UnLoaded!"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FHokuyoModule, Hokuyo)