const https = require('https');
const keys = require('./keys');

const accountSid = keys.twilio.accountSid;
const authToken = keys.twilio.authToken;

const client = require('twilio')(accountSid, authToken);


var allowSendSMSTemp = true;
var allowSendSMSLight = true;



function sendSms(alertMessage, sensor, value, map) {

    var msg = createMessage(alertMessage, sensor, value, map);

    client.messages
        .create({
            body: msg,
            from: '+17867016193',
            to: '+94775808225'
        })
        .then(message => console.log(message.sid));
}

function validateSms(map){

    if(map.temp >= 35 && allowSendSMSTemp){
        sendSms("High Temperature Alert!", "Temperature", map.temp, map);
        allowSendSMSTemp = false;
    }else if(map.temp < 35){
        allowSendSMSTemp = true;
    }

    if(map.light <= 100 && allowSendSMSLight){
        sendSms("Light detected inside the incubator!", "Light Intensity", map.light, map);
        allowSendSMSLight = false;
    }else if(map.light > 100){
        allowSendSMSLight = true;
    }
}

function createMessage(alertMessage, sensor, value, map){
    var msg = "Alert  -  " + alertMessage + '\n';
    msg += "Temperature  -  " + map.temp + String.fromCharCode(0x00B0) + "C\n";

    return msg;
}

module.exports.validateSms = validateSms;