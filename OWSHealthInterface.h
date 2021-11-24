// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "OWSAdvancedArrow.h"
#include "UObject/Interface.h"
#include "OWSHealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOWSHealthInterface : public UAbilitySystemInterface
{
	GENERATED_BODY()
	
};

/**
 * 
 */
class OWSPLUGIN_API IOWSHealthInterface : public IAbilitySystemInterface
{
	
	GENERATED_BODY()

	

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return nullptr;
	};

	

	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleArrowDamage(AOWSAdvancedArrow* Arrow, bool UseExplosionEffect, FVector HitLocation);
	
	
};
