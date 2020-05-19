// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Engine.h"
#include "ModuleManager.h"


DECLARE_LOG_CATEGORY_EXTERN(HokuyoLog, Log, All);

class FHokuyoModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};