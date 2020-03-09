const mysql = require('mysql');
const keys = require('./keys');

var con = mysql.createConnection({
    host: keys.db.host,
    user: keys.db.username,
    password: keys.db.password,
    database : keys.db.database
  });

module.exports = con;