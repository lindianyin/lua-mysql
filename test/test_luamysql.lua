local mysql = require 'luamysql'
assert(type(mysql) == 'table')

print('library version', mysql._VERSION)

local config = 
{
    host = 'localhost',
    user = 'root',
    passwd = 'holyshit',
    db = 'world'
}

local function test_set_option()
    local c = mysql.newclient()
    c:set_charset('utf8')
    c:set_reconnect(true)
    c:connect(config)
    c:ping()
    c:close()
end

local function test_query()
    local client = mysql.newclient()
    local stmt = 'select * from country limit 10'
    local cur = client:execute(stmt)
    local result = cur:fetch_all('a')
    for id, res in ipairs(result) do
        print(id)
        for k, v in pairs(res) do
            print(k, v)
        end
    end
    client:close()
end

local function test_update()
end

local function test_delete()
end

local function test_insert()
end

local function main()
    test_set_option()
    test_query()
end

main{...}