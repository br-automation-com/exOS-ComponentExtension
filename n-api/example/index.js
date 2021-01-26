const process = require('process');
let SimPanel = require('./simpanel');

let interval = (undefined != process.argv.find(element => element == "-i"));

SimPanel.DataModel.Encoder.onChange(() => {
    console.log(`Hey I got called ${SimPanel.DataModel.Encoder.value}`);
    console.log(SimPanel);
});

SimPanel.connectionOnChange(() => {
    console.log("SimPanel changed state to: " + SimPanel.connectionState);
});
SimPanel.DataModel.Display.connectionOnChange(() => {
    console.log("SimPanel.DataModel.Display changed state to: " + SimPanel.connectionState);
});
SimPanel.DataModel.Encoder.connectionOnChange(() => {
    console.log("SimPanel.DataModel.Encoder changed state to: " + SimPanel.connectionState);
});

if (interval) {
    let i = 1;
    setInterval(() => {
        i = (i + 2) % 1000;
        SimPanel.DataModel.Display.value = ++i;
        let text = JSON.stringify(SimPanel.DataModel.Display.value, null, 4);
        console.log(`Published ${SimPanel.DataModel.Display.value}`);
        SimPanel.DataModel.Display.publish();
        console.log(SimPanel);
    }, 10000);
}