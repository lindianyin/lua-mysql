lua-mysql
===============

simple(but not complete) Lua 5.2 binding of MySQL 5.5 C API

## Installation

1. Obtain [premake4](http://industriousone.com/premake/download).
2. `premake4 gmake` on Linux or `premake4 vs2013` on Windows.


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

See more [exampes](https://github.com/ichenq/lua-mysql/tree/master/test)