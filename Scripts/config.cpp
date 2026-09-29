class CfgPatches
{
	class BelowZeroTrack_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
	};
};

class CfgMods
{
	class BelowZeroTrack
	{
		dir = "BelowZeroTrack";
		name = "BELOWZERO Crimson Rime";
		author = "Dead Air Studio / BELOWZERO";
		version = "0.1.0";
		extra = 0;
		type = "mod";
		dependencies[] = { "Game", "World", "Mission" };
		class defs
		{
			class gameScriptModule { value = ""; files[] = { "BelowZeroTrack/Scripts/3_Game" }; };
			class worldScriptModule { value = ""; files[] = { "BelowZeroTrack/Scripts/4_World" }; };
			class missionScriptModule { value = ""; files[] = { "BelowZeroTrack/Scripts/5_Mission" }; };
		};
	};
};
