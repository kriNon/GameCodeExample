// Fill out your copyright notice in the Description page of Project Settings.


#include "OWSAbilityTask_SpawnArrowV2.h"

#include "AbilitySystemComponent.h"

UOWSAbilityTask_SpawnArrowV2* UOWSAbilityTask_SpawnArrowV2::SpawnArrowV2(UGameplayAbility* OwningAbility,
                                                                         FTransform SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride,
                                                                         TSubclassOf<AOWSAdvancedArrow> Class)
{
	UOWSAbilityTask_SpawnArrowV2* MyObj = NewAbilityTask<UOWSAbilityTask_SpawnArrowV2>(OwningAbility);
	MyObj->CachedSpawnTransform = SpawnTransform;
	MyObj->CachedCollisionHandlingOverride = CollisionHandlingOverride;
	return MyObj;
}

bool UOWSAbilityTask_SpawnArrowV2::BeginSpawningActor(UGameplayAbility* OwningAbility,
	TSubclassOf<AOWSAdvancedArrow> Class, AOWSAdvancedArrow*& SpawnedActor)
{
	if (!Ability)
	{
		return false;
	}
	if (Ability->GetCurrentActorInfo()->IsNetAuthority() && ShouldBroadcastAbilityTaskDelegates()) //SPAWN SERVERSIDE
	{
		UWorld* const World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Spawn Actor on Server"));
			SpawnedActor = World->SpawnActorDeferred<AOWSAdvancedArrow>(Class, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
			//AOWSAdvancedArrow* SpawnedArrow = Cast<AOWSAdvancedArrow>(SpawnedActor);
			//SpawnedArrow->ArrowMovement->ProjectileGravity = -980;
			//SpawnedArrow->ArrowMovement->InitialSpeed = 5000;
			//SpawnedArrow->FinishSpawning(CachedSpawnTransform);
			//For the actor that spawns on the server, the gravity actually works properly, but when it gets replicated to clients, they don't see it.
			
		}
	}
	// else //SPAWN CLIENTSIDE
	// {
	// 	//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Client Start"));
	//
	// 	UWorld* const World = GEngine->GetWorldFromContextObject(OwningAbility, EGetWorldErrorMode::LogAndReturnNull);
	// 	if (World)
	// 	{
	// 		AOWSPlayerController* OwningPlayer = Cast<AOWSPlayerController>(Ability->GetCurrentActorInfo()->PlayerController);
	//
	// 		float CatchupTickDelta = (OwningPlayer ? OwningPlayer->GetPredictionTime() : 0.f);
	//
	//
	// 		if ((CatchupTickDelta > 0.f))
	// 		{
	// 			//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Client CatchupTickDelta > 0"));
	//
	// 			float SleepTime = OwningPlayer->GetArrowSleepTime();
	// 			if (SleepTime > 0.f)
	// 			{
	// 				//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Client SleepTime > 0"));
	//
	// 				//UWorld* World = GetWorld();
	// 				// lag is so high need to delay spawn
	// 				if (!World->GetTimerManager().IsTimerActive(SpawnDelayedFakeProjHandle))
	// 				{
	// 					FTransform SpawnTransform;
	// 					GetAimTransform(SpawnTransform);
	// 					DelayedArrow.ArrowClass = Class;
	// 					DelayedArrow.SpawnLocation = SpawnTransform.GetLocation();
	// 					DelayedArrow.SpawnRotation = SpawnTransform.Rotator();
	//
	// 					//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Schedule Delayed Fake Arrow for %f"), SleepTime);
	// 					World->GetTimerManager().SetTimer(SpawnDelayedFakeProjHandle, this, &UOWSAbilityTask_SpawnArrowV2::SpawnDelayedFakeArrow, SleepTime, false);
	//
	// 					SpawnedActor = NULL;
	// 					return false;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Spawn fake Arrow on Client Now!"));
	//
	// 				SpawnedActor = World->SpawnActorDeferred<AOWSAdvancedArrow>(Class, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	// 				
	// 			}
	// 		}
	// 		else
	// 		{
	// 			//UE_LOG(OWS, Verbose, TEXT("BeginSpawningActor: Client won't spawn a fake!"));
	//
	// 			return false;
	// 		}
	// 	}
	// }

	if (SpawnedActor == nullptr)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(nullptr);
		}
		return false;
	}



	return true;
}


void UOWSAbilityTask_SpawnArrowV2::SpawnDelayedFakeArrow()
{
	AOWSPlayerController* OwningPlayer = Cast<AOWSPlayerController>(Ability->GetCurrentActorInfo()->PlayerController);
	if (OwningPlayer)
	{
		if (IsLocallyControlled())
		{
			//UE_LOG(OWS, Verbose, TEXT("SpawnDelayedFakeArrow Client Owner: %s"), *OwningPlayer->GetName());
		}
		else
		{
			//UE_LOG(OWS, Verbose, TEXT("SpawnDelayedFakeArrow Server Owner: %s"), *OwningPlayer->GetName());
		}

		float CatchupTickDelta = (OwningPlayer ? OwningPlayer->GetPredictionTime() : 0.f);

		APawn* MyPawn = Cast<APawn>(Ability->GetCurrentActorInfo()->AvatarActor);
		FActorSpawnParameters Params;
		Params.Instigator = MyPawn;
		Params.Owner = MyPawn;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//AOWSAdvancedArrow* NewArrow = GetWorld()->SpawnActorDeferred<AOWSAdvancedArrow>(DelayedArrow.ArrowClass, FTransform::Identity, NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);		
		AOWSAdvancedArrow* NewArrow = GetWorld()->SpawnActor<AOWSAdvancedArrow>(DelayedArrow.ArrowClass, DelayedArrow.SpawnLocation, DelayedArrow.SpawnRotation, Params);
		//AOWSAdvancedArrow* NewArrow = GetWorld()->SpawnActorDeferred<AOWSAdvancedArrow>(DelayedArrow.ArrowClass, FTransform(DelayedArrow.SpawnRotation, DelayedArrow.SpawnLocation), NULL, NULL, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (NewArrow)
		{
			NewArrow->InitFakeArrow(OwningPlayer);

			//UE_LOG(OWS, Verbose, TEXT("Delayed fake Old Life Span: %f"), NewArrow->GetLifeSpan());

			//float NewLifeSpan = FMath::Min(NewArrow->GetLifeSpan(), 2.f * FMath::Max(0.f, CatchupTickDelta));

			float NewLifeSpan = FMath::Min(NewArrow->GetLifeSpan(),
			                               0.002f * FMath::Max(
				                               0.f, OwningPlayer->MaxPredictionPing + OwningPlayer->
				                               PredictionFudgeFactor));

			//UE_LOG(OWS, Verbose, TEXT("Delayed fake Life Span: %f"), NewLifeSpan);

			NewArrow->SetLifeSpan(NewLifeSpan);
			//NewArrow->ProjectileGravityValue = -980;
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				Success.Broadcast(NewArrow);
			}
		}
	}

	EndTask();
}

void UOWSAbilityTask_SpawnArrowV2::GetAimTransform(FTransform& SpawnTransform)
{
    bool bTransformSet = false;
	if (bUseAimCamera)
	{
		AOWSPlayerController* OwningPlayer = Cast<AOWSPlayerController>(Ability->GetCurrentActorInfo()->PlayerController);
		check(OwningPlayer)

			if (OwningPlayer)
			{
				FVector ViewStart;
				FRotator ViewRot;

				OwningPlayer->GetPlayerViewPoint(ViewStart, ViewRot);

				//Set pitch to zero to ignore it
				if (bIgnoreAimCameraPitch)
				{
					ViewRot.Pitch = 0.f;
				}

				const FVector ViewDirUnit = ViewRot.Vector().GetSafeNormal();

				ACharacter* MyCharacter = Cast<ACharacter>(Ability->GetCurrentActorInfo()->AvatarActor);

				if (MyCharacter)
				{
					FVector SocketLocation = MyCharacter->GetMesh()->GetSocketLocation(nameStartingSocketName);

					SpawnTransform.SetLocation(SocketLocation + (ViewDirUnit * fForwardOffsetFromSocket));
				}

				SpawnTransform.SetRotation(ViewRot.Quaternion());

				bTransformSet = true;
			}
	}
	else if (bUseFixedStartingLocationRotation)
	{
		SpawnTransform.SetLocation(StartingLocation);

		SpawnTransform.SetRotation(StartingRotation.Quaternion());

		bTransformSet = true;
	}

	if (!bTransformSet)
	{
		if (FGameplayAbilityTargetData* LocationData = CachedTargetDataHandle.Get(0))		//Hardcode to use data 0. It's OK if data isn't useful/valid.
		{
			//Set location. Rotation is unaffected.
			if (LocationData->HasHitResult())
			{
				SpawnTransform.SetLocation(LocationData->GetHitResult()->Location);
				SpawnTransform.SetRotation(FQuat(LocationData->GetHitResult()->Normal.X, LocationData->GetHitResult()->Normal.Y, LocationData->GetHitResult()->Normal.Z, 0.0f));
				bTransformSet = true;
			}
			else if (LocationData->HasEndPoint())
			{
				SpawnTransform = LocationData->GetEndPointTransform();
				bTransformSet = true;
			}
		}
		if (!bTransformSet)
		{
			SpawnTransform = AbilitySystemComponent->GetOwner()->GetTransform();
		}
	}
}

void UOWSAbilityTask_SpawnArrowV2::FinishSpawningActor(UGameplayAbility* OwningAbility, AOWSAdvancedArrow* SpawnedActor)
{
	if (SpawnedActor)
		{
			SpawnedActor->ArrowMovement->InitialSpeed = SpawnedActor->ProjectileFirstSpeed;
			SpawnedActor->ArrowMovement->ProjectileAcceleration = SpawnedActor->ProjectileAcceleration;
		    SpawnedActor->ArrowMovement->ProjectileStabilitySpeed = SpawnedActor->ProjectileStabilitySpeed;
		    SpawnedActor->ArrowMovement->ProjectileFallAcceleration = SpawnedActor->ProjectileFallAcceleration;
		    SpawnedActor->ArrowMovement->ProjectileFallStabilitySpeed = SpawnedActor->ProjectileFallStabilitySpeed;
			SpawnedActor->ArrowMovement->ProjectileGravity = SpawnedActor->ProjectileGravity;
			SpawnedActor->ArrowMovement->ProjectileNoGravityRange = SpawnedActor->ProjectileStraightLineDistance;
			
			SpawnedActor->FinishSpawning(CachedSpawnTransform);
			

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				Success.Broadcast(SpawnedActor);
			}
		}

		EndTask();
}
