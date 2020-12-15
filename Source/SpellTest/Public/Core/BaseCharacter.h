// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class AProjectile;

UCLASS()
class SPELLTEST_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	ABaseCharacter();

	/** Max Target Distance represents how far can a pawn be in order to be targeted. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	float TargettingRange = 2000.f;
	/** Projectile Spawn Socket represents the name of the socket from where the projectile will be spawned (in the character mesh) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	FName ProjectileSpawnSocket = "Head";

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> Targets;

protected:
	
	UPROPERTY()
	AActor* LastSpawnedActorRef;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/** Traces from the cross-hair (middle of the screen to a certain range passed into the arguments). Does not replicate.
	 * @param OutHit - Is the output HitResult when the trace hits.
	 * @param Direction - Is the direction of the screen.
	 * @param CollisionChannel - Is the collision channel to be able to hit in the trace.
	 * @param Distance - Is the Distance of the trace to accept (Range).
	 * @param bDebug - Whether display the trace or not (for debug purpose).
	 * @return Whether it has hit or not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Trace")
	bool CrosshairTrace(FHitResult &OutHit, FVector &Direction, const ECollisionChannel CollisionChannel = ECC_Pawn, const float Distance = 2000.f, const bool bDebug = false);
	/** Spawns a projectile after making a trace on a target found in the trace if the projectile is homing or just with a constant velocity in the screen direction. The tracing doesn't replicate but the spawning replicates.
	 * @param ProjectileClass - Is the projectile class to spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileByCrosshair(TSubclassOf<AProjectile> ProjectileClass);
	/** Spawns a projectile on the server and initialize it with the given arguments.
	 * @param ProjectileClass - Is the projectile class to spawn.
	 * @param Transform - Is the transform of the projectile to be spawned with.
	 * @param IntialDirection - Is the initial direction to set to the projectile.
	 * @param TargetActor - Is the actor to home towards in case it's a homing projectile.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Projectile")
	void ServerSpawnProjectile(TSubclassOf<AProjectile> ProjectileClass, const FTransform& Transform, const FVector IntialDirection, AActor* TargetActor);
	
};
