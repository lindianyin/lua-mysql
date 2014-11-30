local mysql = require 'luamysql'

local conf =
{
    host = 'localhost',
    user = 'root',
    passwd = 'holyshit',
    db = 'world',
}

local stmt = [[CREATE TABLE IF NOT EXISTS `mytest`(
    `id` int NOT NULL UNIQUE KEY,
    `name` varchar(32) NOT NULL,
    `score` int DEFAULT 0
    ) ]]

local client = mysql.newclient()
client:connect(conf)
local r = client:execute(stmt)

local total = 10000
for n=1, total do
    local fmt = [[insert into mytest values(%d, '%s', %d)]]
    local stmt = string.format(fmt, n, 'nn' .. n, math.random(100))
    assert(client:execute(stmt) == 1)
    print(stmt)
end

r = client:execute('delete from mytest')
assert(r == total)
print('passed')