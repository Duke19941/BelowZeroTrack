modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		BZ_Config.Load();
		BZ_Manager.Get().Start();
		Print(BZ_Constants.LOG_PREFIX + "BELOWZERO crust is listening. Rain will not edit this.");
	}
};
modded class MissionGameplay
{
	override void OnInit()
	{
		super.OnInit();
		Print(BZ_Constants.LOG_PREFIX + "BELOWZERO — you can read the crust.");
	}
};
