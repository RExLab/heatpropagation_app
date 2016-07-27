// Setup basic express server
var panel = require('./build/Release/panel.node');
var express = require('express');
var app = express();

var server = require('http').createServer(app);
var io = require('socket.io')(server);
var port = 80;

app.use(express.static(__dirname + '/public'));

server.listen(port, function () {
    console.log('Server listening at port %d', port);

});


var configured = false, authenticated = false;

io.on('connection', function (socket) {

    var password = "";
    var duration = 0, interval = 0;
    var step = 2;


    function SendMessage(socket) {
        var data;
        data = panel.getvalues();
        socket.emit('data received', data);

    }


    socket.on('new connection', function (data) {
        // fazer acesso ao rlms para autenticar
        //password = data.pass;
        console.log('new connection' + data);

        authenticated = true;
        configured = panel.setup();
        if (configured) {
            console.log(panel.run());
        } else {
            panel.exit();
            panel.setup();
            panel.run();
        }

    });


    socket.on('start', function (data) {
        console.log(data);
        if (data.sw > 0) {
            clearInterval(interval);

            panel.intensity(data.power, parseInt(data.duration + 10));

            panel.digital(1, parseInt(data.duration));

            duration = data.duration;
            step = (data.step > 1) ? data.step : 2;
            
            var values = JSON.parse(panel.getvalues());
            console.log(values);
            for (var line of values.thermometers) {
                if(line >= 70){
                    socket.emit('lab sync', { 'iscooling': true });    
                }   
            }

            SendMessage(socket);
            interval = setInterval(function () {
                SendMessage(socket);
            }, step * 1000);

           

        } else {
            panel.digital(0);
            clearInterval(interval);
        }

    });



    socket.on('disconnect', function () {
        clearInterval(interval);
        console.log('disconnected');
        panel.digital(0);
        setTimeout(function () {

            if (configured) {
                configured = authenticated = false;
                panel.exit();
            } else {
                panel.setup();
                panel.run();
                panel.exit();
            }
        }, 1000);
    });


});