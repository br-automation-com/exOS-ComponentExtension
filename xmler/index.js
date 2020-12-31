//const header = require('../exos_header');

/*
<struct name="<NAME>" dataType="MyApplication" comment="" >

    <variable name="Enable" dataType="BOOL" comment="SUB" />
    <variable name="Counter" dataType="INT" comment="PUB" />
    <variable name="Temperature" dataType="REAL" comment="PUB" />
    <variable name="Buffer" dataType="USINT" comment="PUB" arraySize=1000 />

    <struct name="Config" dataType="myApplicationConfig" comment="PUB SUB">
        <variable name="MaxPressure" dataType="LREAL" comment="" />
        <variable name="SleepTime" dataType="LREAL" comment="" />
    </struct>

    <struct name="Status" dataType="myApplicationStatus" comment="PUB">
        <variable name="Result" dataType="UINT" comment="" />
        <variable name="ActualPressure" dataType="LREAL" comment="" />
        <variable name="Message" dataType="STRING" stringLength=81 comment="" />
    </struct>

</struct>


*/
const parse = require('xml-parser');
const fs = require('fs');

function createXml(fileName)
{
    //let jsonTyp = header.parseTypFile(fileName, typName);

    let xmlTyp = parse(fs.readFileSync(fileName).toString());
    console.log(JSON.stringify(xmlTyp.root,null,4));
}


module.exports = {
    createXml
}