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

	UFUNCTION(BlueprintCallable, Category = "Trace")
	bool CrosshairTrace(FHitResult &OutHit, FVector &Direction, const ECollisionChannel CollisionChannel = ECC_Pawn,const float Distance = 2000.f, const bool bDebug = false);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileByCrosshair(TSubclassOf<AProjectile> ProjectileClass);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Projectile")
	void ServerSpawnProjectile(TSubclassOf<AProjectile> ProjectileClass, const FTransform& Transform, const FVector IntialDirection, AActor* TargetActor);
	
};
