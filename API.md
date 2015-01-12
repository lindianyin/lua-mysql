## MySQL API

### mysql.newclient()

Create and initializes a MYSQL connection client.

### mysql._VERSION

return a string of client library version info.


## Connection API

### conn:connect(config)

config is a table contains:

------------|-------------------
   host     | a host name or an IP address
   user     | user's MySQL login ID
  passwd    | password for user
    db      | database name
   port     | port number of TCP/IP connection
unix_socket | socket or named pipe to use
client_flag | feature flags

See [mysql_real_connect](http://dev.mysql.com/doc/refman/5.5/en/mysql-real-connect.html)

### conn:close()

Explicitly close this connection.

### conn:set_charset(charset)

Set the default character set.
See [Connection Character Sets](http://dev.mysql.com/doc/refman/5.5/en/charset-connection.html)

### conn:set_reconnect(is_auto_reconnect)

Enable or disable automatic reconnection to the server if the connection is found to have been lost.

### conn:set_timeout(how, timeout)

`how` can be one of following:

----------|--------------------------------
  connect | Connect timeout in seconds
    read  | The timeout in seconds for each attempt to read from the server
    write | The timeout in seconds for each attempt to write to the server.

### conn:set_protocol(protocol)

Type of protocol to use, `protocol` is enumerated in following constants:

~~~ C
mysql.PROTOCOL_DEFAULT
mysql.PROTOCOL_TCP
mysql.PROTOCOL_SOCKET
mysql.PROTOCOL_PIPE
mysql.PROTOCOL_MEMORY
~~~

### conn:set_compress()

Use the compressed client/server protocol.

### conn:escape_string(stmt)

Create a legal SQL string that you can use in an SQL statement.
See [mysql_real_escape_string](http://dev.mysql.com/doc/refman/5.5/en/mysql-real-escape-string.html)

### conn:ping()

Checks whether the connection to the server is working.
See [mysql_ping](http://dev.mysql.com/doc/refman/5.5/en/mysql-ping.html)

### conn:execute(opt)

Executes the SQL statement, opt can be one of following:

-----------|-----------------------------------
  `store`  | retrieve the entire result set all at once, this is the default behavior
  `use`    | initiate a row-by-row result set retrieval

return a `Cursor` object when succeeded.

See [mysql_real_query](http://dev.mysql.com/doc/refman/5.5/en/mysql-real-query.html)

### conn:commit()

Commits the current transaction.
See [mysql_commit](http://dev.mysql.com/doc/refman/5.5/en/mysql-commit.html)

### conn:rollback()

Rolls back the transaction.
See [mysql_rollback](http://dev.mysql.com/doc/refman/5.5/en/mysql-rollback.html)


## Cursor API

### cursor:fetch(opt)

Fetch result row-by-row, and return a table.
`opt` can be one of the following:

---------|--------------------------------
    'n'  | numeric table index, as default
    'a'  | alphabetic table index
    
### cursor:fetch_all(opt)

Fetch all result in a table.
`opt` can be one of the following:

---------|--------------------------------
    'n'  | numeric table index, as default
    'a'  | alphabetic table index
    
Cannot work with `conn:execute('use')`
