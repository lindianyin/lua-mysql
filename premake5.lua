--
-- Premake script (http://premake.github.io)
--

assert(os.get() == 'windows' or os.get() == 'linux')

solution 'lua-mysql'
    configurations {'Debug', 'Release'}
    targetdir 'bin'

    configuration 'Debug'
        defines { 'DEBUG' }
        flags { 'Symbols' }

    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Symbols', 'Optimize' }

    configuration 'vs*'
        defines
        {
            'WIN32',
            'WIN32_LEAN_AND_MEAN',
            '_WIN32_WINNT=0x0600',
            '_CRT_SECURE_NO_WARNINGS',
            'NOMINMAX',
			'LUA_BUILD_AS_DLL',
			'inline=__inline',
        }
    configuration 'gmake'
        buildoptions '-std=c99'
        links 
        {
            'm',
            'dl',
        }
        
    project 'luamysql'
        targetname 'luamysql'
        location 'build'
        language 'C'
        kind 'SharedLib'
                    
		defines {'LUA_LIB'}
        files
        {
            'src/*.h',
            'src/*.c',
        }
        includedirs
        {
            'dep/libmysql',
            'dep/lua/src',
        }
        libdirs 'bin'
        links 'lua5.3'
        
        configuration 'windows'
            links 'libmysql'
            
        configuration 'linux'
            links 'mysqlclient'        
     
	project 'lua'
        targetname 'lua'
        location 'build'
        language 'C'
        kind 'ConsoleApp'
        
        files
        {
            'dep/lua/src/lua.c',
        }
        links 'lua5.3'
        
        configuration 'linux'
            defines 'LUA_USE_LINUX'
            links { 'm', 'dl', 'readline'}          

    project 'lua5.3'
        targetname 'lua5.3'
        location 'build'
        language 'C'
        kind 'SharedLib'
        files
        {
            'dep/lua/src/*.h',
            'dep/lua/src/*.c',
        }
        removefiles
        {
            'dep/lua/src/lua.c',
            'dep/lua/src/luac.c',
        }
        
