// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Core/Projectile.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsSavingTargets = false;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Check if there are targets in the range of the crosshair every tick to add to the array.
	if (bIsSavingTargets)
		ProcessSavingTargets();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ABaseCharacter::CrosshairTrace(FHitResult& OutHit, FVector &Direction, const ECollisionChannel CollisionChannel, const float Distance, const bool bDebug)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		// Get viewport size
		int32 ViewportSizeX;
		int32 ViewportSizeY;
		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
		// Get the position in the middle of the screen (Cross-hair position in the 3D scene)
		FVector CrosshairLocation_Start;
		PlayerController->DeprojectScreenPositionToWorld(ViewportSizeX / 2.f, ViewportSizeY / 2.f, CrosshairLocation_Start, Direction);
		// Calculate the end position where the cross-hair is pointing at by a given distance
		FVector CrosshairLocation_End = ((GetBaseAimRotation().Vector() * Distance) + CrosshairLocation_Start);
		if (bDebug)
			DrawDebugLine(GetWorld(), CrosshairLocation_Start, CrosshairLocation_End, FColor::Red, false, 1, 0, 1);
		if (UWorld* World = GetWorld()) {
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			return World->LineTraceSingleByObjectType(OutHit, CrosshairLocation_Start, CrosshairLocation_End, CollisionChannel, CollisionParams);
		}
	}
	return false;
}

void ABaseCharacter::StartSavingTargets()
{
	SavedTargets.Empty();
	bIsSavingTargets = true;
}

void ABaseCharacter::ProcessSavingTargets()
{
	FHitResult OutHit;
	FVector Direction;
	if (CrosshairTrace(OutHit, Direction, ECC_Pawn, TargettingRange, false))
		if (!SavedTargets.Contains(OutHit.GetActor()))
			AddActorToSavedTargets(OutHit.GetActor());
}

void ABaseCharacter::EndSavingTargets()
{
	bIsSavingTargets = false;
	OnEndSavingTargets_Native(SavedTargets);
}

void ABaseCharacter::OnEndSavingTargets_Native(const TArray<AActor*>& TargetActors)
{
	OnEndSavingTargets(TargetActors);
}

void ABaseCharacter::AddActorToSavedTargets(AActor* TargetActor)
{
	SavedTargets.AddUnique(TargetActor);
	OnAddActorToSavedTargets_Native(TargetActor);
}

void ABaseCharacter::OnAddActorToSavedTargets_Native(AActor* TargetActor)
{
	OnAddActorToSavedTargets(TargetActor);
}

FVector ABaseCharacter::GetProjectileSpawnLocation()
{
	return GetMesh()->GetSocketLocation(ProjectileSpawnSocket);
}

void ABaseCharacter::SpawnProjectileByCrosshair(TSubclassOf<AProjectile> ProjectileClass)
{
	// Process the trace
	FHitResult OutHit;
	FVector CrosshairDirection;
	CrosshairTrace(OutHit, CrosshairDirection, ECC_Pawn, TargettingRange);
	// Set up the location and direction for the projectile spawn
	FVector SpawnLocation = GetMesh()->GetSocketLocation(ProjectileSpawnSocket);
	FVector InitialDirection = UKismetMathLibrary::GetDirectionUnitVector(SpawnLocation, OutHit.TraceEnd);
	ServerSpawnProjectile(ProjectileClass, FTransform(SpawnLocation), InitialDirection, OutHit.GetActor());
}

void ABaseCharacter::ServerSpawnProjectile_Implementation(TSubclassOf<AProjectile> ProjectileClass, const FTransform& Transform, const FVector IntialDirection, AActor* TargetActor)
{
	if (UWorld* World = GetWorld()) {
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = this;
		AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, Transform, ActorSpawnParams);
		if (Projectile)
			Projectile->InitProjectile(IntialDirection, TargetActor);
	}
}

void ABaseCharacter::SpawnMultipleProjectiles(TSubclassOf<AProjectile> ProjectileClass, const FTransform& Transform, const FVector IntialDirection, TArray<AActor*>TargetActors)
{
	for (AActor* TargetActor : TargetActors)
	{
		ServerSpawnProjectile(ProjectileClass, Transform, IntialDirection, TargetActor);
	}
}

void ABaseCharacter::SpawnMultipleProjectilesOnSavedActors(TSubclassOf<AProjectile> ProjectileClass)
{
	FHitResult OutHit;
	FVector CrosshairDirection;
	CrosshairTrace(OutHit, CrosshairDirection, ECC_Pawn, TargettingRange);
	FVector InitialDirection = UKismetMathLibrary::GetDirectionUnitVector(GetProjectileSpawnLocation(), OutHit.TraceEnd);
	SpawnMultipleProjectiles(ProjectileClass, FTransform(GetProjectileSpawnLocation()), InitialDirection, SavedTargets);
}