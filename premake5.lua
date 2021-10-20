workspace "jaz80"
        configurations { "Release", "Debug"  }

        kind "ConsoleApp"
        language "C"

	includedirs { "include/" }

project "jaz80"
        targetdir "bin/%{cfg.buildcfg}"

        files { "include/*.h", "src/*.c" }

        filter "configurations:Debug"
                defines { "DEBUG" }
		buildoptions { "-g", "-Wall" }
                symbols "On"
	
	filter "configurations:Release"
		optimize "Speed"
