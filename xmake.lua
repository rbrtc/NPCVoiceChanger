-- include subprojects
includes("lib/commonlibsse-ng", "extern/clib-util")

-- packages
add_requires("simpleini 4.25", "nlohmann_json 3.12.0")

-- set project constants
set_project("NPCVoiceChanger")
set_version("1.0.0")
set_license("GPL-3.0")
set_languages("c++23")
set_warnings("less")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- define targets
target("NPCVoiceChanger")
add_packages("simpleini", "nlohmann_json")
add_deps("clib-util")
add_rules("commonlibsse-ng.plugin", {
	name = "NPCVoiceChanger",
	author = "rbrtc",
	description = "Changes voices of NPCs",
})

-- add src files
add_files("src/**.cpp")
add_headerfiles("src/**.h")
add_includedirs("src")
set_pcxxheader("src/pch.h")
