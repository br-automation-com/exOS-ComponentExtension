const process = require('process');
let MyApp = require('./MyApp');

//connection callbacks
MyApp.connectionOnChange(() => {
    console.log("MyApp changed state to: " + MyApp.connectionState);
});
MyApp.dataModel.execute.connectionOnChange(() => {
    console.log("MyApp.dataModel.execute changed state to: " + MyApp.dataModel.execute.connectionState);
});
MyApp.dataModel.done.connectionOnChange(() => {
    console.log("MyApp.dataModel.done changed state to: " + MyApp.dataModel.done.connectionState);
});
MyApp.dataModel.message.connectionOnChange(() => {
    console.log("MyApp.dataModel.message changed state to: " + MyApp.dataModel.message.connectionState);
});
MyApp.dataModel.parameters.connectionOnChange(() => {
    console.log("MyApp.dataModel.parameters changed state to: " + MyApp.dataModel.parameters.connectionState);
});
MyApp.dataModel.results.connectionOnChange(() => {
    console.log("MyApp.dataModel.results changed state to: " + MyApp.dataModel.results.connectionState);
});

//value callbacks
MyApp.dataModel.execute.onChange(() => {
    console.log(`execute changed to: ${MyApp.dataModel.execute.value}`);

    if (MyApp.dataModel.execute.value) {
        console.log(`calculating results...`);

        for (let i = 0; i < MyApp.dataModel.parameters.value.length; i++) {
            MyApp.dataModel.results.value[i].product = MyApp.dataModel.parameters.value[i].x * MyApp.dataModel.parameters.value[i].y;
            console.log(`calculating results...[${i}].product = ${MyApp.dataModel.results.value[i].product}`);
        }

        console.log(`publishing results...`);
        MyApp.dataModel.results.publish();
        MyApp.dataModel.done.value = true;
        MyApp.dataModel.done.publish();
    }
});

MyApp.dataModel.done.onChange(() => {
    console.log(`done changed to: ${MyApp.dataModel.done.value}`);
});

MyApp.dataModel.message.onChange(() => {
    console.log(`message changed to: ${MyApp.dataModel.message.value}`);
});

MyApp.dataModel.parameters.onChange(() => {
    console.log(`parameters changed to: ` + JSON.stringify(MyApp.dataModel.parameters.value));
});

//cyclic/application stuff
setInterval(() => {
    MyApp.dataModel.message.value = "123456789009876543211234567890";
    MyApp.dataModel.message.publish();
}, 30000);
