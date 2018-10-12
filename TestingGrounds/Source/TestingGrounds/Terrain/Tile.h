// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"


class UActorPool;

USTRUCT(Blueprintable)
struct FSpawnParameters {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite) int32 MinSpawn; 
	UPROPERTY(BlueprintReadWrite) int32 MaxSpawn;
	UPROPERTY(BlueprintReadWrite) bool Rotate = true;
	UPROPERTY(BlueprintReadWrite) float MinScale = 1;
	UPROPERTY(BlueprintReadWrite) float MaxScale = 1;
	UPROPERTY(BlueprintReadWrite) float Radius = 500.f;
	UPROPERTY(BlueprintReadWrite) int32 ZOffset = 0;
};

USTRUCT()
struct FSpawnPosition {
	GENERATED_USTRUCT_BODY()

	FVector Location;
	float Rotation; 
	float Scale;
};

UCLASS()
class TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category = Setup)
	void PlaceActors(TSubclassOf<AActor> ToSpawn, FSpawnParameters Parameters);

	UFUNCTION(BlueprintCallable, Category = Setup)
	void SetPool(UActorPool* PoolToSet);

	
protected:
	UPROPERTY(EditDefaultsOnly, Category = Setup) FVector NavigationBoundsOffset;
	UPROPERTY(EditDefaultsOnly, Category = Setup) FVector MinSpawnBound;
	UPROPERTY(EditDefaultsOnly, Category = Setup) FVector MaxSpawnBound;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY() UActorPool* Pool;
	UPROPERTY() AActor* NavMeshBoundsVolume;
	
	void PositionNavMeshBoundsVolume();
	bool CanSpawnAtLocation(FVector Location, float Radius);
	bool FindEmptyLocation(FVector& OutLocation, float Radius, int32 ZOffset);
	TArray<FSpawnPosition> GenerateRandomSpawnPositions(FSpawnParameters Parameters);
	void PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition Position);
};
