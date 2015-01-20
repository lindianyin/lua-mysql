--
-- Premake4 build script (http://industriousone.com/premake/download)
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
        }
    configuration 'gmake'
        buildoptions '-std=c99'
        links 
        {
            'm',
            'dl',
        }
        
    project 'luamysql'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid 'A75AF625-DDF0-4E60-97D8-A2FDC6229AF7'
        if os.get() == 'windows' then
        defines 'inline=__inline'
        end        
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

    project 'lua'
        language 'C'
        kind 'ConsoleApp'   
        if os.get() == 'linux' then
        defines 'LUA_USE_LINUX'
        links { 'm', 'dl', 'readline'}
        end             
        files
        {
            'dep/lua/src/lua.c',
        }
        links 'lua5.3'


    project 'lua5.3'
        language 'C'
        kind 'SharedLib'
        if os.get() == 'linux' then
        defines 'LUA_USE_LINUX'
        links { 'm', 'dl', 'readline'}
        else
        defines 'LUA_BUILD_AS_DLL'
        end
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
        