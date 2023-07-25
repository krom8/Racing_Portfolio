// Fill out your copyright notice in the Description page of Project Settings.


#include "General/RiderMovementComponent.h"

void URiderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(100.0f) * DeltaTime * 150.f;


    //if (!DesiredMovementThisFrame.IsNearlyZero())
    if (!DesiredMovementThisFrame.IsNearlyZero())
    {
        double PlusSpeed = DesiredMovementThisFrame.SizeSquared();
        FHitResult Hit;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
        }
    }

}








