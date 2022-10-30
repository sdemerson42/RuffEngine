void Gem_Main(ScriptComponent@ api)
{
	
}

void Gem_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Wizard"))
	{
		gLogic.PlaySound("Gem.wav", 1, 30, 1.0f, 0.0f, 0.0f, 0.0f, false);
		gLogic.SetInt("gem", 1);
		api.Despawn();
		api.SceneDespawn();
	}
}