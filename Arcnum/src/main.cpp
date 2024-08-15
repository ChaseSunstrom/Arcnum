#include <core/pch.hpp>
#include <core/util/types.hpp>
#include <core/window/gl/gl_window.hpp>
#include <core/event/event.hpp>
#include <core/app/application.hpp>
#include <core/ecs/ecs.hpp>
#include <core/system/serializer.hpp>
#include <core/system/file_serializer.hpp>
#include <core/system/manager.hpp>

struct TestSer
{
	i32 x;
	i32 y;
	std::vector<i32> xs;

	static void Serialize(Spark::ISerializer* s, const TestSer& x)
	{
		s->WriteRaw(x.x);
		s->WriteRaw(x.y);
		s->WriteVector(x.xs);
	}

	static void Deserialize(Spark::IDeserializer* s, TestSer& x)
	{
		s->ReadRaw(x.x);
		s->ReadRaw(x.y);
		s->ReadVector(x.xs);
	}
};

i32 main()
{
	{
		TestSer x{ 1, 2, {} };

		for (i32 i = 0; i < 10; i++)
		{
			x.xs.push_back(i);
		}

		Spark::FileSerializer fs("output.bin");
		fs.WriteObj(x);
	}

	{
		TestSer x;
		Spark::FileDeserializer fd("output.bin");
		fd.ReadObj(x);

		for (auto& i : x.xs)
		{
			std::cout << i << std::endl;
		}
	}
}