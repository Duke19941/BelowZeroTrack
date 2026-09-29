class BZ_Manager
{
	protected static ref BZ_Manager s_Instance;
	protected ref array<BZ_Spoor> m_Marks;
	protected ref map<PlayerBase, float> m_Cooldown;
	protected ref map<PlayerBase, vector> m_LastPos;
	static BZ_Manager Get() { if (!s_Instance) s_Instance = new BZ_Manager(); return s_Instance; }
	void BZ_Manager() { m_Marks = new array<BZ_Spoor>; m_Cooldown = new map<PlayerBase, float>; m_LastPos = new map<PlayerBase, vector>; }
	void Start() { GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Tick, 1000, true); }
	protected void Tick()
	{
		if (!GetGame() || !GetGame().IsServer()) return;
		BZ_Settings settings = BZ_Config.Get();
		if (!settings || !settings.enabled) return;
		DropFromBleeders(settings);
		AgeMarks(WeatherMul(settings));
	}
	protected float WeatherMul(BZ_Settings settings)
	{
		float mul = 1.0 * settings.blizzardMul;
		Weather weather = GetGame().GetWeather();
		if (!weather) return mul;
		float wind = 0;
		if (weather.GetWindMagnitude()) wind = weather.GetWindMagnitude().GetActual();
		else wind = weather.GetWind().Length();
		float wind01 = wind / 20.0;
		if (wind01 < 0) wind01 = 0;
		if (wind01 > 1.5) wind01 = 1.5;
		mul = mul + (settings.windMultiplier * wind01);
		float overcast = 0;
		if (weather.GetOvercast()) overcast = weather.GetOvercast().GetActual();
		if (overcast >= settings.overcastThreshold) mul = mul + (settings.stormMultiplier * overcast);
		float snow = 0;
		if (weather.GetSnowfall()) snow = weather.GetSnowfall().GetActual();
		if (snow > 0.08) mul = mul + (settings.stormMultiplier * snow * 1.4);
		return mul;
	}
	protected void DropFromBleeders(BZ_Settings settings)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(i));
			if (!pb || !pb.IsAlive() || !pb.IsBleeding() || pb.IsSwimming() || pb.BZ_IsInTransport()) continue;
			float cd = 0;
			if (m_Cooldown.Contains(pb)) cd = m_Cooldown.Get(pb);
			cd = cd - 1.0;
			if (cd > 0) { m_Cooldown.Set(pb, cd); continue; }
			vector pos = pb.GetPosition();
			float need = settings.minDistance * pb.BZ_TrackSpacingMul();
			if (pb.BZ_IsSprinting()) need = settings.minDistance * 1.55;
			float yaw = pb.GetOrientation()[0];
			if (m_LastPos.Contains(pb))
			{
				vector last = m_LastPos.Get(pb);
				if (vector.Distance(pos, last) < need) continue;
				vector dir = pos - last;
				if (dir.Length() > 0.2) yaw = dir.VectorToAngles()[0];
			}
			int weight = 1;
			if (pb.GetHealth("", "Health") < 60.0) weight = 2;
			if (pb.GetHealth("", "Health") < 35.0) weight = 3;
			SpawnMark(pos, settings.lifetimeSeconds, weight, yaw);
			m_LastPos.Set(pb, pos);
			float drop = settings.dropSeconds;
			if (weight >= 3) drop = drop * 0.7;
			m_Cooldown.Set(pb, drop);
		}
	}
	protected void SpawnMark(vector pos, float life, int weight, float yaw)
	{
		if (pos == "0 0 0") return;
		pos = SnapToCrust(pos);
		BZ_Settings settings = BZ_Config.Get();
		if (settings && m_Marks.Count() >= settings.maxMarks)
		{
			BZ_Spoor oldest = m_Marks.Get(0);
			m_Marks.Remove(0);
			if (oldest) oldest.DeleteSafe();
		}
		Object obj = GetGame().CreateObjectEx(BZ_Constants.MARK_CLASS, pos, ECE_PLACE_ON_SURFACE | ECE_NOLIFETIME);
		BZ_Spoor mark = BZ_Spoor.Cast(obj);
		if (!mark) return;
		mark.Arm(life, weight, yaw);
		m_Marks.Insert(mark);
	}
	protected vector SnapToCrust(vector pos)
	{
		string surf = "";
		float y = GetGame().SurfaceGetType(pos[0], pos[2], surf);
		if (y != 0) pos[1] = y;
		return pos;
	}
	protected void AgeMarks(float weatherMul)
	{
		for (int i = m_Marks.Count() - 1; i >= 0; i--)
		{
			BZ_Spoor mark = m_Marks.Get(i);
			if (!mark) { m_Marks.Remove(i); continue; }
			mark.ServerAge(1.0, weatherMul);
			if (mark.GetStage() <= 0) m_Marks.Remove(i);
		}
	}
};
