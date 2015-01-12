lua-mysql
===============

lua-mysql is a simple(but not complete) Lua 5.2 binding of [MySQL 5.5 C API](http://dev.mysql.com/doc/refman/5.5/en/c-api.html)

## Installation

First obtain [premake4](http://industriousone.com/premake/download).

### Build on Windows 7 x64

1. Type `premake4 vs2013` on command window to generate MSVC solution files.
2. Use Visual Studio 2013(either Ultimate or Community version) to compile executable binary.

### Build on Ubuntu 14.04 64-bit

1. Type `sudo apt-get install lua5.2 liblua5.2-dev libmysqlclient-dev`
2. Type `premake4 gmake && make config=release64`


## Usage at a glance

~~~~~~~~~~lua
local mysql = require 'luamysql'
local client = mysql.newclient()
client:connect{host = 'localhost', user = 'root', passwd = 'mypwd', db = 'test'}
local cur = client:execute('select * from test')
while true do
     local r = cur:fetch()
     for k, v in ipairs(r) do
        print(k, v)
     end
end
~~~~~~~~~~

See more [exampes](https://github.com/ichenq/lua-mysql/tree/master/test) and [API doc](https://github.com/ichenq/lua-mysql/blob/master/API.md)
