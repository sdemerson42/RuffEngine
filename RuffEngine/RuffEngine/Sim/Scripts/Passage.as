void Passage_Main(ScriptComponent@ api)
{
}

void Passage_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Wizard"))
	{
		Logic_DoTravel(api.GetString("init"));
	}
}