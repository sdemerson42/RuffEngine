ScriptComponent@ gLogic;

void Logic_Main(ScriptComponent@ api)
{
	@gLogic = api;
	float spawnTimer = 0.0f;
	api.SetInt("batCount", 0);
	api.SetInt("score", 0);

	string s = api.ReadFile("Sim/Data/ArenaMap.txt");
	auto div = s.findFirst("*");
	string t1 = s.substr(0, div);
	string t2 = s.substr(div + 1, s.length() - (div + 1));

	api.AddDynamicTileMap(1, 1, 32, t1, "", "", "default", "");
	api.AddDynamicTileMap(2, 1, 32, t2, "", "", "default", "ground");
	api.Log(api.GetString("init"));
	api.ChangeScene(1);

	while(true)
	{
		spawnTimer += api.DeltaTime();
		int batCount = api.GetInt("batCount");
		if (spawnTimer >= 2.0f && batCount < 10)
		{
			spawnTimer = 0.0f;
			int count = api.AddInt("batCount", 1);

			int spawnLoc = api.RandomRangeInt(1, 4);
			float spawnX = 100.0f;
			float spawnY = 100.0f;

			if (spawnLoc == 1)
			{
				spawnX = 900.0f;
			}
			else if (spawnLoc == 2)
			{
				spawnY = 900.0f;
			}
			else if (spawnLoc == 3)
			{
				spawnX = 900.0f;
				spawnY = 900.0f;
			}

			auto bat = api.SpawnEntity("Bat", spawnX, spawnY);
		}

		api.SetTextString("Score: " + api.GetInt("score"));

		api.Suspend();
	}
}
