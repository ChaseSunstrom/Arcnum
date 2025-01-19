#define SPARK_USE_GL
#include "spark.hpp"

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

spark::i32 main()
{
    auto layout_descriptor = spark::CreateVertexLayout<
        spark::LayoutDescriptor{"m_i", spark::AttributeType::INT, sizeof(int)},
        spark::LayoutDescriptor{"m_j", spark::AttributeType::INT, sizeof(int)}, 
        spark::LayoutDescriptor{"m_k", spark::AttributeType::INT, sizeof(int)}
    >();

    spark::Logger::Logln(spark::LogLevel::DEBUG, "%u", layout_descriptor.GetStride());

    spark::Application<spark::opengl::GL> app("Arcnum", 1280, 720);
    app.DeltaTime(60)
        .Subscribe<Blah>([](spark::Event<Blah>& event)
            {
                spark::Logger::Logln("Received Blah Event: %d", event->i);
            })

        .SubmitEvent(Blah{1})
       .Start()
       .SubmitCommand<spark::RenderCommand>({0.2f, 0.3f, 0.4f, 1.0f})
       .Run()
       .Close();

    return 0;
}