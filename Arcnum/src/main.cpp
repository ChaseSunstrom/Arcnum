#include "spark_ecs.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <random>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
size_t getProcessMemory() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<size_t>(pmc.WorkingSetSize);
    }
    return 0;
}
#else
// Fallback for non-Windows: returns 0 (memory measurement unsupported)
size_t getProcessMemory() {
    return 0;
}
#endif

struct BenchResult {
    double time_sec;
    size_t operations;
    size_t memory_bytes;

    double opsPerSecond() const {
        return operations / time_sec;
    }
    double microsecondsPerOp() const {
        return (time_sec * 1e6) / operations;
    }
    std::string memoryString() const {
        if (memory_bytes < 1024) return std::to_string(memory_bytes) + " B";
        if (memory_bytes < 1024 * 1024) return std::to_string(memory_bytes / 1024.0) + " KB";
        return std::to_string(memory_bytes / (1024.0 * 1024.0)) + " MB";
    }
};

// Component definitions
struct Position { float x, y, z, _padding; };
struct Velocity { float x, y, z, _padding; };
struct Health { float value, _p0, _p1, _p2; };
struct Transform { float matrix[16]; };
struct Render { int mesh_id, material_id; float _p0, _p1; };
struct Physics { float mass, friction, restitution, _padding; };
struct LargeComponent { float data[256]; };

// Benchmark: Entity creation with simple components
BenchResult benchmarkEntityCreationSimple(size_t count) {
    size_t startMem = getProcessMemory();
    auto start = std::chrono::high_resolution_clock::now();
    spark::Coordinator coord;
    for (size_t i = 0; i < count; ++i) {
        coord.CreateEntity(Position{ 0,0,0,0 }, Velocity{ 0,0,0,0 });
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), count, getProcessMemory() - startMem };
}

// Benchmark: Entity creation with complex components
BenchResult benchmarkEntityCreationComplex(size_t count) {
    size_t startMem = getProcessMemory();
    auto start = std::chrono::high_resolution_clock::now();
    spark::Coordinator coord;
    for (size_t i = 0; i < count; ++i) {
        coord.CreateEntity(
            Position{ 0,0,0,0 }, Velocity{ 0,0,0,0 }, Health{ 100,0,0,0 },
            Transform{}, Render{ 0,0,0,0 }, Physics{ 1,0.5f,0.5f,0 }
        );
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), count, getProcessMemory() - startMem };
}

// Benchmark: Entity creation with a large component
BenchResult benchmarkEntityCreationLarge(size_t count) {
    size_t startMem = getProcessMemory();
    auto start = std::chrono::high_resolution_clock::now();
    spark::Coordinator coord;
    for (size_t i = 0; i < count; ++i) {
        coord.CreateEntity(LargeComponent{});
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), count, getProcessMemory() - startMem };
}

// Benchmark: Entity iteration simple
BenchResult benchmarkEntityIterationSimple(size_t count) {
    spark::Coordinator coord;
    for (size_t i = 0; i < count; ++i) {
        if (i % 2 == 0) {
            coord.CreateEntity(Position{ 0,0,0,0 }, Velocity{ 0,0,0,0 });
        } else {
            coord.CreateEntity(Position{ 0,0,0,0 });
        }
    }
    auto start = std::chrono::high_resolution_clock::now();
    size_t iterated = 0;
    auto query = coord.CreateQuery<Position, spark::Without<Velocity>>();
    query.ForEach([&](const spark::Entity& e, Position& p) {
        ++iterated;
        });
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), iterated, 0 };
}

// Benchmark: Entity iteration complex
BenchResult benchmarkEntityIterationComplex(size_t count) {
    spark::Coordinator coord;
    for (size_t i = 0; i < count; ++i) {
        coord.CreateEntity(Position{ 0,0,0,0 }, Velocity{ 0,0,0,0 }, Health{ 100,0,0,0 }, Transform{});
    }
    auto start = std::chrono::high_resolution_clock::now();
    size_t iterated = 0;
    auto query = coord.CreateQuery<Position, Velocity, Health, Transform>();
    query.ForEach([&](const spark::Entity& e, Position& p, const Velocity& v, const Health& h, const Transform& t) {
        ++iterated;
        });
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), iterated, 0 };
}

// Benchmark: Random component access
BenchResult benchmarkComponentAccess(size_t accesses) {
    std::mt19937_64 rng(12345);
    spark::Coordinator coord;
    std::vector<spark::Entity> entities;
    size_t createCount = accesses / 10;
    for (size_t i = 0; i < createCount; ++i) {
        entities.push_back(coord.CreateEntity(Position{ 0,0,0,0 }, Velocity{ 0,0,0,0 }, Health{ 100,0,0,0 }));
    }
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < accesses; ++i) {
        auto& ent = entities[rng() % entities.size()];
        coord.GetComponent<Position>(ent);
        coord.GetComponent<Velocity>(ent);
        coord.GetComponent<Health>(ent);
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), accesses, 0 };
}

// Benchmark: Mixed operations
BenchResult runMixedOperationsBenchmark() {
    const size_t OPERATIONS = 1'000'000;
    size_t startMem = getProcessMemory();
    auto start = std::chrono::high_resolution_clock::now();
    spark::Coordinator coord;
    std::vector<spark::Entity> entities;
    entities.reserve(OPERATIONS / 5);
    for (size_t i = 0; i < OPERATIONS; ++i) {
        switch (i % 5) {
        case 0:
        case 1:
        case 2: {
            auto query = coord.CreateQuery<Position, Velocity>();
            query.ForEach([](const spark::Entity&, Position& p, const Velocity& v) {
                p.x += v.x; p.y += v.y; p.z += v.z;
                });
            break;
        }
        case 3: {
            entities.push_back(coord.CreateEntity(Position{ 1,1,1,0 }, Velocity{ 0.1f,0.1f,0.1f,0 }));
            break;
        }
        case 4: {
            if (!entities.empty()) {
                size_t idx = i % entities.size();
                coord.DestroyEntity(entities[idx]);
                entities.erase(entities.begin() + idx);
            }
            break;
        }
        }
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return { std::chrono::duration<double>(elapsed).count(), OPERATIONS, getProcessMemory() - startMem };
}

int main() {
    constexpr size_t ENTITY_COUNT = 1'000'000;
    constexpr size_t ACCESS_COUNT = 10'000'000;

    std::cout << "Running Spark ECS benchmarks...\n\n";

    auto res1 = benchmarkEntityCreationSimple(ENTITY_COUNT);
    std::cout << "=== Entity Creation Simple ===\n"
        << res1.opsPerSecond() << " entities/sec, "
        << res1.microsecondsPerOp() << " us/entity, Memory: "
        << res1.memoryString() << "\n\n";

    auto res2 = benchmarkEntityCreationComplex(ENTITY_COUNT);
    std::cout << "=== Entity Creation Complex ===\n"
        << res2.opsPerSecond() << " entities/sec, "
        << res2.microsecondsPerOp() << " us/entity, Memory: "
        << res2.memoryString() << "\n\n";

    auto res3 = benchmarkEntityCreationLarge(ENTITY_COUNT);
    std::cout << "=== Entity Creation Large ===\n"
        << res3.opsPerSecond() << " entities/sec, "
        << res3.microsecondsPerOp() << " us/entity, Memory: "
        << res3.memoryString() << "\n\n";

    auto res4 = benchmarkEntityIterationSimple(ENTITY_COUNT);
    std::cout << "=== Entity Iteration Simple ===\n"
        << res4.opsPerSecond() << " entities/sec, "
        << res4.microsecondsPerOp() << " us/entity\n\n";

    auto res5 = benchmarkEntityIterationComplex(ENTITY_COUNT);
    std::cout << "=== Entity Iteration Complex ===\n"
        << res5.opsPerSecond() << " entities/sec, "
        << res5.microsecondsPerOp() << " us/entity\n\n";

    auto res6 = benchmarkComponentAccess(ACCESS_COUNT);
    std::cout << "=== Random Component Access ===\n"
        << res6.opsPerSecond() << " accesses/sec, "
        << res6.microsecondsPerOp() << " us/access\n\n";

    auto res7 = runMixedOperationsBenchmark();
    std::cout << "=== Mixed Operations ===\n"
        << res7.opsPerSecond() << " ops/sec, "
        << res7.microsecondsPerOp() << " us/op, Memory: "
        << res7.memoryString() << "\n";

    return 0;
}
