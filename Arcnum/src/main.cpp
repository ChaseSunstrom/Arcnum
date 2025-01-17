#define SPARK_USE_GL
#include "spark.hpp"

struct TestVertex
{
    int i;
    int j;
    int k;
};

spark::i32 main()
{
    auto& layout_descriptor = spark::CreateVertexLayout<
        TestVertex,
        spark::LayoutDescriptor{"m_i", spark::AttributeType::INT, sizeof(int)},
        spark::LayoutDescriptor{"m_j", spark::AttributeType::INT, sizeof(int)}, 
        spark::LayoutDescriptor{"m_k", spark::AttributeType::INT, sizeof(int)}
    >();

    spark::Logger::Logln(spark::LogLevel::DEBUG, "%u", layout_descriptor.GetStride());

    spark::Application<spark::opengl::GL> app("Arcnum", 1280, 720);
    app.DeltaTime(60)
       .Start()
       .Run()
       .Close();

    return 0;
}