const keys = require('./keys');
var mqtt = require('mqtt');
var con = require('./db');
var sms = require('./send_sms');
var writeData = require('./data_handler');

var options = keys.options;
const client = mqtt.connect('mqtt://soldier.cloudmqtt.com', options);

con.connect((err) => {
    if(err) throw err;
    console.log("Database connected");
});

client.on('connect', () => {
    console.log("Connected...");
    client.subscribe('data');
});

client.on('message', (topic, message) => {
    if(message.toString() !== "Data not received"){
        var data = message.toString().split(':-')[1].split(',');

        console.log(data)

        var map = {
            temp: data[0],
            hum: data[1],
            light: data[3],
            soil: data[2]
        }

        //console.log(map.temp);    

        try{
            writeData(map);
            sms.validateSms(map);
        }catch(err){
            console.log(err);
        }
        
    }
});

