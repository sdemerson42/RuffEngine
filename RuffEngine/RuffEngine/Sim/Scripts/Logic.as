ScriptComponent@ gLogic;

const int MODE_GAME = 0;
const int MODE_PASSAGE = 1;
const int MODE_DIALOG = 2;

void Logic_Main(ScriptComponent@ api)
{
	@gLogic = api;

	// Title
	while(true)
	{
		auto input = api.GetInput();
		if (input.button == 1)
		{
			api.ChangeScene(1);
			api.Suspend();
			api.PlayMusic("ForestTheme.wav", 50.0f, true);
			break;
		}
		api.Suspend();
	}
	
	api.SpawnEntity("Wizard", 300.0f, 300.0f);

	while(true)
	{
		api.Suspend();
	}
}

void Logic_OnPause(ScriptComponent@ api)
{
	while(true)
	{
		int mode = api.GetInt("mode");
		if (mode == MODE_PASSAGE)
		{
			Logic_Passage(api);
		}

		if (mode == MODE_DIALOG)
		{
			Logic_Dialog(api);
		}

		api.PauseSuspend();
	}
}

void Logic_DoTravel(const string& in data)
{
	auto scene = parseInt(data.substr(0, 1));
	auto x = parseInt(data.substr(1, 4));
	auto y = parseInt(data.substr(5, 4));

	gLogic.SetInt("newScene", scene);
	gLogic.SetInt("wizardX", x);
	gLogic.SetInt("wizardY", y);

	gLogic.SetInt("mode", MODE_PASSAGE);
	gLogic.SetInt("backdropCounter", 0);
	gLogic.SetPause(true);
}

void Logic_DoDialog(const string& in msg)
{
	gLogic.SetFloat("dialogPop", 0.0f);
	gLogic.SetString("dialogMsg", msg);

	gLogic.SetInt("mode", MODE_DIALOG);

	gDialogBox.SetComponentActive("render", true);
	gDialogBox.SetScale(0.0f, 0.0f);
	gLogic.SetPause(true);

	gLogic.PlaySound("DialogPop.wav", 1, 30, 1.0f, 0.0f, 0.0f, 0.0f, false);
}

void Logic_Passage(ScriptComponent@ api)
{
	int counter = api.GetInt("backdropCounter");
	
	if (counter < 255)
	{
		api.SetRenderColor(255, 255, 255, counter);
	}
	else if (counter == 255)
	{
		int scene = api.GetInt("newScene");
		api.ChangeScene(scene);
	}
	else if (counter == 256)
	{
		api.SetViewCenter(api.GetInt("wizardX"), api.GetInt("wizardY"));
		if (api.GetInt("newScene") == 3)
		{
			api.PlayMusic("CaveTheme.wav", 50.0f, true);
		}
		else
		{
			api.PlayMusic("ForestTheme.wav", 50.0f, true);
		}
	}
	else if (counter < 510)
	{
		api.SetRenderColor(255, 255, 255, 510 - counter);
	}
	else
	{
		api.SetRenderColor(255, 255, 255, 0);
		api.SetInt("mode", MODE_GAME);
		api.SpawnEntity("Wizard", api.GetInt("wizardX"), api.GetInt("wizardY"));
		api.SetPause(false);
	}

	api.AddInt("backdropCounter", 1);
}

void Logic_Dialog(ScriptComponent@ api)
{
	auto pop = api.GetFloat("dialogPop");
	if (pop != 1.0f)
	{
		pop += api.DeltaTime() * 8.0f;
		if (pop >= 1.0f)
		{
			pop = 1.0f;
		}
		api.SetFloat("dialogPop", pop);
		gDialogBox.SetScale(pop, pop);
		return;
	}

	gDialogBox.SetTextString(api.GetString("dialogMsg"));
	auto input = api.GetInput();
	if (input.button == 1)
	{
		gDialogBox.SetTextString("");
		gDialogBox.SetComponentActive("render", false);
		api.SetInt("mode", MODE_GAME);
		api.SetPause(false);
	}
}

