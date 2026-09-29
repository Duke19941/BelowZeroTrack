class BZ_Settings
{
	bool enabled = true;
	float dropSeconds = 3.2;
	float lifetimeSeconds = 900.0;
	float minDistance = 1.6;
	float windMultiplier = 2.8;
	float overcastThreshold = 0.55;
	float stormMultiplier = 2.2;
	float blizzardMul = 1.0;
	int maxMarks = 80;
	void ResetToDefaults()
	{
		enabled = true;
		dropSeconds = BZ_Constants.DEFAULT_DROP_SEC;
		lifetimeSeconds = BZ_Constants.DEFAULT_LIFE_SEC;
		minDistance = BZ_Constants.DEFAULT_MIN_DIST;
		windMultiplier = BZ_Constants.DEFAULT_WIND_MUL;
		overcastThreshold = BZ_Constants.DEFAULT_OVERCAST_T;
		stormMultiplier = BZ_Constants.DEFAULT_STORM_MUL;
		blizzardMul = BZ_Constants.DEFAULT_BLIZZARD_MUL;
		maxMarks = BZ_Constants.DEFAULT_MAX_MARKS;
	}
};
class BZ_Config
{
	protected static ref BZ_Settings s_Settings;
	static BZ_Settings Get() { if (!s_Settings) s_Settings = new BZ_Settings(); return s_Settings; }
	static void Load()
	{
		s_Settings = new BZ_Settings();
		if (!FileExist(BZ_Constants.PROFILE_DIR)) MakeDirectory(BZ_Constants.PROFILE_DIR);
		if (FileExist(BZ_Constants.SETTINGS_PATH)) JsonFileLoader<BZ_Settings>.JsonLoadFile(BZ_Constants.SETTINGS_PATH, s_Settings);
		else { s_Settings.ResetToDefaults(); JsonFileLoader<BZ_Settings>.JsonSaveFile(BZ_Constants.SETTINGS_PATH, s_Settings); }
		Print(BZ_Constants.LOG_PREFIX + "Loaded for " + BZ_Constants.SERVER_NAME + ". Rain will not touch this.");
	}
};
