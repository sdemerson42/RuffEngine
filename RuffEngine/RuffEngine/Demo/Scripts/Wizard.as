void Wizard_Main(ScriptComponent@ api)
{
	const float speed = 200.0f;

	while(true)
	{
		auto i = api.GetInput();
		api.SetVelocity(i.leftStick.x * speed, i.leftStick.y * speed);

		if (i.leftStick.x != 0.0f || i.leftStick.y != 0.0f) api.PlayAnimation("walk", 6.0f, false, true);
		else api.StopAnimation();

		api.Suspend();
	}
}