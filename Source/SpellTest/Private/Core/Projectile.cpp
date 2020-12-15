// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SpellTest/SpellTest.h"
#include "Core/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	// Adding a collision component for the projectile
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	if (CollisionComponent) {
		SetRootComponent(CollisionComponent);
		CollisionComponent->SetSphereRadius(16);
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnCollisionBeginOverlap_Native);
	}
	// Adding a Movement component for the projectile
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	if (ProjectileMovementComponent) {
		ProjectileMovementComponent->SetIsReplicated(true);
		ProjectileMovementComponent->InitialSpeed = 1000.f;
		ProjectileMovementComponent->MaxSpeed = 1200.f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingAccelerationMagnitude = ProjectileMovementComponent->MaxSpeed * 10;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
	}
	
	SetReplicatingMovement(true);

	bDestroyOnHit = true;
	bAffectOwner = false;
	bHasHit = false;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AProjectile::InitProjectile(const FVector InitialDirection, AActor* TargetActor)
{
	// Initialize the velocity
	ProjectileMovementComponent->Velocity = InitialDirection * ProjectileMovementComponent->InitialSpeed;
	// Check if homing and the target is available
	if(ProjectileMovementComponent->bIsHomingProjectile && TargetActor) {
		// look for a component in the center of the actor to travel towards it. The component should have "Center" as tag.
		TArray<UActorComponent*> TargetActorComponents = TargetActor->GetComponentsByTag(UActorComponent::StaticClass(), "Center");
		UActorComponent* TargetCenterActorComp;
		// Set the homing target component as the center component if found, else just pick the root component.
		if (TargetActorComponents.IsValidIndex(0)) {
			TargetCenterActorComp = TargetActorComponents[0];
			if (USceneComponent* TargetCenterSceneComp = Cast<USceneComponent>(TargetCenterActorComp))
				ProjectileMovementComponent->HomingTargetComponent = TargetCenterSceneComp;
		}
		else ProjectileMovementComponent->HomingTargetComponent = TargetActor->GetRootComponent();
	}
}

bool AProjectile::ApplyProjectileEffects_Implementation(AActor* HitActor)
{
	UE_LOG(LogTest, Log, TEXT("Projectile <%s> has hit <%s>"), *GetName(), *HitActor->GetName());
	return true;
}

void AProjectile::OnCollisionBeginOverlap_Native(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority()) {
		// Check if the owner can be affected or not
		if (bAffectOwner || OtherActor != GetOwner()) {
			// Execute spell effects here
			if (!bHasHit) {
				ApplyProjectileEffects(OtherActor);
				bHasHit = true;
			}
			if (bDestroyOnHit) {
				// Destroy(true); // Using Destroy will not wait for the projectile effects to function properly, for that we hide it and give it a second until everything has properly functioned for all the clients in the ApplyProjectileEffects
				SetActorHiddenInGame(true);
				SetLifeSpan(2.f); // 2 seconds can vary depending on how much of a net latency is at the client however a game with 2000ms is not really playable.
			}
		}
	}
	OnCollisionBeginOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AProjectile::ServerSpawnEmitterAtLocation_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroy /*= true*/, EPSCPoolMethod PoolingMethod /*= EPSCPoolMethod::None*/, bool bAutoActivateSystem /*= true*/)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, EmitterTemplate, Location, Rotation, Scale, bAutoDestroy, PoolingMethod, bAutoActivateSystem);
}

void AProjectile::MulticastSpawnEmitterAtLocation_Implementation(UParticleSystem* EmitterTemplate, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroy /*= true*/, EPSCPoolMethod PoolingMethod /*= EPSCPoolMethod::None*/, bool bAutoActivateSystem /*= true*/)
{
	UGameplayStatics::SpawnEmitterAtLocation(this, EmitterTemplate, Location, Rotation, Scale, bAutoDestroy, PoolingMethod, bAutoActivateSystem);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

