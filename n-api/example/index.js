var SimPanel = require('./simpanel')
console.log(SimPanel);
SimPanel.Display.OnChange(function (val) {
    //console.log(`hey I got called ${val}`);
});
var i = 1;
setInterval(
    function () {
        i = (i + 2) % 1000;
        SimPanel.Display.Publish(i++);
    }
    , 2);