// Fill out your copyright notice in the Description page of Project Settings.

#include "ChooseNextWaypoint.h"
#include "AIController.h"
#include "PatrolComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	// Get Patrol Points
	auto AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto PatrolComponent = AIPawn->FindComponentByClass<UPatrolComponent>();
	if (!PatrolComponent) return EBTNodeResult::Aborted;
	TArray<AActor*> PatrolPoints = PatrolComponent->GetPatrolPoints();

	if (PatrolPoints.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("No Patrol point set for %s"), *AIPawn->GetName());
		return EBTNodeResult::Aborted;
	}

	// Set Next Waypoint
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);
	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);
	// UE_LOG(LogTemp, Warning, TEXT("Waypoint Index: %s: %i"), *IndexKey.SelectedKeyName.ToString(), Index);

	// Cycle Index
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);
	return EBTNodeResult::Succeeded;
}


