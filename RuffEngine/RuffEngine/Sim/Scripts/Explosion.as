void Explosion_Main(ScriptComponent@ api)
{
	float lifeCounter = 0.0f;

	while(true)
	{
		lifeCounter += api.DeltaTime();
		if (lifeCounter >= 0.1f)
		{
			api.Despawn();
			break;
		}

		api.Suspend();
	}
}