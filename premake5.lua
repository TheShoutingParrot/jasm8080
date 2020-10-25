workspace "jasm8080"
        configurations { "Debug", "Release" }

        kind "ConsoleApp"
        language "C"

	includedirs { "include/" }

project "jasm8080"
        targetdir "bin/%{cfg.buildcfg}"

        files { "include/*.h", "src/*.c" }

        filter "configurations:Debug"
                defines { "DEBUG" }
		buildoptions { "-g", "-Wall" }
                symbols "On"
	
	filter "configurations:Release"
		optimize "Speed"
