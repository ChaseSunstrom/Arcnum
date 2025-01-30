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

// System function to initialize entities
// System function to initialize entities
void InitEntities(Arcnum& app, spark::Coordinator& coordinator)
{
    for (int i = 0; i < 100; i++)
    {
        // Initialize Velocity with non-zero values
        auto entity = coordinator.CreateEntity(
            Position{ 0, 0, 0 },
            Velocity{ 0.0001, 0.0001, 0.0001 } // Example non-zero velocity
        );
        spark::Logger::Logln(spark::LogLevel::DEBUG, "Created entity with Position and Velocity: %d", entity.GetId());
    }
}


// System function to move entities
void Move(
    Arcnum& app,
    spark::Coordinator& coordinator,
    spark::Query<Position, Velocity>& query,
    spark::Query<Position>& pquery,
    spark::Query<Position, spark::Without<Velocity>>& vquery,
    spark::Query<Velocity>& vquery2
)
{
    query.ForEach([](spark::u32 ent, Position& pos, Velocity& vel)
        {
            pos.x += vel.x;
            pos.y += vel.y;
            pos.z += vel.z;
        });
}

// System function to see entities
void See(Arcnum& app, spark::Query<Position, Velocity>& query)
{
    spark::Logger::Logln(spark::LogLevel::INFO, "See Query size: %d", query.Size());

    query.ForEach([](spark::u32 ent, Position& pos, Velocity& vel)
        {
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
    app.RegisterSystem(InitEntities, spark::LifecyclePhase::ON_START);
    app.RegisterSystem(Move, spark::LifecyclePhase::UPDATE);
    app.RegisterSystem(See, spark::LifecyclePhase::ON_SHUTDOWN);

    app.Start();
    app.Run();
    app.Close();

    return 0;
}
