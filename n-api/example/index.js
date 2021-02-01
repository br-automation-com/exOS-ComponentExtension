const myModuleLib = require('./NodeJS/build/Release/NodeJS');
myModule = myModuleLib.libNodeJS_init();

console.log(myModule);

myModule.connect();

setInterval(() => {
    myModule.process();
    if (myModule.is_connected == true) {
        if ((myModule.start.value == true) && (myModule.reset.value == false)) {
            myModule.countUp.value++;
            myModule.countUp.publish();
            myModule.countDown.value++;
            myModule.countDown.publish();

            console.log(`countUp   -> published: ${myModule.countUp.value}`);
            console.log(`countDown -> published: ${myModule.countDown.value}`);
        }

        if (myModule.reset.value == true) {
            let doReset = false;

            if (myModule.countUp.value != 0) {
                myModule.countUp.value = 0;
                myModule.countUp.publish();
                doReset = true;
            }

            if (myModule.countDown.value != 0) {
                myModule.countDown.value = 0;
                myModule.countDown.publish();
                doReset = true;
            }

            if (doReset == true) {
                console.log(`COUNTERS RESET`);
                console.log(`countUp   : ${myModule.countUp.value}`);
                console.log(`countDown : ${myModule.countDown.value}`);
            }
        }
    }
}, 50);

setInterval(() => {
    console.log(``);
    console.log(`countUp     : ${myModule.countUp.value}`);
    console.log(`countDown   : ${myModule.countDown.value}`);
    console.log(`connected   : ${myModule.is_connected}`);
    console.log(`operational : ${myModule.is_operational}`);
    console.log(`start       : ${myModule.start.value}`);
    console.log(`reset       : ${myModule.reset.value}`);
    console.log(``);
    console.log(`-----------------`);
}, 2000);