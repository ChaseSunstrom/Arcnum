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
    spark::Query<Position, Velocity> query,
    spark::Query<Position, Velocity> query2,
    spark::Query<Position, Velocity> query3,
    spark::Query<Position, Velocity> query4,
    spark::Query<Position, Velocity> query5,
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

void Move2(
    spark::Ref<spark::Coordinator> coordinator,
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

void EventMaker(spark::Ref<spark::Application> app)
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

    query.ForEach([](spark::u32 ent, const Position& pos, const Velocity& vel) {
        spark::Logln(spark::LogLevel::INFO) << "Entity: " << ent
            << " Position: " << pos.x << ", " << pos.y << ", " << pos.z
            << " Velocity: " << vel.x << ", " << vel.y << ", " << vel.z << std::endl;
        });
}

void TestThreading(spark::threading::ThreadPool& pool)
{
    using namespace spark::threading;

    // Enqueue tasks with different priorities
    auto task1 = pool.Enqueue(TaskPriority::HIGH, []() -> int {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Executing High Priority Task\n";
        return 42;
        });

    auto task2 = pool.Enqueue(TaskPriority::LOW, []() -> std::string {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Executing Low Priority Task\n";
        return "Hello, World!";
        });

    auto task3 = pool.Enqueue(TaskPriority::CRITICAL, []() -> void {
        std::cout << "Executing Critical Task\n";
        });

    // Retrieve and print the results
    std::cout << "Result of Task1: " << task1.result.get() << "\n";
    std::cout << "Task1 executed by thread ID: " << task1.thread_id.get() << "\n";

    std::cout << "Result of Task2: " << task2.result.get() << "\n";
    std::cout << "Task2 executed by thread ID: " << task2.thread_id.get() << "\n";

    task3.result.get(); // Wait for completion
    std::cout << "Task3 executed by thread ID: " << task3.thread_id.get() << "\n";

    // Wait for all tasks to complete before shutting down
    pool.WaitForAllTasks();
}

spark::i32 main()
{
    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);

    // Register systems with correct parameter passing
    app.RegisterSystem(EventMaker, spark::SystemSettings{ .execution_mode = spark::SystemExecutionMode::MULTITHREADED_ASYNC, .priority = spark::SystemPriority::NORMAL });
    app.RegisterSystem(Move, spark::SystemSettings{ .execution_mode = spark::SystemExecutionMode::MULTITHREADED_ASYNC, .priority = spark::SystemPriority::NORMAL });
    app.RegisterSystem(Move2, spark::SystemSettings{ .execution_mode = spark::SystemExecutionMode::MULTITHREADED_ASYNC, .priority = spark::SystemPriority::NORMAL });
    //app.RegisterSystem(TestThreading);

    app.RegisterSystem(See, spark::SystemSettings{ spark::SystemPhase::ON_SHUTDOWN });

    app.Start();
    app.Run();
    app.Close();

    return 0;
}
