// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(NotBlueprintType)
class SPELLTEST_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	/** Shape component used for collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;
	/** Projectile movement component that controls the movement of the projectile (Speed, Homing or not homing)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	/** Set the projectile to be destroyed on collision event after applying the effects */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bDestroyOnHit : 1;
	/** Does the projectile affect its owner when applying effects? */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bAffectOwner : 1;
	/** Is true after colliding with something */
	UPROPERTY()
	uint8 bHasHit : 1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Initiate the projectile data by setting its target actor to home towards and Initial direction to head to
	 * @param InitialDirection The initial direction that the projectile will be launched into.
	 * @param TargetActor The target actor to home towards.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void InitProjectile(const FVector InitialDirection, AActor* TargetActor);
	/** Initiate the projectile data by setting its target actor to home towards and Initial direction to head to
	 * @param HitActor The actor to apply the affects to.
	 * @Return Whether the effects have been properly applied or not.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyProjectileEffects(AActor* HitActor);

	UFUNCTION()
	virtual void OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** Begin tracking an overlap interaction with the collision component. */
	UFUNCTION(BlueprintImplementableEvent)
	void OnCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** Plays the specified effect at the given location and rotation, fire and forget. The system will go away when the effect is complete. Replicates on all the clients (if the owner is server).
	 * @param WorldContextObject - Object that we can obtain a world context from
	 * @param EmitterTemplate - particle system to create
	 * @param Location - location to place the effect in world space
	 * @param Rotation - rotation to place the effect in world space
	 * @param Scale - scale to create the effect at
	 * @param bAutoDestroy - Whether the component will automatically be destroyed when the particle system completes playing or whether it can be reactivated
	 * @param PoolingMethod - Method used for pooling this component. Defaults to none.
	 * @param bAutoActivate - Whether the component will be automatically activated on creation.
	 */
	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MulticastSpawnEmitterAtLocation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroy = true, EPSCPoolMethod PoolingMethod = EPSCPoolMethod::None, bool bAutoActivateSystem = true);
};
