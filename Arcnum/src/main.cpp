#include "spark_application.hpp"
#include "spark_threading.hpp"

struct Position
{
    spark::f64 x, y, z;
};

struct Velocity
{
    spark::f64 x, y, z;
};

struct Acceleration
{
    spark::u64 x, y, z;
};

struct Blah
{
    spark::u32 i;
};

using Arcnum = spark::Application;


// System function to move entities
void Move(
    spark::Ref<spark::Coordinator> coordinator,
    spark::Event<Blah, Acceleration> event
)
{
    if (event.Holds<Blah>())
        spark::Logger::Logln("Got Blah event: %u", event.Get<Blah>().i);


    if (event.Holds<Acceleration>())
    {
        auto aevent = event.Get<Acceleration>();
        spark::Logger::Logln("Got Acceleration event: %llu, %llu, %llu", aevent.x, aevent.y, aevent.z);
    }

    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }
}

void Move2(
    spark::Ref<spark::Coordinator> coordinator
)
{
    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }
}

void Move3(
    spark::Ref<spark::Coordinator> coordinator
)
{
    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }
}

void Move4(
    spark::Ref<spark::Coordinator> coordinator
)
{
    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }
}

void Move5(
    spark::Ref<spark::Coordinator> coordinator
)
{
    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }

}

void Move6(
    spark::Ref<spark::Coordinator> coordinator
)
{
    for (spark::i32 i = 0; i < 1000; i++)
    {
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
        );
    }
}

void EventMaker(spark::Ref<spark::Application> app)
{
    static std::atomic<spark::u32> i{ 0 };

    spark::u32 current = i.fetch_add(1, std::memory_order_relaxed);
    // Now we safely have a unique 'current' for this call

    if (current % 2000 == 0)
        app.SubmitEvent(Blah{ current });
    else if (current % 3000 == 0)
        app.SubmitEvent(Acceleration{ current, current * 2, current * 3 });
}


// System function to see entities
// Modified See function to debug
void See(spark::Ref<spark::Coordinator> coordinator, spark::Query<Position, Velocity> query)
{
    spark::Logger::Logln(spark::LogLevel::INFO, "See Query size: %d", query.Size());
}


spark::i32 main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register systems with correct parameter passing
    app.RegisterSystems(Move, Move2, Move3, EventMaker, spark::SystemSettings{.execution_mode = spark::SystemExecutionMode::MULTITHREADED_ASYNC});
    app.RegisterSystems(Move4, Move5, Move6, EventMaker, spark::SystemSettings{ .execution_mode = spark::SystemExecutionMode::SINGLE_THREADED });

    app.RegisterSystem(See, spark::SystemSettings{ spark::SystemPhase::ON_SHUTDOWN });

    app.Start();
    app.Run();

    return 0;
}
