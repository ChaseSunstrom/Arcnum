#ifndef SPARK_MODDING_HPP
#define SPARK_MODDING_HPP

#include <string>
#include <vector>

namespace spark
{
	class Application;

	class IMod
	{
	public:
        virtual ~IMod() = default;

        // Called when the mod is loaded
        virtual void OnLoad(Application* app) = 0;

        // Called when the mod is unloaded
        virtual void OnUnload() = 0;

        // Metadata
        virtual std::string GetModName() const = 0;
        virtual std::string GetModVersion() const = 0;
        virtual std::vector<std::string> GetDependencies() const = 0;
	};
}

#endif