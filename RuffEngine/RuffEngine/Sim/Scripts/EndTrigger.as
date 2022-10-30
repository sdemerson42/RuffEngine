void EndTrigger_Main(ScriptComponent@ api)
{
	
}

void EndTrigger_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Wizard") && gLogic.GetInt("gem") == 1)
	{
		api.ChangeScene(6);
	}
}