// Copyright (C) 2014 ichenq@gmail.com. All rights reserved.
// Distributed under the terms and conditions of the Apache License.
// See accompanying files LICENSE.

#ifdef _WIN32
#include <WinSock2.h>
#define LUAMYSQL_EXPORT  __declspec(dllexport)
#else
#define LUAMYSQL_EXPORT
#endif
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


#define LUAMYSQL_CONN       "Connection*"
#define LUAMYSQL_CURSOR     "Cursor*"

#define check_conn(L)   ((Connection*)luaL_checkudata(L, 1, LUAMYSQL_CONN))
#define check_cursor(L) ((Cursor*)luaL_checkudata(L, 1, LUAMYSQL_CURSOR))

#define THROW_ERROR(L, conn, msg)    \
    (luaL_error(L, "%s, %s\n", (msg), mysql_error(conn)))

typedef struct _Connection
{
    int     closed;     // is this connection closed
    MYSQL*  my_conn;    // mysql connection instance
}Connection;

typedef struct _Cursor
{
    int conn;                   // reference to connection
    int fetch_all;              // fetch all result in one query
    unsigned int    numcols;    // number of columns
    MYSQL_RES*      my_res;     // mysql result instance
    MYSQL_FIELD*    fields;     // column names and types
}Cursor;

static int create_cursor(lua_State* L, int conn, MYSQL_RES* result, 
    int numcols, int fetch_all)
{
    Cursor* cur = (Cursor*)lua_newuserdata(L, sizeof(Cursor));
    if (cur)
    {
        lua_pushvalue(L, conn);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        if (ref != LUA_NOREF)
        {
            luaL_getmetatable(L, LUAMYSQL_CURSOR);
            lua_setmetatable(L, -2);
            cur->conn = ref;
            cur->numcols = numcols;
            cur->fetch_all = fetch_all;
            cur->fields = NULL;
            cur->my_res = result;
            return 1;
        }
    }
    return 0;
}

// Closes the cursos and nullify all structure fields.
static void cursor_nullify(lua_State* L, Cursor* cur) 
{
    /* Nullify structure fields. */
    if (cur->my_res)
    {
        mysql_free_result(cur->my_res);
        cur->my_res = NULL;
        cur->fields = NULL;
    }
    if (cur->conn != LUA_NOREF)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, cur->conn);
    }
}

static int cursor_gc(lua_State *L)
{
    Cursor* cur = check_cursor(L);
    if (cur)
    {
        cursor_nullify(L, cur);
    }
    return 0;
}

static int cursor_numrows(lua_State *L) 
{
    Cursor* cur = check_cursor(L);
    luaL_argcheck(L, cur && cur->my_res, 1, "invalid Cursor object");
    my_ulonglong rows = mysql_num_rows(cur->my_res);
    lua_pushnumber(L, (lua_Number)rows);
    return 1;
}

static void pushvalue(lua_State* L, enum enum_field_types type, 
    const char* row, unsigned long len)
{
    if (row != NULL && type != MYSQL_TYPE_NULL)
    {
        switch (type)
        {
        case MYSQL_TYPE_DECIMAL:
        case MYSQL_TYPE_SHORT:
        case MYSQL_TYPE_LONG:
        case MYSQL_TYPE_FLOAT:
        case MYSQL_TYPE_DOUBLE:
        case MYSQL_TYPE_LONGLONG:
        case MYSQL_TYPE_INT24:
        case MYSQL_TYPE_YEAR:
        case MYSQL_TYPE_TINY:
            lua_pushnumber(L, atof(row));
            break;
        default:
            lua_pushlstring(L, row, len);
            break;
        }
    }
    else
    {
        lua_pushnil(L);
    }
}

static int cursor_fetch(lua_State* L)
{
    Cursor* cur = check_cursor(L);
    luaL_argcheck(L, cur && cur->my_res, 1, "invalid Cursor object");
    MYSQL_RES* res = cur->my_res;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) // no more results
    {
        cursor_nullify(L, cur);
        lua_pushnil(L);
        return 1;
    }
    unsigned long* lengths = mysql_fetch_lengths(res);
    if (cur->fields == NULL)
    {
        cur->fields = mysql_fetch_fields(res);
    }
    luaL_argcheck(L, lengths && cur->fields, 1, "fetch fields failed");
    luaL_checkstack(L, cur->numcols, "too many columns");
    for (unsigned i = 0; i < cur->numcols; i++)
    {
        pushvalue(L, cur->fields[i].type, row[i], lengths[i]);
    }
    return cur->numcols;
}

static int cursor_fetch_all(lua_State* L)
{
    Cursor* cur = check_cursor(L);
    luaL_argcheck(L, cur && cur->my_res, 1, "invalid Cursor object");
    luaL_argcheck(L, cur->fetch_all, 1, "'fetch-all' not set with execute()");
    const char* opt = lua_tostring(L, 2);
    int alpha_idx = (opt && strcmp(opt, "a") == 0);

    MYSQL_RES* res = cur->my_res;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL || cur->numcols == 0) // no results
    {
        cursor_nullify(L, cur);
        lua_pushnil(L);
        return 1;
    }
    unsigned long* lengths = mysql_fetch_lengths(res);
    if (cur->fields == NULL)
    {
        cur->fields = mysql_fetch_fields(res);
    }
    luaL_argcheck(L, lengths && cur->fields, 1, "fetch fields failed");
    int rownum = (int)mysql_num_rows(cur->my_res);
    lua_createtable(L, rownum, 0);
    rownum = 1;
    while (row)
    {
        if (alpha_idx)
        {
            lua_createtable(L, 0, cur->numcols);
            for (unsigned i = 0; i < cur->numcols; i++)
            {
                pushvalue(L, cur->fields[i].type, row[i], lengths[i]);
                lua_setfield(L, -2, cur->fields[i].name);
            }
        }
        else // numeric index
        {
            lua_createtable(L, cur->numcols, 0);
            for (unsigned i = 0; i < cur->numcols; i++)
            {
                pushvalue(L, cur->fields[i].type, row[i], lengths[i]);
                lua_rawseti(L, -2, i + 1);
            }
        }
        lua_rawseti(L, -2, rownum++);
        row = mysql_fetch_row(res);
    }
    cursor_nullify(L, cur);
    return 1;
}


static int conn_create(lua_State* L)
{
    Connection* conn = (Connection*)lua_newuserdata(L, sizeof(Connection));    
    if (conn)
    {
        conn->closed = 0;
        conn->my_conn = mysql_init(NULL);
        if (conn->my_conn)
        {
            luaL_getmetatable(L, LUAMYSQL_CONN);
            lua_setmetatable(L, -2);
            return 1;
        }
    }
    luaL_error(L, "%s\n", "insufficient memory");
    return 0;
}

static int conn_gc(lua_State* L)
{
    Connection* conn = check_conn(L);
    if (conn && !conn->closed)
    {
        mysql_close(conn->my_conn);
        conn->my_conn = NULL;
        conn->closed = 1;
    }
    return 0;
}

static int conn_close(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    if (!conn->closed)
    {
        conn_gc(L);
        lua_pushboolean(L, 1);
    }
    else
        lua_pushboolean(L, 0);
    return 1;
}

static int conn_tostring(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    lua_pushfstring(L, "Connection* (%p)", conn);
    return 1;
}

static int conn_set_charset(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    const char* charset = luaL_checkstring(L, 2);
    if (mysql_options(conn->my_conn, MYSQL_SET_CHARSET_NAME, charset) != 0)
    {
        THROW_ERROR(L, conn->my_conn, "set_charset() failed");
    }
    return 0;
}

static int conn_set_reconnect(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    my_bool val = (my_bool)lua_toboolean(L, 2);
    if (mysql_options(conn->my_conn, MYSQL_OPT_RECONNECT, &val) != 0)
    {
        THROW_ERROR(L, conn->my_conn, "set_reconnect() failed");
    }
    return 0;
}

static int conn_set_timeout(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    const char* option = luaL_checkstring(L, 2);
    unsigned int timeout = (unsigned int)luaL_checkinteger(L, 3);
    int error = 0;
    if (strcmp(option, "connect") == 0)
    {
        error = mysql_options(conn->my_conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    }
    else if (strcmp(option, "read") == 0)
    {
        error = mysql_options(conn->my_conn, MYSQL_OPT_READ_TIMEOUT, &timeout);
    }
    else if (strcmp(option, "write") == 0)
    {
        error = mysql_options(conn->my_conn, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    }
    else
    {
        luaL_error(L, "invalid timeout option '%s'", option);
    }
    if (error != 0)
    {
        THROW_ERROR(L, conn->my_conn, "set_timeout() failed");
    }
    return 0;
}

static int conn_ping(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    if (mysql_ping(conn->my_conn) != 0)
    {
        THROW_ERROR(L, conn->my_conn, "ping() failed");
    }
    return 0;
}

static int conn_connect(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    luaL_argcheck(L, lua_istable(L, 2), 1, "argument must be table");
    lua_getfield(L, 2, "host");
    const char* host = luaL_checkstring(L, -1);
    lua_getfield(L, 2, "user");
    const char* user = luaL_checkstring(L, -1);
    lua_getfield(L, 2, "passwd");
    const char* passwd = luaL_checkstring(L, -1);
    lua_getfield(L, 2, "db");
    const char* db = luaL_checkstring(L, -1);
    lua_getfield(L, 2, "port");
    unsigned int port = luaL_optint(L, -1, 3306);
    lua_getfield(L, 2, "unix_socket");
    const char* unix_socket = luaL_optstring(L, -1, NULL);
    // enable multiple result set and multiple statements
    lua_getfield(L, 2, "client_flag");
    unsigned long flags = luaL_optint(L, -1, CLIENT_MULTI_RESULTS | CLIENT_MULTI_QUERIES);
    lua_pop(L, 7);
    if (mysql_real_connect(conn->my_conn, host, user, passwd, db, port,
        unix_socket, flags) != conn->my_conn)
    {
        THROW_ERROR(L, conn->my_conn, "connect() failed");
    }
    return 0;
}

static int conn_escape_string(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    size_t length = 0;
    const char* stmt = luaL_checklstring(L, 2, &length);
    char* dest = (char*)malloc(length*2 + 1);
    if (dest)
    {
        unsigned long newlen = mysql_real_escape_string(conn->my_conn, dest, stmt, 
            (unsigned long)length);
        lua_pushlstring(L, dest, newlen);
        free(dest);
        return 1;
    }
    return 0;
}

static int conn_commit(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    lua_pushboolean(L, !mysql_commit(conn->my_conn));
    return 1;
}

static int conn_rollback(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    lua_pushboolean(L, !mysql_rollback(conn->my_conn));
    return 1;
}

static int conn_execute(lua_State* L)
{
    Connection* conn = check_conn(L);
    luaL_argcheck(L, conn && conn->my_conn, 1, "invalid Connection object");
    size_t length = 0;
    const char* stmt = luaL_checklstring(L, 2, &length);
    const char* fetch_opt = luaL_optstring(L, 3, "fetch-all");
    int fetch_all = strcmp(fetch_opt, "fetch-all") == 0;
    MYSQL* my_conn = conn->my_conn;
    if (mysql_real_query(my_conn, stmt, (unsigned long)length) == 0)
    {
        MYSQL_RES* res = (fetch_all ? mysql_store_result(my_conn)
            : mysql_use_result(my_conn));
        unsigned int numcols = mysql_field_count(my_conn);
        if (res)
        {
            return create_cursor(L, 1, res, numcols, fetch_all);
        }
        else
        {
            if (numcols == 0) // query does not return data (not SELECT)
            {
                lua_pushinteger(L, (lua_Integer)mysql_affected_rows(my_conn));
                return 1;
            }
        }
    }
    THROW_ERROR(L, my_conn, "execute() failed");
    return 0;
}

static void make_meta(lua_State* L, const char* name, const luaL_Reg* metalib)
{
    if (luaL_newmetatable(L, name))
    {
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, metalib, 0);
    }
    else
    {
        luaL_error(L, "create metatable failed: %s", name);
    }
}

static void create_metatables(lua_State* L)
{
    static const luaL_Reg conn_methods[] =
    {
        { "__gc", conn_gc },
        { "__tostring", conn_tostring },
        { "set_charset", conn_set_charset },
        { "set_reconnect", conn_set_reconnect },
        { "set_timeout", conn_set_timeout },
        { "escape", conn_escape_string },
        { "connect", conn_connect },
        { "ping", conn_ping },
        { "close", conn_close },
        { "execute", conn_execute },
        { "commit", conn_commit },
        { "rollback", conn_rollback },
        { NULL, NULL },
    };
    static const luaL_Reg cur_methods[] =
    { 
        { "__gc", cursor_gc },
        { "fetch", cursor_fetch },
        { "fetch_all", cursor_fetch_all },
        { "numrows", cursor_numrows },
        { NULL, NULL },
    };
    make_meta(L, LUAMYSQL_CONN, conn_methods);
    make_meta(L, LUAMYSQL_CURSOR, cur_methods);
    lua_pop(L, 1);  /* pop new metatable */
}

LUAMYSQL_EXPORT int luaopen_luamysql(lua_State* L)
{
    static const luaL_Reg lib[] =
    {
        { "create", conn_create },
        { NULL, NULL },
    };
    create_metatables(L);
    luaL_newlib(L, lib);
    lua_pushliteral(L, "_VERSION");
    lua_pushstring(L, mysql_get_client_info());
    lua_settable(L, -3);
    return 1;
}
