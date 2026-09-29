modded class PlayerBase
{
	float BZ_TrackSpacingMul()
	{
		if (IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
			return 0.65;
		return 1.0;
	}

	bool BZ_IsSprinting()
	{
		HumanMovementState state = new HumanMovementState();
		GetMovementState(state);
		if (state && state.m_iMovement == DayZPlayerConstants.MOVEMENTIDX_SPRINT)
			return true;
		vector vel = GetVelocity(this);
		return vel.Length() > 5.8;
	}

	bool BZ_IsInTransport()
	{
		return GetCommand_Vehicle() != null;
	}
};
