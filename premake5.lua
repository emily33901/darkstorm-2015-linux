-- premake5.lua
workspace "Darkstorm"
   configurations { "Debug", "Release" }

   platforms { "x32"}

project "Darkstorm"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }
	  buildoptions{ "-fpermissive -fPIC -std=c++11" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
	  buildoptions{ "-fpermissive -fPIC -std=c++11" }
