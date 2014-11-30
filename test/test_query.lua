local mysql = require 'luamysql'

local conf =
{
    host = 'localhost',
    user = 'root',
    passwd = 'holyshit',
    db = 'world',
}

local stmt = [[select * from country where continent='asia' limit 2]]

local function query_use_result()
    print('query_use_result:')
    local client = mysql.newclient()
    client:connect(conf)
    local cur = client:execute(stmt, 'use')
    local result = {}
    while true do
         local r = cur:fetch() -- numeric table index
         if not r then break end
         for k, v in ipairs(r) do
            print(v)
         end
         print('-----------------------')
    end
    print('-----------------------')
end

local function query_use_result_alpha_index()
    print('query_use_result_alpha_index:')
    local client = mysql.newclient()
    client:connect(conf)
    local cur = client:execute(stmt, 'use')
    while true do
         local r = cur:fetch('a') -- alphabetic table index
         if not r then break end
         for k, v in pairs(r) do
            print(k, v)
         end
         print('-----------------------')
    end
    print('-----------------------')
end

local function query_store_result()
    print('query_store_result:')
    local client = mysql.newclient()
    client:connect(conf)
    local cur = client:execute(stmt)
    local result = cur:fetch_all()
    for _, row in ipairs(result) do
        for _, v in ipairs(row) do
            print(v)
        end
        print('-----------------------')
    end
    print('-----------------------')
end

local function query_store_result_alpha_index()
    print('query_store_result_alpha_index:')
    local client = mysql.newclient()
    client:connect(conf)
    local cur = client:execute(stmt)
    local num_rows = cur:numrows()
    for n=1, num_rows do
        local r = cur:fetch('a') -- alphabetic table index
        for k, v in pairs(r) do
            print(k, v)
        end
        print('-----------------------')
    end
    print('-----------------------')
end

query_use_result()
query_use_result_alpha_index()
query_store_result()
query_store_result_alpha_index()
