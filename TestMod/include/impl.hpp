#ifndef TESTMOD_IMPL_HPP
#define TESTMOD_IMPL_HPP

#include "special/spark_modding.hpp"
#include "spark_log.hpp"

class TestMod : public spark::IMod
{
public:
    TestMod() = default;

    void OnLoad(spark::Application* app) override
    {
        spark::Logger::Logln(spark::LogLevel::INFO, "TestMod loaded!");
    }

    void OnUnload() override
    {
        spark::Logger::Logln(spark::LogLevel::INFO, "TestMod unloaded!");
    }

    std::string GetModName() const override
    {
        return "TestMod";
    }

    std::string GetModVersion() const override
    {
        return "1.0.0";
    }

    std::vector<std::string> GetDependencies() const override
    {
        return {};
    }
};

extern "C" MOD_API spark::IMod* CreateMod();

#endif