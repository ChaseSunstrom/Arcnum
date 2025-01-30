#include "spark_application.hpp"

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
    spark::Query<Position, Velocity> query,
    spark::Event<Blah, Acceleration> event
)
{
    if (event.Holds<Blah>())
        spark::Logger::Logln("Got Blah event: %d", event.Get<Blah>().i);


    if (event.Holds<Acceleration>())
    {
        auto aevent = event.Get<Acceleration>();
        spark::Logger::Logln("Got Acceleration event: %d, %d, %d", aevent.x, aevent.y, aevent.z);
    }

    auto entity = coordinator.CreateEntity(
        Position{ 0, 0, 0 },
        Velocity{ 0.01, 0.01, 0.01 } // Example non-zero velocity
    );

    query.ForEach([](spark::u32 ent, Position& pos, Velocity& vel)
        {
            pos.x += vel.x;
            pos.y += vel.y;
            pos.z += vel.z;
        });
}

void EventMaker(spark::Ref<Arcnum> app)
{
    static spark::u32 i = 0;

    if (i % 2000 == 0)
        app.SubmitEvent(Blah{ i });
    else if (i % 3000 == 0)
        app.SubmitEvent(Acceleration{ i, i * 2, i * 3});

    i++;
}

// System function to see entities
// Modified See function to debug
void See(spark::Ref<spark::Coordinator> coordinator, spark::Query<Position, Velocity> query)
{
    spark::Logger::Logln(spark::LogLevel::INFO, "See Query size: %d", query.Size());

    // Log all entities in the coordinator
    for (const auto& arch_pair : coordinator.GetArchetypes()) {
        const auto& signature = arch_pair.first;
        spark::Logger::Logln(spark::LogLevel::INFO, "Archetype Signature: %s", signature.to_string().c_str());

        for (const auto& chunk : arch_pair.second->GetChunks()) {
            spark::Logger::Logln(spark::LogLevel::INFO, "Chunk has %zu entities", chunk.Size());
        }
    }

    query.ForEach([](spark::u32 ent, Position& pos, Velocity& vel) {
        spark::Log(spark::LogLevel::INFO) << "Entity: " << ent
            << " Position: " << pos.x << ", " << pos.y << ", " << pos.z
            << " Velocity: " << vel.x << ", " << vel.y << ", " << vel.z
            << std::endl;
        });
}
int main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register systems with correct parameter passing
    app.RegisterSystem(EventMaker);
    app.RegisterSystem(Move);

    app.RegisterSystem(See, spark::LifecyclePhase::ON_SHUTDOWN);

    app.Start();
    app.Run();
    app.Close();

    return 0;
}
