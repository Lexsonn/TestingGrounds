// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"


const int32 MAX_TRIES = 32;

// Sets default values
ATile::ATile() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATile::BeginPlay() {
	Super::BeginPlay();
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius) {
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool Hit = GetWorld()->SweepSingleByChannel(
		OUT HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);
	FColor Color = (Hit) ? FColor::Red : FColor::Green;
	DrawDebugCapsule(GetWorld(), GlobalLocation, 0, Radius, FQuat::Identity, Color, true, 100.f);
	return !Hit;
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius, int32 ZOffset) {
	FBox Bounds(FVector(0, -2000, 0), FVector(4000, 2000, 0));
	for (int32 i = 0; i < MAX_TRIES; i++) {
		FVector Point = FMath::RandPointInBox(Bounds) + FVector(0, 0, ZOffset);
		if (CanSpawnAtLocation(Point, Radius)) {
			OutLocation = Point;
			return true;
		}
	}
	return false;
}

void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale) {
	AActor * Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(SpawnPoint);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, Rotation, 0));
	Spawned->SetActorScale3D(FVector(Scale));
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int32 MinSpawn, int32 MaxSpawn, bool Rotate, float MinScale, float MaxScale, float Radius, int32 ZOffset) {
	if (!ToSpawn) return;
	int32 NumToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (int i = 0; i < NumToSpawn; i++) {
		float Scale = FMath::RandRange(MinScale, MaxScale);
		FVector SpawnPoint;
		if (FindEmptyLocation(OUT SpawnPoint, Radius * MaxScale, ZOffset * Scale)) 
			PlaceActor(ToSpawn, SpawnPoint + FVector(0, 0, ZOffset * Scale), Rotate * FMath::RandRange(-180.f, 180.f), Scale);
		// UE_LOG(LogTemp, Warning, TEXT("Random point: %s"), *Spawnpoint.ToCompactString());
		
	}
}

