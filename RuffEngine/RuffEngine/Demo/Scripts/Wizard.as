void Wizard_Main(ScriptComponent@ api)
{
	const float speed = 200.0f;

	while(true)
	{
		auto i = api.GetInput();
		api.SetVelocity(i.leftStick.x * speed, i.leftStick.y * speed);

		if (i.leftStick.x != 0.0f || i.leftStick.y != 0.0f) api.PlayAnimation("walk", 6.0f, false, true);
		else api.StopAnimation();

		// Camera

		auto pos = api.GetPosition();
		auto viewPos = api.GetViewCenter();

		float deltaX = pos.x - viewPos.x;
		float deltaY = pos.y - viewPos.y;

		if (deltaX * deltaX + deltaY * deltaY > 10.0f)
		{
			float t = 0.95f * api.DeltaTime();
			float ix = pos.x * t + viewPos.x * (1.0f - t);
			float iy = pos.y * t + viewPos.y * (1.0f - t);

			api.SetViewCenter(ix, iy);
		}
		api.Suspend();
	}
}