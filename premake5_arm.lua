-- A solution
workspace "focas_ubus"
	configurations { "Debug", "Release"}
	location "build"
	
project "focas_ubus"
	kind "ConsoleApp"
	language "C"
	location "build/focas_ubus"
	targetdir "bin/%{cfg.buildcfg}"
	dependson { "libjson-c-static", "libubox-static", "libubus-static" }

	includedirs { 
		".",
		"./src",
		"./deps",
		"./deps/ubus",
		"./focas"
	}
	files {
		"./src/main.c" 
	}
	libdirs {
		--"./focas/x86"
		"./focas/arm/"
	}
	links { "libubus-static", "libubox-static", "libjson-c-static", "fwlib32", "pthread", "stdc++", "rt"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"


project "libubus-static"
	kind "StaticLib"
	language "C"
	location "build/libubox"
	targetprefix ""
	targetdir "bin/%{cfg.buildcfg}"
	dependson { "libjson-c-static", "libubux-static" }

	defines {
		"UBUS_MAX_MSGLEN=1048576",
		'UBUS_UNIX_SOCKET="/var/run/ubus.sock"'
	}
	includedirs { 
		".",
		"./deps/libubox/",
		"./deps/",
	}
	files { 
		"./deps/ubus/libubus*.h",
		"./deps/ubus/libubus*.c",
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"


project "libubox-static"
	kind "StaticLib"
	language "C"
	location "build/libubox"
	targetprefix ""
	targetdir "bin/%{cfg.buildcfg}"
	dependson { "libjson-c-static" }

	includedirs { 
		".",
		"./deps/",
	}
	files {
		"./deps/libubox/avl.c",
		"./deps/libubox/avl-cmp.c",
		"./deps/libubox/blob.c",
		"./deps/libubox/blobmsg.c",
		"./deps/libubox/uloop.c",
		"./deps/libubox/usock.c",
		"./deps/libubox/ustream.c",
		"./deps/libubox/ustream-fd.c",
		"./deps/libubox/vlist.c", 
		"./deps/libubox/utils.c",
		"./deps/libubox/safe_list.c",
		"./deps/libubox/runqueue.c",
		"./deps/libubox/md5.c",
		"./deps/libubox/kvlist.c",
		"./deps/libubox/ulog.c",
		"./deps/libubox/base64.c",
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"


project "libjson-c-static"
	kind "StaticLib"
	language "C"
	location "build/json-c"
	targetdir "bin/%{cfg.buildcfg}"

	includedirs { 
		".",
	}
	files { 
		"./deps/json-c/*.h",
		"./deps/json-c/*.c",
	}

	--buildoptions { '-Wpedantic', '-pedantic', '-pedantic-errors', "-fPIC" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

