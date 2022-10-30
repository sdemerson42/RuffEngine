void Dialog_Main(ScriptComponent@ api)
{
	api.SetString("dialog0", "Greetings oh Wizard!\nTravel south to the\ncave and bring back\nthe treasure!");
	api.SetString("dialog1", "You have found the\ngem! What a magical\nand beardy wizard\nyou are!\n\nReturn to thy hovel\nto complete thy\nwizardly quest!");
}

void Dialog_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Wizard"))
	{
		auto input = api.GetInput();
		if (input.button == 1)
		{
			auto state = gLogic.GetInt("gem");
			if (state == 0)
			{
				Logic_DoDialog(api.GetString("dialog0"));
			}
			else
			{
				Logic_DoDialog(api.GetString("dialog1"));
			}
		}
	}
}