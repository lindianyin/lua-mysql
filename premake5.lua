--
-- Premake script (http://premake.github.io)
--

assert(os.get() == 'windows' or os.get() == 'linux')

solution 'lua-mysql'
    configurations {'Debug', 'Release'}
    --flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x32', 'x64'}

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
            '_SCL_SECURE_NO_WARNINGS',
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
        
	project 'lua'
		language 'C'
		kind 'ConsoleApp'
		location 'build'
		files
		{
			'dep/lua/src/lua.c',
		}
		includedirs
		{
			'dep/lua/src',
		}
		libdirs 'bin'
		links 'lua5.3'
		
	project 'lua5.3'
		language 'C'
		kind 'SharedLib'
		location 'build' 		
		files
		{
			'dep/lua/src/*.h',
			'dep/lua/src/*.c',
		}
		excludes
		{
			'dep/lua/src/lua.c',
			'dep/lua/src/luac.c',
		}
		includedirs
		{
			'dep/lua/src',
		}		
		
    project 'luamysql'
        language 'C'
        kind 'SharedLib'
        location 'build'
		defines 'LUA_LIB'
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
        if os.get() == 'windows' then
        links 'libmysql'
        else
        links 'mysqlclient'
        end
