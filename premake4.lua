--
-- Premake4 build script (http://industriousone.com/premake/download)
--

local this_os = os.get()
assert(this_os == 'windows' or this_os == 'linux')

solution 'lua-mysql'
    configurations {'Debug', 'Release'}
    --flags {'ExtraWarnings'}
    targetdir 'bin'
    platforms {'x64'}

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
        links 'lua5.2'
        if os.get() == 'windows' then
        links 'libmysql'
        else
        links 'mysqlclient'
        end

    if os.get() == 'linux' then return end

    project 'lua'
        language 'C'
        kind 'ConsoleApp'
        location 'build'
        uuid '9C08AC41-18D8-4FB9-80F2-01F603917025'
        files
        {
            'dep/lua/src/lua.c',
        }
        links 'lua52'
        
    project 'lua5.2'
        language 'C'
        kind 'SharedLib'
        location 'build'
        uuid 'C9A112FB-08C0-4503-9AFD-8EBAB5B3C204'
        if os.get() == 'windows' then
        defines 'LUA_BUILD_AS_DLL'
        else
        defines 'LUA_USE_LINUX'
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
