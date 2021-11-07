void Fireball_Main(ScriptComponent@ api)
{
	float lifeCounter = 0.0f;

	while(true)
	{
		lifeCounter += api.DeltaTime();
		if (lifeCounter >= 3.0f)
		{
			api.Despawn();
			break;
		}

		api.Suspend();
	}
}

void Fireball_OnCollision(ScriptComponent@ api, Entity@ collider)
{
	if (collider.HasTag("Wizard") || collider.HasTag("Fireball"))
	{
		return;
	}

	api.Despawn();
}