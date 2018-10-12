// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"
#include "NavMeshBoundsVolume.h"
#include "Actors/ActorPool.h"
#include "EngineUtils.h"


AInfiniteTerrainGameMode::AInfiniteTerrainGameMode() {
	NavMeshBoundsVolumePool = CreateDefaultSubobject<UActorPool>(FName("Nav Mesh Bounds Volume Pool"));
}

void AInfiniteTerrainGameMode::PopulateBoundsVolumePool() {

	auto NavIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (NavIterator) {
		AddToPool(*NavIterator);
		++NavIterator;
	}
}

void AInfiniteTerrainGameMode::AddToPool(ANavMeshBoundsVolume * ToAdd) {
	NavMeshBoundsVolumePool->Add(ToAdd);
}
