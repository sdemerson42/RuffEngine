void Bat_Main(ScriptComponent@ api)
{
	float axisSpeed = 200.0f;
	api.SetFloat("axisSpeed", axisSpeed);
	
	int batDir = api.RandomRangeInt(1, 4);
	float dirX = axisSpeed;
	float dirY = axisSpeed;
	if (batDir == 1)
	{
		dirX *= -1.0f;
	}
	else if (batDir == 2)
	{
		dirY *= -1.0f;
	}
	else if  (batDir == 3)
	{
		dirX *= -1.0f;
		dirY *= -1.0f;
	}

	api.SetVelocity(dirX, dirY);
}

void Bat_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Fireball"))
	{
		auto p = api.GetPosition();
		api.PlaySound("Explosion.wav", 1, 50, 1.0f, 0.0f, 0.0f, 0.0f, false);
		auto explosionEntity = api.SpawnEntity("Explosion", p.x, p.y);
		gLogic.AddInt("batCount", -1);
		gLogic.AddInt("score", 1);
		api.Despawn();
	}

	auto vel = api.GetVelocity();
	float axisSpeed = api.GetFloat("axisSpeed");
	if (xDir < 0.0f) api.SetVelocity(axisSpeed, vel.y);
	if (xDir > 0.0f) api.SetVelocity(-1.0f * axisSpeed, vel.y);
	if (yDir < 0.0f) api.SetVelocity(vel.x, axisSpeed);
	if (yDir > 0.0f) api.SetVelocity(vel.x, -1.0f * axisSpeed);
}