class BZ_Spoor extends Inventory_Base
{
	protected int m_Stage;
	protected int m_Weight;
	protected int m_Kicked;
	protected float m_Life;
	protected float m_MaxLife;
	protected float m_Yaw;
	protected Particle m_Blood;
	protected Particle m_Steam;
	protected bool m_HotSoundPlayed;

	void BZ_Spoor()
	{
		RegisterNetSyncVariableInt("m_Stage");
		RegisterNetSyncVariableInt("m_Weight");
		RegisterNetSyncVariableInt("m_Kicked");
		RegisterNetSyncVariableFloat("m_Yaw");
		SetEventMask(EntityEvent.INIT);
	}

	override void EEInit()
	{
		super.EEInit();
		SetTakeable(false);
		SetAffectPathgraph(false, false);
		SetScale(0.12);
	}

	override bool CanPutInCargo(EntityAI parent) { return false; }
	override bool CanPutIntoHands(EntityAI parent) { return false; }
	override bool IsInventoryVisible() { return false; }

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionReadBelowZeroSpoor);
		AddAction(ActionKickBelowZeroSpoor);
	}

	void Arm(float lifetime, int weight, float yaw)
	{
		m_MaxLife = lifetime;
		m_Life = lifetime;
		m_Weight = weight;
		m_Yaw = yaw;
		m_Stage = BZ_Constants.STAGE_HOT;
		SetOrientation(Vector(yaw, 0, 0));
		SetSynchDirty();
	}

	void ServerAge(float dt, float weatherMul)
	{
		if (!GetGame().IsServer()) return;
		m_Life = m_Life - (dt * weatherMul);
		int stage = BZ_Constants.STAGE_HOT;
		float ratio = 1.0;
		if (m_MaxLife > 0) ratio = m_Life / m_MaxLife;
		if (ratio > 0.62) stage = BZ_Constants.STAGE_HOT;
		else if (ratio > 0.28) stage = BZ_Constants.STAGE_GLAZE;
		else if (ratio > 0) stage = BZ_Constants.STAGE_RIME;
		else stage = BZ_Constants.STAGE_GONE;
		if (stage != m_Stage) { m_Stage = stage; SetSynchDirty(); }
		if (m_Life <= 0) GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, 250, false);
	}

	int GetStage() { return m_Stage; }
	bool WasKicked() { return m_Kicked == 1; }

	void KickOver(PlayerBase player)
	{
		if (!GetGame().IsServer()) return;
		if (m_Stage <= 0) return;
		m_Kicked = 1;
		m_Stage = BZ_Constants.STAGE_RIME;
		m_Life = m_MaxLife * 0.22;
		SetSynchDirty();
		if (player) player.MessageImportant("BELOWZERO — you kicked snow over it. A tracker can still see the scuff.");
	}

	string ReadTheCrust()
	{
		if (m_Kicked == 1) return "BELOWZERO — someone kicked snow over this. The scuff is new. They were scared, or smart.";
		string weight = "a thin mark";
		if (m_Weight >= 3) weight = "a heavy pour";
		else if (m_Weight == 2) weight = "a steady drip";
		if (m_Stage == BZ_Constants.STAGE_HOT) return "BELOWZERO — still steaming. " + weight + ". They have not made the next tree line.";
		if (m_Stage == BZ_Constants.STAGE_GLAZE) return "BELOWZERO — glass on the crust. " + weight + ". Minutes. The wind has not found it.";
		if (m_Stage == BZ_Constants.STAGE_RIME) return "BELOWZERO — frost ate the red. " + weight + ". Look twice or you will walk past them.";
		return "BELOWZERO — the drift has the rest.";
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		SetOrientation(Vector(m_Yaw, 0, 0));
		ApplyFx();
	}

	override void EEDelete(EntityAI parent) { StopFx(); super.EEDelete(parent); }

	protected void ApplyFx()
	{
		if (GetGame().IsDedicatedServer()) return;
		StopFx();
		if (m_Stage <= 0) return;
		if (m_Kicked == 0) m_Blood = Particle.PlayOnObject(ParticleList.BLEEDING_SOURCE, this, "0 0.04 0");
		if (m_Stage == BZ_Constants.STAGE_HOT && m_Kicked == 0)
		{
			m_Steam = Particle.PlayOnObject(ParticleList.CAMP_SMALL_SMOKE, this, "0 0.18 0");
			if (!m_HotSoundPlayed)
			{
				m_HotSoundPlayed = true;
				SEffectManager.PlaySoundOnObject(BZ_Constants.SOUND_HOT, this);
			}
		}
	}

	protected void StopFx()
	{
		if (m_Blood) { m_Blood.Stop(); m_Blood = null; }
		if (m_Steam) { m_Steam.Stop(); m_Steam = null; }
	}
};

class ActionReadBelowZeroSpoorCB : ActionContinuousBaseCB { override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(1.1); } };
class ActionReadBelowZeroSpoor : ActionContinuousBase
{
	void ActionReadBelowZeroSpoor()
	{
		m_CallbackClass = ActionReadBelowZeroSpoorCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Read the crust (BELOWZERO)";
	}
	override void CreateConditionComponents() { m_ConditionItem = new CCINone; m_ConditionTarget = new CCTCursor; }
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item) { return player && target && BZ_Spoor.Cast(target.GetObject()) != null; }
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target || !action_data.m_Player) return;
		BZ_Spoor mark = BZ_Spoor.Cast(action_data.m_Target.GetObject());
		if (mark) action_data.m_Player.MessageImportant(mark.ReadTheCrust());
	}
};

class ActionKickBelowZeroSpoorCB : ActionContinuousBaseCB { override void CreateActionComponent() { m_ActionData.m_ActionComponent = new CAContinuousTime(1.8); } };
class ActionKickBelowZeroSpoor : ActionContinuousBase
{
	void ActionKickBelowZeroSpoor()
	{
		m_CallbackClass = ActionKickBelowZeroSpoorCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "Kick snow over it (BELOWZERO)";
	}
	override void CreateConditionComponents() { m_ConditionItem = new CCINone; m_ConditionTarget = new CCTCursor; }
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!player || !target) return false;
		BZ_Spoor mark = BZ_Spoor.Cast(target.GetObject());
		if (!mark || mark.WasKicked() || mark.GetStage() <= 0) return false;
		return true;
	}
	override void OnFinishProgressServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target) return;
		BZ_Spoor mark = BZ_Spoor.Cast(action_data.m_Target.GetObject());
		if (mark) mark.KickOver(action_data.m_Player);
	}
};

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionReadBelowZeroSpoor);
		actions.Insert(ActionKickBelowZeroSpoor);
	}
};
