void Wizard_Main(ScriptComponent@ api)
{
	const float speed = 150.0f;
	const float shootRate = 0.25f;
	float shootCounter = shootRate;

	const float fbVelocityStraight = 500.0f;
	const float fbVelocityDiag = fbVelocityStraight * sqrt(2) / 2.0f; 

	while(true)
	{
		auto i = api.GetInput();
		api.SetVelocity(i.leftStick.x * speed, i.leftStick.y * speed);

		if (i.leftStick.x != 0.0f || i.leftStick.y != 0.0f) api.PlayAnimation("walk", 6.0f, false, true);
		else api.StopAnimation();

		// SpawnTest

		if (shootCounter < shootRate) shootCounter += api.DeltaTime();
		if (shootCounter >= shootRate && (i.rightStick.x != 0.0f || i.rightStick.y != 0.0f))
		{
			auto p = api.GetPosition();
			auto fireballEntity = api.SpawnEntity("Fireball", p.x, p.y);
			auto fs = api.GetScriptFromEntity(fireballEntity);
			
			api.PlaySound("Fireball.wav", 1, 50, 1.0f, 0.0f, 0.0f, 0.0f, false);
			
			if (i.rightStick.x < 0.0f)
			{
				if (i.rightStick.y < 0.0f)
				{
					fs.SetRotation(-45);
					fs.SetVelocity(-1.0f * fbVelocityDiag, -1.0f * fbVelocityDiag);
				}
				if (i.rightStick.y == 0.0f)
				{
					fs.SetRotation(-90);
					fs.SetVelocity(-1.0f * fbVelocityStraight, 0);
				}
				if (i.rightStick.y > 0.0f)
				{
					fs.SetRotation(-135);
					fs.SetVelocity(-1.0f * fbVelocityDiag, fbVelocityDiag);
				}

			}

			if (i.rightStick.x == 0.0f)
			{
				if (i.rightStick.y < 0.0f)
				{
					fs.SetRotation(0);
					fs.SetVelocity(0, -1.0f * fbVelocityStraight);
				}
				
				if (i.rightStick.y > 0.0f)
				{
					fs.SetRotation(180);
					fs.SetVelocity(0, fbVelocityStraight);
				}
			}

			if (i.rightStick.x > 0.0f)
			{
				if (i.rightStick.y < 0.0f)
				{
					fs.SetRotation(45);
					fs.SetVelocity(fbVelocityDiag, -1.0f * fbVelocityDiag);
				}
				if (i.rightStick.y == 0.0f)
				{
					fs.SetRotation(90);
					fs.SetVelocity(fbVelocityStraight, 0);
				}
				if (i.rightStick.y > 0.0f)
				{
					fs.SetRotation(135);
					fs.SetVelocity(fbVelocityDiag, fbVelocityDiag);
				}
			}

			shootCounter = 0.0f;
		}

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

void Wizard_OnCollision(ScriptComponent@ api, Entity@ collider, float xDir, float yDir)
{
	if (collider.HasTag("Bat"))
	{
		api.StopMusic();
		api.PlaySound("Boom.wav", 1, 50, 1.0f, 0.0f, 0.0f, 0.0f, false);
		api.ChangeScene(5);
	}
}