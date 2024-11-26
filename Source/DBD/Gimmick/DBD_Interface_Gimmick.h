// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DBD_Interface_Gimmick.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDBD_Interface_Gimmick : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DBD_API IDBD_Interface_Gimmick
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Gimimick 이 상호작용할 때 호출되는 함수
	virtual void Interaction(AActor* Caller = nullptr) = 0;
	virtual void FailedInteraction() = 0;
	virtual FString GetGimmickName() = 0;
	virtual FString GetInteractKey() = 0;
};
