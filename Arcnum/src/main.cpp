#define SPARK_USE_GL
#include "spark.hpp"
#include "spark_ecs.hpp"

struct TestVertex
{
    int i;
    int j;
    int k;
};

struct Blah
{
    int i;
};

struct Blah2
{
    int j;
};

struct Blah3
{
    int k;
};

spark::i32 main()
{
    auto layout_descriptor = spark::CreateVertexLayout<
        spark::LayoutDescriptor{"m_i", spark::AttributeType::INT, sizeof(int)},
        spark::LayoutDescriptor{"m_j", spark::AttributeType::INT, sizeof(int)}, 
        spark::LayoutDescriptor{"m_k", spark::AttributeType::INT, sizeof(int)}
    >();

    spark::Coordinator coordinator;
    spark::Stopwatch sw;

    sw.Start();
    
    for (int i = 0; i < 12000000; i++)
    {
        auto ent = coordinator.CreateEntity(Blah{1}, Blah2{2}, Blah3{3});
    }

    sw.Stop();

    spark::Logger::Logln(spark::LogLevel::DEBUG, "%lf", sw.GetElapsedTime());

    spark::Logger::Logln(spark::LogLevel::DEBUG, "%u", layout_descriptor.GetStride());

    spark::Application app(spark::GraphicsApi::OPENGL, "Arcnum", 1280, 720);
    app.DeltaTime(60)
        .Subscribe<Blah>([](spark::Event<Blah>& event)
            {
                spark::Logger::Logln("Received Blah Event: %d", event->i);
            })

        .Subscribe<Blah>([](spark::Event<Blah>& event)
            {
                spark::Logger::Logln("Received Blah Event: %d", event->i * 2);
            })

        .SubmitEvent(Blah{1})
        .SubmitEvent(Blah{ 2 })
        .SubmitEvent(Blah{ 3 })
       .Start()
       .SubmitCommand<spark::RenderCommand>({0.2f, 0.3f, 0.4f, 1.0f})
       .Run()
       .Close();

    return 0;
}