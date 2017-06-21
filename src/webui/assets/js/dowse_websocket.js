    // configuration
// host = ;    // hostname or IP address of the dowse box is defined in the calling .html
//    port = 1888;
//    topic = 'dns-query-channel'; // topic to subscribe it should define in the calling html
    useTLS = false;
    username = null;
    password = null;
    cleansession = true;
    // end of conf
    
    var mqtt;
    var reconnectTimeout = 20000;
    
function MQTTconnect() {
    mqtt = new Paho.MQTT.Client(
        host,
        port,
        "web_" + parseInt(Math.random() * 100,
                          10));

    var options = {
        timeout: 3,
        useSSL: useTLS,
        cleanSession: cleansession,
        onSuccess: onConnect,
        onFailure: function (message) {
            $('#status').val("Connection failed: " + message.errorMessage + "Retrying");
            setTimeout(MQTTconnect, reconnectTimeout);
        }
    };

    mqtt.onConnectionLost = onConnectionLost;
    mqtt.onMessageArrived = onMessageFromLogQueue;

    if (username != null) {
        options.userName = username;
        options.password = password;
    }
    console.log("Host="+ host + ", port=" + port + " TLS = " + useTLS +
        " username=" + username + " password=" + password);
    mqtt.connect(options);
}

function onConnect() {
    $('#status').val('Connected to ' + host + ':' + port);
    // Connection succeeded; subscribe to our topic
    mqtt.subscribe(topic, {qos: 0});
    $('#topic').val(topic);
}

function onConnectionLost(responseObject) {
    setTimeout(MQTTconnect, reconnectTimeout);
    $('#status').val("connection lost: " + responseObject.errorMessage + ". Reconnecting");

};

function onMessageArrived(message) {

    var topic = message.destinationName;
    var payload = message.payloadString;

    $('#ws').prepend('<li>' + topic + ' = ' + payload + '</li>');
};

function level_to_bootstrap( internal_level ){
    if (internal_level == "DEBUG") return "info";
    if (internal_level == "ERROR") return "danger";
    if (internal_level == "NOTICE") return "success";
    if (internal_level == "WARN") return "warning";
    if (internal_level == "ACT") return "success";
    return "danger"
}

function onMessageFromLogQueue(message) {

    var topic = message.destinationName;
    var payload = message.payloadString;

//    $('#ws').prepend('<li>' + topic + ' = ' + payload + '</li>');

    var field=payload.split(":");

    var level=level_to_bootstrap(field[1])

    /**/    
    var d = new Date(0);
    d.setUTCMilliseconds( parseInt(field[2]) );

    /**/
    var time=""+(1900+d.getYear())+"/"+(1+d.getMonth())+"/"+d.getDate()+" " +d.getHours()+":"+d.getMinutes()+":"+d.getSeconds()+"."+d.getMilliseconds();

    /**/
    var str='<div class="alert alert-'+level+'">';
    str+="<strong>"+field[1]+"!</strong> Process ("+field[0]+") at : "+time+" : "+atob(field[3]);
    str+="</div>";

    $('#ws').prepend(str);
};

