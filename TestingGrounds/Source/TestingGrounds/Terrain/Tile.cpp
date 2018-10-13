// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Actors/ActorPool.h"
#include "Engine/World.h"


const int32 MAX_TRIES = 32;

// Sets default values
ATile::ATile() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	NavigationBoundsOffset = FVector(2000, 0, 0);

	MinSpawnBound = FVector(0, -2000, 0);
	MaxSpawnBound = FVector(4000, 2000, 0);
}

// Called when the game starts or when spawned
void ATile::BeginPlay() {
	Super::BeginPlay();

}

void ATile::SetPool(UActorPool * PoolToSet) {
	Pool = PoolToSet;

	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume() {
	NavMeshBoundsVolume = Pool->Checkout();
	if (!NavMeshBoundsVolume) { UE_LOG(LogTemp, Warning, TEXT("[%s] Not enough Actors in Pool"), *GetName()); return; }
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + NavigationBoundsOffset);
	//GetWorld()->GetNavigationSystem()->Build();
	UNavigationSystemV1::GetNavigationSystem(GetWorld())->Build();
	UE_LOG(LogTemp, Warning, TEXT("[%s] Got Volume %s at %s"), *GetName(), *NavMeshBoundsVolume->GetName(), *(GetActorLocation()+NavigationBoundsOffset).ToCompactString());
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
	FBox Bounds(MinSpawnBound, MaxSpawnBound);
	for (int32 i = 0; i < MAX_TRIES; i++) {
		FVector Point = FMath::RandPointInBox(Bounds) + FVector(0, 0, ZOffset);
		if (CanSpawnAtLocation(Point, Radius)) {
			OutLocation = Point;
			return true;
		}
	}
	return false;
}

template<class T>
void ATile::PlaceActor(TSubclassOf<T> ToSpawn, FSpawnPosition Position) {
	AActor * Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(Position.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorRotation(FRotator(0, Position.Rotation, 0));
	Spawned->SetActorScale3D(FVector(Position.Scale));
}

template<>
void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition Position) {
	APawn* Pawn = GetWorld()->SpawnActor<APawn>(ToSpawn);
	Pawn->SetActorRelativeLocation(Position.Location);
	Pawn->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Pawn->SpawnDefaultController();
	Pawn->Tags.Add(FName("Enemy"));
}

template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, FSpawnParameters Parameters) {
	int32 NumToSpawn = FMath::RandRange(Parameters.MinSpawn, Parameters.MaxSpawn);
	for (int i = 0; i < NumToSpawn; i++) {
		FSpawnPosition Position;
		Position.Rotation = Parameters.Rotate * FMath::RandRange(-180.f, 180.f);
		Position.Scale = FMath::RandRange(Parameters.MinScale, Parameters.MaxScale);
		if (FindEmptyLocation(OUT Position.Location, Parameters.Radius * Parameters.MaxScale, Parameters.ZOffset * Position.Scale)) {
			Position.Location = Position.Location + FVector(0, 0, Parameters.ZOffset) * Position.Scale;
			PlaceActor(ToSpawn, Position);
		}
		// UE_LOG(LogTemp, Warning, TEXT("Random point: %s"), *Spawnpoint.ToCompactString());
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnParameters Parameters) {
	if (!ToSpawn) return;
	RandomlyPlaceActors(ToSpawn, Parameters);
}

void ATile::PlaceAIPawn(TSubclassOf<APawn> ToSpawn, FSpawnParameters Parameters) {
	if (!ToSpawn) return;
	Parameters.MinScale = 1;
	Parameters.MaxScale = 1;
	RandomlyPlaceActors(ToSpawn, Parameters);
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	Pool->Return(NavMeshBoundsVolume);
}

