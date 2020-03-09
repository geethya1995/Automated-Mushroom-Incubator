const con = require('./db');

var writeData = (data) => {
    var sql = "INSERT INTO Sensor_Data(Temperature, Humidity, LightIntensity, SoilMoisture) VALUES("+data.temp+", "+data.hum+", "+data.light+", "+data.soil+")";
    
    try{
        con.query(sql, (err, result) => {
            if(err) throw err;
            console.log(result);
        });
    }catch(err){
        throw err;
    }
    
}

module.exports = writeData;


