#include "spark_pch.hpp"
#include "spark_log.hpp"

#include "special/spark_modding.hpp"

#ifdef ERROR
#undef ERROR
#endif

namespace fs = std::filesystem;

namespace spark
{
    struct ModManager::Impl
    {
        Impl(Application* app) : app(app) {}

        Application* app;
        std::unordered_map<std::string, ModHandle> loaded_mods;
    };

    ModManager::ModManager(Application* app)
	    : m_impl(new Impl(app))
    {
    }

    ModManager::~ModManager()
    {
        UnloadAllMods();
        delete m_impl;
    }

    void ModManager::LoadMods(const std::string& directory)
    {
        std::string base_directory;

#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        base_directory = std::filesystem::path(buffer).parent_path().string();
#else
        char buffer[1024];
        ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (count != -1)
        {
            base_directory = std::filesystem::path(std::string(buffer, count)).parent_path().string();
        }
#endif

        // Construct the full path to the mods directory
        std::string mods_directory = base_directory + "/" + directory;

        // Ensure the mods directory exists
        if (!fs::exists(mods_directory))
        {
            fs::create_directory(mods_directory);
        }

        if (fs::is_empty(mods_directory))
        {
            Logger::Logln(LogLevel::INFO, "No mods found in directory: %s", mods_directory.c_str());
            return;
        }

        for (const auto& entry : fs::directory_iterator(mods_directory))
        {
            if (entry.is_regular_file() &&
                (entry.path().extension() == ".dll" || entry.path().extension() == ".so"))
            {
                Logger::Logln(LogLevel::INFO, "Loading mod: %s", entry.path().c_str());

                auto handle = LoadLibrary(entry.path().string());
                if (!handle)
                {
                    Logger::Logln(LogLevel::ERROR, "Failed to load library: %s", entry.path().c_str());
                    continue;
                }

                IMod* mod_instance = LoadModInstance(handle);
                if (!mod_instance)
                {
                    UnloadLibrary(handle);
                    continue;
                }

                if (!ValidateDependencies(mod_instance))
                {
                    Logger::Logln(LogLevel::ERROR, "Dependencies not satisfied for mod: %s", mod_instance->GetModName().c_str());
                    delete mod_instance;
                    UnloadLibrary(handle);
                    continue;
                }

                mod_instance->OnLoad(m_impl->app);
                m_impl->loaded_mods[mod_instance->GetModName()] = ModHandle{ handle, mod_instance };
                Logger::Logln(LogLevel::INFO, "Successfully loaded mod: %s", mod_instance->GetModName().c_str());
            }
        }
    }

    void ModManager::UnloadAllMods()
    {
        for (auto& [name, handle] : m_impl->loaded_mods)
        {
            handle.mod_instance->OnUnload();
            delete handle.mod_instance;
            UnloadLibrary(handle.library_handle);
        }
        m_impl->loaded_mods.clear();
    }

    IMod* ModManager::GetMod(const std::string& mod_name)
    {
        if (m_impl->loaded_mods.find(mod_name) != m_impl->loaded_mods.end())
        {
            return m_impl->loaded_mods[mod_name].mod_instance;
        }
        return nullptr;
    }

#ifdef _WIN32
    HMODULE
#else
    void*
#endif
    	ModManager::LoadLibrary(const std::string& path)
    {
#ifdef _WIN32
        return ::LoadLibraryA(path.c_str());
#else
        return dlopen(path.c_str(), RTLD_LAZY);
#endif
    }

    void ModManager::UnloadLibrary(void* handle)
    {
#ifdef _WIN32
        ::FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
    }

    IMod* ModManager::LoadModInstance(void* library_handle)
    {
#ifdef _WIN32
        auto create_mod = reinterpret_cast<IMod * (*)()>(
            GetProcAddress(static_cast<HMODULE>(library_handle), "CreateMod"));
#else
        auto create_mod = reinterpret_cast<IMod * (*)()>(
            dlsym(library_handle, "CreateMod"));
#endif

        if (!create_mod)
        {
            Logger::Logln(LogLevel::ERROR, "Failed to find CreateMod entry point.");
            return nullptr;
        }

        IMod* mod_instance = create_mod();
        if (!mod_instance)
        {
            Logger::Logln(LogLevel::ERROR, "Failed to create mod instance.");
        }
        return mod_instance;
    }

    bool ModManager::ValidateDependencies(IMod* mod_instance)
    {
        const auto& dependencies = mod_instance->GetDependencies();
        for (const auto& dependency : dependencies)
        {
            if (m_impl->loaded_mods.find(dependency) == m_impl->loaded_mods.end())
            {
                Logger::Logln(LogLevel::ERROR, "Missing dependency: %s", dependency.c_str());
                return false;
            }
        }
        return true;
    }
}