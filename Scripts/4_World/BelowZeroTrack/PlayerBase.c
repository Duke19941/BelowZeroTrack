modded class PlayerBase
{
	float BZ_TrackSpacingMul()
	{
		if (IsPlayerInStance(DayZPlayerConstants.STANCEMASK_CROUCH)) return 0.65;
		return 1.0;
	}
	bool BZ_IsSprinting()
	{
		vector vel = GetVelocity(this);
		return vel.Length() > 5.8;
	}
};
