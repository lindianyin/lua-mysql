
local mysql = require 'luamysql'
assert(type(mysql) == 'table')

local config = 
{
    host = 'localhost',
    user = 'root',
    passwd = 'holyshit',
    db = 'world'
}
local conn = mysql.create()
conn:set_charset('utf8')
conn:set_reconnect(true)

conn:connect(config)
conn:ping()

local stmt = 'select * from country limit 10'
local cur = conn:execute(stmt)
local result = cur:fetch_all('a')
for id, res in ipairs(result) do
    print(id)
    for k, v in pairs(res) do
        print(k, v)
    end
end