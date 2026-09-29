class CfgPatches
{
	class BelowZeroTrack_Data
	{
		units[] = { "BZ_Spoor" };
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = { "DZ_Data", "DZ_Sounds_Effects" };
	};
};

class CfgVehicles
{
	class Inventory_Base;
	class BZ_Spoor : Inventory_Base
	{
		scope = 2;
		displayName = "BELOWZERO blood";
		descriptionShort = "Crimson on the crust. BELOWZERO does not wash this. The wind does.";
		model = "\dz\gear\consumables\stone.p3d";
		weight = 1;
		itemSize[] = { 1, 1 };
		forceFarBubble = 1;
		canBeDigged = 0;
	};
};

class CfgSoundShaders
{
	class BZ_SpoorHot_SoundShader
	{
		samples[] = { { "DZ\sounds\environment\waters\brook\brook_l", 1 } };
		volume = 0.28;
		range = 14;
		rangeCurve = "LinearCurve";
		limitation = 0;
	};
};

class CfgSoundSets
{
	class BZ_SpoorHot_SoundSet
	{
		soundShaders[] = { "BZ_SpoorHot_SoundShader" };
		spatial = 1;
		doppler = 0;
		loop = 0;
		volumeCurve = "characterAttenuationCurve";
		distanceFilter = "defaultDistanceFilter";
	};
};
