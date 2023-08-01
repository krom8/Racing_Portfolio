// Fill out your copyright notice in the Description page of Project Settings.


#include "General/RiderMovementComponent.h"

void URiderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }

    FRotator CurRotator = PawnOwner->Controller->GetControlRotation();
    FVector InputVector = ConsumeInputVector();

    if (!InputVector.IsNearlyZero())
    {
        CurInput = InputVector;
        if (CurSpeed < MaxSpeed - AccelSpeed)
        {
            CurSpeed += AccelSpeed;
        }
        CurVector = InputVector * CurSpeed * DeltaTime;

        FHitResult Hit;
        SafeMoveUpdatedComponent(CurVector, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(CurVector, 1.0f - Hit.Time, Hit.Normal, Hit);
        }
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("----------------")));
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), InputVector.X));
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), InputVector.Y));
        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), InputVector.Z));
    }

    else
    {
        if (CurSpeed <= 0)
        {
            return;
        }
        else if (CurSpeed <= DecreaseSpeed)
        {
            CurSpeed = 0;
        }
        else
        {
            CurSpeed -= DecreaseSpeed;
            CurVector = CurInput * CurSpeed * DeltaTime;
            FHitResult Hit;
            SafeMoveUpdatedComponent(CurVector, UpdatedComponent->GetComponentRotation(), true, Hit);

            if (Hit.IsValidBlockingHit())
            {
                SlideAlongSurface(CurVector, 1.0f - Hit.Time, Hit.Normal, Hit);
            }

        }
    }


}








