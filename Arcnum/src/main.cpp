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

using Arcnum = spark::Application;


// System function to move entities
void Move(
    Arcnum& app,
    spark::Coordinator& coordinator,
    spark::Query<Position, Velocity> query
)
{
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

// System function to see entities
// Modified See function to debug
void See(spark::Coordinator& coordinator, spark::Query<Position, Velocity> query, spark::Query<Velocity> vquery, spark::Query<Position> pquery)
{

    auto man_query = coordinator.CreateQuery<Position, Velocity>();



    spark::Logger::Logln(spark::LogLevel::INFO, "See Query size: %d", query.Size());
spark::Logger::Logln(spark::LogLevel::INFO, "See MANUAL QUERY size: %d", man_query.Size());


    // Log all entities in the coordinator
    for (const auto& arch_pair : coordinator.GetArchetypes()) {
        const auto& signature = arch_pair.first;
        spark::Logger::Logln(spark::LogLevel::INFO, "Archetype Signature: %s", signature.to_string().c_str());

        for (const auto& chunk : arch_pair.second->GetChunks()) {
            spark::Logger::Logln(spark::LogLevel::INFO, "Chunk has %zu entities", chunk.Size());
        }
    }

    query.ForEach([](spark::u32 ent, Position& pos, Velocity& vel) {
        std::cout << "Entity: " << ent
            << " Position: " << pos.x << ", " << pos.y << ", " << pos.z
            << " Velocity: " << vel.x << ", " << vel.y << ", " << vel.z
            << std::endl;
        });
}
int main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register systems with correct parameter passing
    app.RegisterSystem(Move, spark::LifecyclePhase::UPDATE);
    app.RegisterSystem(See, spark::LifecyclePhase::ON_SHUTDOWN);

    app.Start();
    app.Run();
    app.Close();

    return 0;
}
