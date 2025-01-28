// main.cpp

#include "spark_ecs.hpp"
#include "spark_stopwatch.hpp" // Assuming you have implemented spark_stopwatch.hpp
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

// Define some components
struct Position {
    float x, y, z;
    float _padding; // To match the Rust struct layout
};

struct Velocity {
    float vx, vy, vz;
    float _padding; // To match the Rust struct layout
};

struct Health {
    float hp;
    float _padding1, _padding2, _padding3; // To match the Rust struct layout
};

struct Transform {
    float matrix[16];
};

struct Render {
    int mesh_id;
    int material_id;
    float _padding1, _padding2; // To match the Rust struct layout
};

struct Physics {
    float mass;
    float friction;
    float restitution;
    float _padding; // To match the Rust struct layout
};

struct LargeComponent {
    float data[256]; // 1KB of data
};

// Struct to hold benchmark results
struct BenchResult {
    double time;        // Elapsed time in seconds
    size_t operations;  // Number of operations performed
    size_t memory;      // Memory used in bytes

    double ops_per_second() const {
        return operations / time;
    }

    double time_per_op_microseconds() const {
        return (time * 1'000'000.0) / operations;
    }

    std::string memory_str() const {
        if (memory < 1024)
            return std::to_string(memory) + "B";
        else if (memory < 1024 * 1024)
            return std::to_string(memory / 1024.0) + "KB";
        else if (memory < 1024 * 1024 * 1024)
            return std::to_string(memory / (1024.0 * 1024.0)) + "MB";
        else
            return std::to_string(memory / (1024.0 * 1024.0 * 1024.0)) + "GB";
    }
};

// Simple Logger class
class Logger {
public:
    static void Logln(const std::string& message) {
        std::cout << message << std::endl;
    }

    template <typename... Args>
    static void Logln(const char* format, Args... args) {
        printf(format, args...);
        printf("\n");
    }
};

// Function to get process memory usage (Windows implementation)
size_t get_process_memory() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    HANDLE process = GetCurrentProcess();
    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize);
    }
    return 0;
#else
    // Implement other platforms if needed
    return 0;
#endif
}

// Stopwatch class using std::chrono
class Stopwatch {
public:
    void Start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void Stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }

    double GetElapsedTime() const { // Returns time in seconds
        return std::chrono::duration<double>(end_time - start_time).count();
    }

private:
    std::chrono::high_resolution_clock::time_point start_time, end_time;
};

// Benchmark Functions

// Benchmark: Entity Creation Simple (2 components)
BenchResult benchmark_entity_creation_simple(spark::Coordinator& coordinator, size_t entity_count) {
    size_t start_mem = get_process_memory();
    Stopwatch sw;
    sw.Start();

    for (size_t i = 0; i < entity_count; ++i) {
        coordinator.CreateEntity(
            Position{ 0.0f, 0.0f, 0.0f, 0.0f },
            Velocity{ 0.0f, 0.0f, 0.0f, 0.0f }
        );
    }

    sw.Stop();
    size_t end_mem = get_process_memory();

    return BenchResult{
        sw.GetElapsedTime(),
        entity_count,
        end_mem > start_mem ? end_mem - start_mem : 0
    };
}

// Benchmark: Entity Creation Complex (6 components)
BenchResult benchmark_entity_creation_complex(spark::Coordinator& coordinator, size_t entity_count) {
    size_t start_mem = get_process_memory();
    Stopwatch sw;
    sw.Start();

    for (size_t i = 0; i < entity_count; ++i) {
        coordinator.CreateEntity(
            Position{ 0.0f, 0.0f, 0.0f, 0.0f },
            Velocity{ 0.0f, 0.0f, 0.0f, 0.0f },
            Health{ 100.0f, 0.0f, 0.0f, 0.0f },
            Transform{ {0.0f} },
            Render{ 0, 0, 0.0f, 0.0f },
            Physics{ 1.0f, 0.5f, 0.5f, 0.0f }
        );
    }

    sw.Stop();
    size_t end_mem = get_process_memory();

    return BenchResult{
        sw.GetElapsedTime(),
        entity_count,
        end_mem > start_mem ? end_mem - start_mem : 0
    };
}

// Benchmark: Entity Creation Large (1KB component)
BenchResult benchmark_entity_creation_large(spark::Coordinator& coordinator, size_t entity_count) {
    size_t start_mem = get_process_memory();
    Stopwatch sw;
    sw.Start();

    for (size_t i = 0; i < entity_count; ++i) {
        coordinator.CreateEntity(
            LargeComponent{ /* Initialize with zeros */ }
        );
    }

    sw.Stop();
    size_t end_mem = get_process_memory();

    return BenchResult{
        sw.GetElapsedTime(),
        entity_count,
        end_mem > start_mem ? end_mem - start_mem : 0
    };
}

// Benchmark: Entity Iteration Simple (2 components)
BenchResult benchmark_entity_iteration_simple(spark::Coordinator& coordinator, size_t entity_count) {
    // Setup: Create entities with Position and Velocity
    for (size_t i = 0; i < entity_count; ++i) {
        coordinator.CreateEntity(
            Position{ 0.0f, 0.0f, 0.0f, 0.0f },
            Velocity{ 0.0f, 0.0f, 0.0f, 0.0f }
        );
    }

    Stopwatch sw;
    size_t count = 0;
    sw.Start();

    auto query = coordinator.CreateQuery<Position, Velocity>();
    query.ForEach([&count](spark::u32 entity_id, Position& pos, Velocity& vel) {
        (void)entity_id; // Suppress unused variable warning
        (void)pos;
        (void)vel;
        count++;
        });

    sw.Stop();

    return BenchResult{
        sw.GetElapsedTime(),
        count,
        0 // Memory not measured for iteration
    };
}

// Benchmark: Entity Iteration Complex (4 components)
BenchResult benchmark_entity_iteration_complex(spark::Coordinator& coordinator, size_t entity_count) {
    // Setup: Create entities with Position, Velocity, Health, Transform
    for (size_t i = 0; i < entity_count; ++i) {
        coordinator.CreateEntity(
            Position{ 0.0f, 0.0f, 0.0f, 0.0f },
            Velocity{ 0.0f, 0.0f, 0.0f, 0.0f },
            Health{ 100.0f, 0.0f, 0.0f, 0.0f },
            Transform{ {0.0f} },
            // Optional: Add other components if needed
            // For consistency with Rust's 4 components, only 4 are added here
            // You can adjust as necessary
            Render{ 0, 0, 0.0f, 0.0f }
        );
    }

    Stopwatch sw;
    size_t count = 0;
    sw.Start();

    auto query = coordinator.CreateQuery<Position, Velocity, Health, Render>();
    query.ForEach([&count](spark::u32 entity_id, Position& pos, Velocity& vel, Health& hp, Render& rend) {
        (void)entity_id;
        (void)pos;
        (void)vel;
        (void)hp;
        (void)rend;
        count++;
        });

    sw.Stop();

    return BenchResult{
        sw.GetElapsedTime(),
        count,
        0 // Memory not measured for iteration
    };
}

// Benchmark: Random Component Access
BenchResult benchmark_component_access(spark::Coordinator& coordinator, size_t access_count) {
    // Setup: Create entities with Position, Velocity, Health
    std::vector<spark::Entity> entities;
    size_t test_entities = access_count / 10;
    entities.reserve(test_entities);

    for (size_t i = 0; i < test_entities; ++i) {
        auto entity = coordinator.CreateEntity(
            Position{ 0.0f, 0.0f, 0.0f, 0.0f },
            Velocity{ 0.0f, 0.0f, 0.0f, 0.0f },
            Health{ 100.0f, 0.0f, 0.0f, 0.0f }
        );
        entities.push_back(entity);
    }

    // Prepare random number generator
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, entities.size() - 1);

    Stopwatch sw;
    sw.Start();

    for (size_t i = 0; i < access_count; ++i) {
        size_t idx = dist(rng);
        spark::Entity entity = entities[idx];
        auto pos = coordinator.GetComponent<Position>(entity);
        auto vel = coordinator.GetComponent<Velocity>(entity);
        auto hp = coordinator.GetComponent<Health>(entity);
        (void)pos;
        (void)vel;
        (void)hp;
    }

    sw.Stop();

    return BenchResult{
        sw.GetElapsedTime(),
        access_count,
        0 // Memory not measured for component access
    };
}

// Benchmark: Mixed Operations
BenchResult run_mixed_operations_benchmark(spark::Coordinator& coordinator) {
    size_t start_mem = get_process_memory();
    Stopwatch sw;
    sw.Start();

    std::vector<spark::Entity> entities;
    const size_t OPERATIONS = 1'000'000;

    for (size_t i = 0; i < OPERATIONS; ++i) {
        switch (i % 5) {
        case 0:
        case 1:
        case 2: { // 60% Queries
            auto query = coordinator.CreateQuery<Position, Velocity>();
            query.ForEach([&coordinator](spark::u32 entity_id, Position& pos, Velocity& vel) {
                pos.x += vel.vx;
                pos.y += vel.vy;
                pos.z += vel.vz;
                });
            break;
        }
        case 3: { // 20% Creates
            auto entity = coordinator.CreateEntity(
                Position{ 1.0f, 1.0f, 1.0f, 0.0f },
                Velocity{ 0.1f, 0.1f, 0.1f, 0.0f }
            );
            entities.push_back(entity);
            break;
        }
        case 4: { // 20% Destroys
            if (!entities.empty()) {
                size_t index = i % entities.size();
                coordinator.DestroyEntity(entities[index]);
                entities.erase(entities.begin() + index);
            }
            break;
        }
        default:
            break;
        }
    }

    sw.Stop();
    size_t end_mem = get_process_memory();

    return BenchResult{
        sw.GetElapsedTime(),
        OPERATIONS,
        end_mem > start_mem ? end_mem - start_mem : 0
    };
}

int main() {
    const size_t ENTITY_COUNT = 1'000'000;
    const size_t ACCESS_COUNT = 10'000'000;

    std::cout << "\nRunning Spark ECS benchmarks...\n" << std::endl;

    // Entity Creation Benchmarks
    std::cout << "=== Entity Creation Benchmarks ===" << std::endl;
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_entity_creation_simple(coordinator, ENTITY_COUNT);
        std::cout << "Simple (2 components):" << std::endl;
        std::cout << "  " << result.ops_per_second() << " entities/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/entity" << std::endl;
        std::cout << "  Memory: " << result.memory_str() << std::endl;
    }
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_entity_creation_complex(coordinator, ENTITY_COUNT);
        std::cout << "Complex (6 components):" << std::endl;
        std::cout << "  " << result.ops_per_second() << " entities/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/entity" << std::endl;
        std::cout << "  Memory: " << result.memory_str() << std::endl;
    }
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_entity_creation_large(coordinator, ENTITY_COUNT);
        std::cout << "Large (1KB component):" << std::endl;
        std::cout << "  " << result.ops_per_second() << " entities/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/entity" << std::endl;
        std::cout << "  Memory: " << result.memory_str() << std::endl;
    }

    // Entity Iteration Benchmarks
    std::cout << "\n=== Entity Iteration Benchmarks ===" << std::endl;
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_entity_iteration_simple(coordinator, ENTITY_COUNT);
        std::cout << "Simple (2 components):" << std::endl;
        std::cout << "  " << result.ops_per_second() << " entities/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/entity" << std::endl;
    }
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_entity_iteration_complex(coordinator, ENTITY_COUNT);
        std::cout << "Complex (4 components):" << std::endl;
        std::cout << "  " << result.ops_per_second() << " entities/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/entity" << std::endl;
    }

    // Component Access Benchmarks
    std::cout << "\n=== Random Component Access Benchmarks ===" << std::endl;
    {
        spark::Coordinator coordinator;
        BenchResult result = benchmark_component_access(coordinator, ACCESS_COUNT);
        std::cout << "Multiple components:" << std::endl;
        std::cout << "  " << result.ops_per_second() << " accesses/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/access" << std::endl;
    }

    // Mixed Operations Benchmark
    std::cout << "\n=== Mixed Operations Benchmark ===" << std::endl;
    {
        spark::Coordinator coordinator;
        BenchResult result = run_mixed_operations_benchmark(coordinator);
        std::cout << "Mixed operations:" << std::endl;
        std::cout << "  " << result.ops_per_second() << " ops/sec" << std::endl;
        std::cout << "  " << result.time_per_op_microseconds() << " �s/op" << std::endl;
        std::cout << "  Memory: " << result.memory_str() << std::endl;
    }

    return 0;
}