// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OWSAbilityTask_SpawnArrow.h"
#include "OWSAdvancedArrow.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "OWSAbilityTask_SpawnArrowV2.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnArrowDelegateV2, AActor*, SpawnedActor);
/**
 * 
 */
UCLASS()
class OWSPLUGIN_API UOWSAbilityTask_SpawnArrowV2 : public UAbilityTask
{
	GENERATED_BODY()
	
	public:

	UPROPERTY(BlueprintAssignable)
	FSpawnArrowDelegateV2	Success;

	/** Called when we can't spawn: on clients or potentially on server if they fail to spawn (rare) */
	UPROPERTY(BlueprintAssignable)
	FSpawnArrowDelegateV2	DidNotSpawn;

	/** Spawn new projectile on the network authority (server) */
	UFUNCTION(BlueprintCallable, meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category="Ability|GAS Companion|Tasks")
	static UOWSAbilityTask_SpawnArrowV2* SpawnArrowV2(UGameplayAbility* OwningAbility, FTransform  SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, TSubclassOf<AOWSAdvancedArrow> Class);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	bool BeginSpawningActor(UGameplayAbility* OwningAbility, TSubclassOf<AOWSAdvancedArrow> Class, AOWSAdvancedArrow*& SpawnedActor);
	
	void SpawnDelayedFakeArrow();

	void GetAimTransform(FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"), Category = "Abilities")
	void FinishSpawningActor(UGameplayAbility* OwningAbility, AOWSAdvancedArrow* SpawnedActor);

	protected:
	FGameplayAbilityTargetDataHandle CachedTargetDataHandle;
	FTransform CachedSpawnTransform;
	ESpawnActorCollisionHandlingMethod CachedCollisionHandlingOverride;

	UPROPERTY()
	bool bUseAimCamera;

	UPROPERTY()
	bool bIgnoreAimCameraPitch;

	UPROPERTY()
	FName nameStartingSocketName;
	
	UPROPERTY()
	float fForwardOffsetFromSocket;

	UPROPERTY()
	bool bUseFixedStartingLocationRotation;

	UPROPERTY()
	FVector StartingLocation;

	UPROPERTY()
	FRotator StartingRotation;

	/** Delayed Arrow information */
	UPROPERTY()
	FDelayedArrowInfo DelayedArrow;

	FTimerHandle SpawnDelayedFakeProjHandle;
};
