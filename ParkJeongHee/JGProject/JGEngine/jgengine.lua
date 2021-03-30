

function DebugConfig()
    symbols  "On"
    optimize "Off"
    defines {"_DEBUG"}
    cppdialect "C++17"
  end


function ReleaseConfig()
    optimize "Full" 
    defines {"_RELEASE"}
    cppdialect "C++17"
end

workspace "JGEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    function SetStaticLibConfig(path)
        location  "Build"
        kind "StaticLib"
        language "C++"
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")


        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
        }



        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()
    end

    function SetSharedLibConfig(defined, path)
        location  "Build"
        kind "SharedLib"
        language "C++"
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")

        defines(defined)

        -- file
        files {
            path .. "**.h",
            path .. "**.cpp",
            path .. "**.c",
        }

        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()

    end

    function SetConsoleAppConfig(path)
        location  "Build"
        kind "ConsoleApp"
        language "C++"
        targetdir ("Bin/%{cfg.buildcfg}/")
        objdir("Build/Obj/%{cfg.buildcfg}/")

        -- file
        files {
            path .. "**.h",
            path .. "**.cpp"
        }

        filter "configurations:Debug"
            DebugConfig()
        filter "configurations:Release"
            ReleaseConfig()
    end










    os.mkdir("Bin");
    os.mkdir("Build");



    group "Engine"
        group "Engine/Core"
            project "Core"
                includedirs{
                    "Source/Core/",
                    "ThirdParty/",
                }
                links {
                    "Imgui",
                    "stb",
                }
                pchheader "pch.h"
                pchsource "Source/Core/pch.cpp"
                SetStaticLibConfig("Source/Core/")
        group "Engine/Editor"
            project "Editor"
                includedirs{
                    "Source/Core/",
                    "Source/Editor/",
                    "ThirdParty/",
                }
                links {
                    "Core"
                }
                SetConsoleAppConfig("Source/Editor/")
    group "ThirdParty"
        project "Imgui"
            SetStaticLibConfig("ThirdParty/Imgui/")
        project "stb"
            SetStaticLibConfig("ThirdParty/stb/")


        

        
    






