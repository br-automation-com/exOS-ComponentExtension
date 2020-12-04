const header = require('../exos_header');
const fs = require('fs');

function generateExosIncludes(template) {
    let out = "";

    out += `#define EXOS_ASSERT_LOG &${template.logname}\n`;

    out += `#include <node_api.h>\n`;
    out += `#include <stdio.h>\n`;
    out += `#include <uv.h>\n`;
    out += `#include <unistd.h>\n`;

    out += `#include "exos_api.h"\n`;
    out += `#include "exos_log.h"\n`;
    out += `#include "${template.headerName}"\n\n`;

    out += `#define SUCCESS(_format_, ...) exos_log_success(&${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
    out += `#define INFO(_format_, ...) exos_log_info(&${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
    out += `#define VERBOSE(_format_, ...) exos_log_debug(&${template.logname}, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);\n`;
    out += `#define ERROR(_format_, ...) exos_log_error(&${template.logname}, _format_, ##__VA_ARGS__);\n`;
    out += `\nexos_log_handle_t ${template.logname};\n\n`;

    return out;
}



function generateNapiModule(fileName, typName) {

    let template = configTemplate(fileName,typName);

}

function configTemplate(fileName, typName) {

    if (!fs.existsSync(fileName)) {
        throw(`file ${fileName} doesnt exist`)
    }

    let template = {
        headerName: "",
        
        artefact: {
            structName: "",
            varName: "",
            dataType: "",
            comment: ""
        },
        values: [],
        logname: ""
    };

    var types = header.parseTypFile(fileName, typName);

    // console.log(JSON.stringify(types));

    template.logname = "logger";
    template.headerName = `exos_${types.attributes.dataType.toLowerCase()}.h`
    
    template.artefact.dataType = types.attributes.dataType;
    template.artefact.structName = types.attributes.dataType;
    //check if toLowerCase is equal to datatype name, then extend it with _artefact
    if (types.attributes.dataType == types.attributes.dataType.toLowerCase()) {
        template.artefact.varName = types.attributes.dataType.toLowerCase() + "_artefact";
    }
    else {
        template.artefact.varName = types.attributes.dataType.toLowerCase();
    }

    //check if toLowerCase is same as struct name, then extend it with _value
    for (let child of types.children) {
        if (child.attributes.name == child.attributes.name.toLowerCase()) {
            template.values.push({
                structName: child.attributes.name,
                varName: child.attributes.name.toLowerCase() + "_value",
                dataType: child.attributes.dataType,
                comment: child.attributes.comment
            });
        }
        else {
            template.values.push({
                structName: child.attributes.name,
                varName: child.attributes.name.toLowerCase(),
                dataType: child.attributes.dataType,
                comment: child.attributes.comment
            });
        }

    }

    return template;
}

if (require.main === module) {
    if (process.argv.length >= 3) {

        let fileName = process.argv[2];
        let structName = process.argv[3];

        try {
            generateNapiModule(fileName, structName);
        } catch (error) {
            process.stderr.write(error);        
        }

    }
    else {
        process.stderr.write("usage: ./exos_napi.js <filename.typ> <structname>\n");
    }
}

module.exports = {
    generateNapiModule
}