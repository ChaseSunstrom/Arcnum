#include <core/pch.hpp>
#include <core/util/types.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/event/event.hpp>
#include <core/app/application.hpp>
#include <core/ecs/ecs.hpp>
#include <core/system/serializer.hpp>
#include <core/system/file_serializer.hpp>
#include <core/system/manager.hpp>
#include <core/ecs/components/transform_component.hpp>

i32 main()
{
	Spark::Ecs ecs;
	
	for (i32 i = 0; i < 10000000; i++)
	{
		Spark::Entity& e = ecs.MakeEntity();
		ecs.AddComponent(e, "transform", new Spark::TransformComponent());
		ecs.AddComponent(e, "transform2", new Spark::TransformComponent());
		ecs.AddComponent(e, "transform3", new Spark::TransformComponent());
	}

	std::cout << "Entity Count: " << ecs.GetEntityCount() << std::endl;
	std::cout << "Transform Components: " << ecs.GetComponentCount<Spark::TransformComponent>() << std::endl;
}