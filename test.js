// var profiler = require('gc-profiler');
// profiler.on('gc', function (info) {
//     console.log(info);
// });

const test = require("./lib/player");
const player = new test.VlcPlayer(["rtmp://58.200.131.2:1935/livetv/hbtv"]);

player.onEmit = player.emit.bind(player);
//
player.onLogMessage = function(jsLevel, message, format){
    console.log("onMessage: ", jsLevel, message);
};

player.on("test", function(data){
    console.log("Test.........", data);
});

//player.play("http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4");
player.play("rtmp://58.200.131.2:1935/livetv/hbtv");

console.log( player );
