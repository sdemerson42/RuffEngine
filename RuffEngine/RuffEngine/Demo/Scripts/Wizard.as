void Wizard_Main(ScriptComponent@ api)
{
	api.Log("I am a mighty wizard! Tremble in fear as I move around!");
	api.PlayAnimation("walk", 4.5f, true, true);
	const float pxPerSecond = 40.0f;
	int cycleCounter = 0;
	const int cycleLimit = 30000;
	float dir = 1.0f;	

	while(true)
	{
		auto position = api.GetPosition();
		float x = position.x + dir * pxPerSecond * api.DeltaTime();
		float y = position.y - dir * pxPerSecond * api.DeltaTime();
		api.SetPosition(x, y);
		
		if (++cycleCounter == cycleLimit)
		{
			cycleCounter = 0;
			dir *= -1.0f;
		}

		api.Suspend();
	}
}