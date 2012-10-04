
#include "stdincludes.hpp"

#include "Box2D/Box2D.h"

void box2dtest()
{
	b2Vec2 gravity(0.0f, -10.0f);
	b2World world(gravity);

	/*
	 * Create a ground body:
	 */

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -10.0f);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(50.0f, 10.0f);

	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	groundBody->CreateFixture(&groundBox, 0.0f);

	/*
	 * Create a dynamic body:
	 */

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 4.0f);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	b2Body* body = world.CreateBody(&bodyDef);
	body->CreateFixture(&fixtureDef);

	/*
	 * Simulation
	 */

	float32 timeStep = 1.0f / 60.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	for (int32 i = 0; i < 60; ++i)
	{
	    world.Step(timeStep, velocityIterations, positionIterations);
	    b2Vec2 position = body->GetPosition();
	    float32 angle = body->GetAngle();
	    printf("%4.2f %4.2f %4.2f\n", position.x, position.y, angle);
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1280, 1024, 32), "Adventure");

	box2dtest();

	while(window.IsOpened())
	{
		for(sf::Event event; window.GetEvent(event); )
		{
			// Close window : exit
			if (event.Type == sf::Event::Closed)
				window.Close();

			// Escape key : exit
			if ((event.Type == sf::Event::KeyPressed) && (event.Key.Code == sf::Key::Escape))
				window.Close();
		}

		window.Clear();
		window.Display();
	}

	return EXIT_SUCCESS;
}
