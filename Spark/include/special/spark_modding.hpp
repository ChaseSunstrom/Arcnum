#ifndef SPARK_MODDING_HPP
#define SPARK_MODDING_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include "spark_defines.hpp"


namespace spark
{
	class Application;

	class SPARK_API IMod
	{
	public:
        IMod() = default;
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

    // Implemented for each Mod individually
	extern "C" MOD_API IMod* CreateMod();

    class SPARK_API ModManager
    {
    public:
        ModManager(Application* app);
        ~ModManager();

        // Load all mods from a directory
        void LoadMods(const std::string& directory);

        // Unload all mods
        void UnloadAllMods();

        // Get a loaded mod by name
        IMod* GetMod(const std::string& mod_name);


#ifdef _WIN32
        HMODULE
#else
        void*
#endif 
            LoadLibrary(const std::string& path);
        void UnloadLibrary(void* handle);
        IMod* LoadModInstance(void* library_handle);

        // Dependency validation
        bool ValidateDependencies(IMod* mod_instance);

    private:
        
        struct ModHandle
        {

#ifdef _WIN32
            HMODULE library_handle = nullptr;
#else
            void* library_handle = nullptr;
#endif
            IMod* mod_instance = nullptr;

			ModHandle() = default;

			ModHandle(decltype(library_handle) lh, IMod* mi)
				: library_handle(lh), mod_instance(mi) {}
        };

        // Raw pointer to dismiss warnings about dll-import
        struct Impl;
		Impl* m_impl;
    };
}

#endif