-- OUT_DIR and INT_DIR must be defined in the top-most premake file before including this file
-- Since librevenge depends on boost and zlib, BOOST_DIR and ZLIB_DIR must be defined before including this file

project "librevenge"
    kind "SharedLib"
    language "C++"

    debugdir (OUT_DIR)
    targetdir (OUT_DIR)
    objdir (INT_DIR)

    files
    {
        "inc/**.h",
        "src/lib/**.h",
        "src/lib/**.cpp"
    }

    includedirs
    {
        "inc",
        "src/lib",
        "%{BOOST_DIR}",
        "%{ZLIB_DIR}"
    }

    links
    {
        "zlib"
    }

    defines
    {
        "DLL_EXPORT",
        "LIBREVENGE_BUILD",
        "LIBREVENGE_STREAM_BUILD",
        "LIBREVENGE_GENERATORS_BUILD",
        "ZLIB_DLL"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"

    filter "system:linux"
        systemversion "latest"
        cppdialect "gnu++17"

    filter "system:macosx"
        systemversion "latest"
        cppdialect "gnu++17"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "Full"

    filter "configurations:not Debug"
        runtime "Release"
        symbols "Off"
        optimize "Full"
