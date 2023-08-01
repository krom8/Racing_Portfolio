// Fill out your copyright notice in the Description page of Project Settings.


#include "General/RiderMovementComponent.h"

void URiderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }
    
    CurPawnRotVector = PawnOwner->Controller->GetControlRotation().Vector();
    FVector InputVector = ConsumeInputVector();

    if (!InputVector.IsNearlyZero())
    {

        if ((InputVector - CurPawnRotVector).IsNearlyZero()) // Forward
        {
            if (CurSpeed < 0)
            {
                CurSpeed = CurSpeed + AccelSpeed + IncreaseSpeed;
            }
            else if (CurSpeed < (MaxSpeed - AccelSpeed))
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
        }
        else if ((InputVector + CurPawnRotVector).IsNearlyZero()) // Backward
        {
            if (CurSpeed <= 0) // Backward Move
            {
                if (CurSpeed < -(BackwardMaxSpeed - BackwardSpeed))
                {
                    CurSpeed = -BackwardMaxSpeed;
                }
                else
                {
                    CurSpeed -= BackwardSpeed;
                }

            }
            else if (CurSpeed <= BreakSpeed) // Stop
            {
                CurSpeed = 0;
            }

            else // Break
            {
                CurSpeed -= BreakSpeed;
            }

            
            CurVector = -InputVector * CurSpeed * DeltaTime;

            FHitResult Hit;
            SafeMoveUpdatedComponent(CurVector, UpdatedComponent->GetComponentRotation(), true, Hit);

            if (Hit.IsValidBlockingHit())
            {
                SlideAlongSurface(CurVector, 1.0f - Hit.Time, Hit.Normal, Hit);
            }
        }
        else //Turn
        {
            int a = 0;
        }

    }

    else
    {
        if (CurSpeed <= -IncreaseSpeed)
        {
            CurSpeed += IncreaseSpeed;
        }
        else if (CurSpeed <= IncreaseSpeed || CurSpeed < 0)
        {
            CurSpeed = 0;
        }
        else
        {
            CurSpeed -= DecreaseSpeed;
        }

        CurVector = CurPawnRotVector * CurSpeed * DeltaTime;
        FHitResult Hit;
        SafeMoveUpdatedComponent(CurVector, UpdatedComponent->GetComponentRotation(), true, Hit);

        if (Hit.IsValidBlockingHit())
        {
            SlideAlongSurface(CurVector, 1.0f - Hit.Time, Hit.Normal, Hit);
        }
    }


}








