const process = require('process');
let SimPanel = require('./simpanel');

let interval = (undefined != process.argv.find(element => element == "-i"));

SimPanel.DataModel.Encoder.onChange(() => {
    console.log(`Encoder changed to: ${SimPanel.DataModel.Encoder.value}`);
});

SimPanel.DataModel.Knobs.onChange(() => {
    console.log(`Knobs changed to: P1=${SimPanel.DataModel.Knobs.value.P1} P2=${SimPanel.DataModel.Knobs.value.P2}`);
    if (SimPanel.DataModel.Knobs.value.P1 > 20 || SimPanel.DataModel.Knobs.value.P2 > 20) {
        SimPanel.DataModel.Knobs.value.P1 = 0;
        SimPanel.DataModel.Knobs.value.P2 = 0;
        SimPanel.DataModel.Knobs.publish();
        console.log("published Knobs - both reset to 0");
    }
});

SimPanel.connectionOnChange(() => {
    console.log("SimPanel changed state to: " + SimPanel.connectionState);
});
SimPanel.DataModel.Display.connectionOnChange(() => {
    console.log("SimPanel.DataModel.Display changed state to: " + SimPanel.DataModel.Display.connectionState);
});
SimPanel.DataModel.Encoder.connectionOnChange(() => {
    console.log("SimPanel.DataModel.Encoder changed state to: " + SimPanel.DataModel.Encoder.connectionState);
});
SimPanel.DataModel.Knobs.connectionOnChange(() => {
    console.log("SimPanel.DataModel.Knobs changed state to: " + SimPanel.DataModel.Knobs.connectionState);
});

if (interval) {
    let i = 1;
    setInterval(() => {
        i = (i + 2) % 10000;
        SimPanel.DataModel.Display.value = ++i;
        console.log(`Published ${SimPanel.DataModel.Display.value}`);
        SimPanel.DataModel.Display.publish();
        console.log(SimPanel);
    }, 10000);
}
