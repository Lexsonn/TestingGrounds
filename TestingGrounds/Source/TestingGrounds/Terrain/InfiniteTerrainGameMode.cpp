// Fill out your copyright notice in the Description page of Project Settings.

#include "InfiniteTerrainGameMode.h"
#include "NavMeshBoundsVolume.h"
#include "EngineUtils.h"


void AInfiniteTerrainGameMode::PopulateBoundsVolumePool() {

	auto NavIterator = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (NavIterator) {
		AddToPool(*NavIterator);
		++NavIterator;
	}
}

void AInfiniteTerrainGameMode::AddToPool(ANavMeshBoundsVolume * ToAdd) {
	UE_LOG(LogTemp, Warning, TEXT("%s"), *ToAdd->GetName());
}
