const process = require('process');
let SimPanel = require('./simpanel');

let interval = (undefined != process.argv.find(element => element == "-i"));

console.log(SimPanel);

SimPanel.Encoder.onChange(function (value) {
    SimPanel.Encoder.value = value;
    console.log(`Hey I got called ${SimPanel.Encoder.value}`);
    console.log(SimPanel);
});

if (interval) {
    let i = 1;
    setInterval(
        function () {
            i = (i + 2) % 1000;
            SimPanel.Display.value = ++i;
            SimPanel.Display.publish(SimPanel.Display.value);
            console.log(`Published ${SimPanel.Display.value}`);
            console.log(SimPanel);
        }
        , 10000);
}