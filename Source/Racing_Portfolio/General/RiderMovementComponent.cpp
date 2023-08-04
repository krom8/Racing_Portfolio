// Fill out your copyright notice in the Description page of Project Settings.


#include "General/RiderMovementComponent.h"
#include "Rider.h"

void URiderMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
    {
        return;
    }
    Con = PawnOwner->GetController();
    CurPawnRotVector = PawnOwner->Controller->GetControlRotation().Vector();
    ARider* RiderOwner = static_cast<ARider*>(PawnOwner);
    MoveDeltaTime = DeltaTime;

    // Getting from Pawn
    BaseTurnRate = RiderOwner->BaseTurnRate;
    MaxSpeed = RiderOwner->BaseTurnRate;
    AccelSpeed = RiderOwner->AccelSpeed;
    DecreaseSpeed = RiderOwner->DecreaseSpeed;
    IncreaseSpeed = RiderOwner->IncreaseSpeed;
    TurnDecreaseSpeed = RiderOwner->TurnDecreaseSpeed;
    BackwardSpeed = RiderOwner->BackwardSpeed;
    BreakSpeed = RiderOwner->BreakSpeed;
    BackwardMaxSpeed = RiderOwner->BackwardMaxSpeed;

    if (!RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && !RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 0000
    {
        DecreaseMove();
    }

    else if (RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && !RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 1000
    {
        ForwardMove();
    }

    else if (!RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && !RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 0100
    {
        BackwardMove();
    }

    else if (RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && !RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 1100
    {

        DecreaseMove();
    }

    else if (!RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 0010
    {
        DecreaseTurnMove(false);
    }

    else if (RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 1010
    {
        ForwardTurnMove(false);
    }

    else if (!RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 0110
    {
        BackwardTurnMove(false);
    }

    else if (RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && RiderOwner->RightPressed && !RiderOwner->LeftPressed) // 1110
    {
        DecreaseTurnMove(false);
    }

    else if (!RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && !RiderOwner->RightPressed && RiderOwner->LeftPressed) // 0001
    {
        DecreaseTurnMove();
    }

    else if (RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && !RiderOwner->RightPressed && RiderOwner->LeftPressed) // 1001
    {
        ForwardTurnMove();
    }

    else if (!RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && !RiderOwner->RightPressed && RiderOwner->LeftPressed) // 0101
    {
        BackwardTurnMove();
    }

    else if (RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && !RiderOwner->RightPressed && RiderOwner->LeftPressed) // 1101
    {
        DecreaseTurnMove();
    }

    else if (!RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && RiderOwner->RightPressed && RiderOwner->LeftPressed) // 0011
    {
        DecreaseMove();
    }

    else if (RiderOwner->ForwardPressed && !RiderOwner->BackwardPressed && RiderOwner->RightPressed && RiderOwner->LeftPressed) // 1011
    {
        ForwardMove();
    }

    else if (!RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && RiderOwner->RightPressed && RiderOwner->LeftPressed) // 0111
    {
        DecreaseMove();
    }

    else if (RiderOwner->ForwardPressed && RiderOwner->BackwardPressed && RiderOwner->RightPressed && RiderOwner->LeftPressed) // 1111
    {
        DecreaseMove();
    }
}


void URiderMovementComponent::Print(FVector A)
{
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("--------------")));
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(" %f"), A.X));
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(" %f"), A.Y));
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(" %f"), A.Z));
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT(" %f"), A.SizeSquared()));

}


void URiderMovementComponent::DecreaseMove()
{
    InputVector = CurPawnRotVector;
    if (CurSpeed < -IncreaseSpeed)
    {
        CurSpeed += IncreaseSpeed;
    }
    else if (CurSpeed < DecreaseSpeed)
    {
        CurSpeed = MaxSpeed;
    }
    else
    {
        CurSpeed -= DecreaseSpeed;
    }

    CurVector = CurPawnRotVector * CurSpeed * MoveDeltaTime;
    Move();
}

void URiderMovementComponent::ForwardMove()
{
    InputVector = CurPawnRotVector;
    if (CurSpeed < 0)
    {
        CurSpeed = CurSpeed + AccelSpeed + IncreaseSpeed;
    }
    else if (CurSpeed < (MaxSpeed - AccelSpeed))
    {
        CurSpeed += AccelSpeed;
    }
    else
    {
        CurSpeed = MaxSpeed;
    }
    CurVector = InputVector * CurSpeed * MoveDeltaTime;
    Move();
}

void URiderMovementComponent::BackwardMove()
{
    InputVector = CurPawnRotVector;
    if (CurSpeed < -(BackwardMaxSpeed - BackwardSpeed))
    {
       CurSpeed = -BackwardMaxSpeed;
    }

    else if (CurSpeed <= 0)
    {
       CurSpeed -= BackwardSpeed;
    }

    else if (CurSpeed <= (BreakSpeed+DecreaseSpeed)) // Stop
    {
        CurSpeed = 0;
    }

    else // Break
    {
        CurSpeed -= (BreakSpeed + DecreaseSpeed);
    }

    CurVector = -InputVector * CurSpeed * MoveDeltaTime;
    Move();

}


void URiderMovementComponent::DecreaseTurnMove(bool Right)
{

	if (CurSpeed < -(TurnDecreaseSpeed + IncreaseSpeed))
	{
		CurSpeed += (TurnDecreaseSpeed + IncreaseSpeed);
        Rotate(Right, true);
	}

    else if (CurSpeed < 0)
    {
        CurSpeed = 0;
        Rotate(Right, true);
    }

    else if (CurSpeed < (TurnDecreaseSpeed + DecreaseSpeed))
    {
        CurSpeed = 0;
        Rotate();
    }

	else
	{
		CurSpeed -= (TurnDecreaseSpeed + DecreaseSpeed);
        Rotate();
	}


	Move();
}

void URiderMovementComponent::ForwardTurnMove(bool Right)
{
    if (CurSpeed < 0)
    {
        CurSpeed = CurSpeed + AccelSpeed;
        Rotate(Right, true);
    }
    else if (CurSpeed < (MaxSpeed - AccelSpeed))
    { 
        CurSpeed += AccelSpeed;
        Rotate();
    }
  
    else
    {
        CurSpeed = MaxSpeed;
        Rotate();
    }


    Move();
}


void URiderMovementComponent::BackwardTurnMove(bool Right)
{

    if (CurSpeed < -(BackwardMaxSpeed - BackwardSpeed))
    {
        CurSpeed = -BackwardMaxSpeed;
        Rotate(Right, true);
    }
    else if (CurSpeed < 0)
    {
        CurSpeed -= BackwardSpeed;
        Rotate(Right, true);
    }
    else if (CurSpeed < (BreakSpeed + DecreaseSpeed))
    { 
        CurSpeed = 0;
        Rotate();
    }
    else
    {
        CurSpeed -= (BreakSpeed + DecreaseSpeed);
        Rotate();
    }

    Move();
}


void URiderMovementComponent::Rotate(bool Right, bool Backward)
{

    FRotator ActorRot = PawnOwner->GetActorRotation();
    double YawInput = BaseTurnRate * MoveDeltaTime * PawnOwner->CustomTimeDilation;
    if ((Backward == true && Right == true)|| (Backward == false && Right == false))
    {
        YawInput = -YawInput;
    }

    ActorRot = ActorRot.Add(0, YawInput, 0);
    InputVector = FRotationMatrix(ActorRot).GetScaledAxis(EAxis::X);
    Con->SetControlRotation(ActorRot);
    PawnOwner->SetActorRotation(ActorRot);
    if (Backward == false)
    {
        CurVector = InputVector * CurSpeed * MoveDeltaTime;
    }
    else
    {
        CurVector = -InputVector * CurSpeed * MoveDeltaTime;
    }
}


void URiderMovementComponent::Move()
{
    FHitResult Hit;
    SafeMoveUpdatedComponent(CurVector, UpdatedComponent->GetComponentRotation(), true, Hit);

    if (Hit.IsValidBlockingHit())
    {
        SlideAlongSurface(CurVector, 1.0f - Hit.Time, Hit.Normal, Hit);
    }
}