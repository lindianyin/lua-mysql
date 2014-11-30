local mysql = require 'luamysql'

print('mysql client version:', mysql._VERSION)

local flag = bit32.bor(mysql.CLIENT_MULTI_STATEMENTS, mysql.CLIENT_MULTI_RESULTS)
local conf = 
{
    host = 'localhost',
    user = 'root',
    passwd = 'holyshit',
    db = 'world',
    client_flag = flag
}

local client = mysql.newclient()
client:set_charset('utf8')
client:set_reconnect(true)
client:set_timeout('connect', 3)
client:set_timeout('read', 3)
client:set_timeout('write', 3)
client:set_protocol(mysql.PROTOCOL_TCP)
client:set_compress()
client:connect(conf)
client:close()