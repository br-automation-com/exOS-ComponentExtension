#!/usr/bin/env node

const header = require('../exos_header');
const fs = require('fs');

function generateWSLBuild(typName) {
    let out = "";

    out += `[CmdletBinding()]\n`;
    out += `param (\n`;
    out += `    [Parameter()][switch]$Rebuild = $false,\n`;
    out += `    [Parameter()][switch]$Pack = $false\n`;
    out += `)\n`;
    out += `\n`;
    out += `$wsli = New-Object System.Diagnostics.ProcessStartInfo\n`;
    out += `$wsli.FileName = "wsl.exe"\n`;
    out += `$wsli.UseShellExecute = $false \n`;
    out += `$wsli.RedirectStandardInput = $true\n`;
    out += `$wsli.RedirectStandardOutput = $true\n`;
    out += `$wsli.Arguments = "-d Debian"\n`;
    out += `$wsli.WorkingDirectory = "$PSScriptRoot"\n`;
    out += `\n`;
    out += `$wsl = New-Object System.Diagnostics.Process\n`;
    out += `$wsl.StartInfo = $wsli\n`;
    out += `\n`;
    out += `Register-ObjectEvent -InputObject $wsl -EventName OutputDataReceived -action {\n`;
    out += `    $output = $Event.SourceEventArgs.Data\n`;
    out += '    Write-Host "WSL: $output`r"\n';
    out += `} | Out-Null\n`;
    out += `Write-Host "Starting WSL.."\n`;
    out += `$wsl.Start()\n`;
    out += `\n`;
    out += `$wsl.BeginOutputReadLine()\n`;
    out += `\n`;
    out += `Remove-Item "$PSScriptRoot\\build\\*" -Recurse\n`;
    out += '$wsl.StandardInput.Write("cd build`n");\n';
    out += '$wsl.StandardInput.Write("cmake ..`n");\n';
    out += '$wsl.StandardInput.Write("make`n");\n';
    out += '$wsl.StandardInput.Write("cpack`n");\n';
    out += '$wsl.StandardInput.Write("echo done`n");\n';
    out += '$wsl.StandardInput.Write("exit`n");\n';
    out += `\n`;
    out += `$wsl.WaitForExit()\n`;
    out += `\n`;
    out += `Copy-Item -Path "$PSScriptRoot\\build\\exar-${typName.toLowerCase()}-*.deb" -Destination "$PSScriptRoot\\..\\..\\..\\"\n`;
    out += `Remove-Item "$PSScriptRoot\\build\\*" -Recurse\n`;
    out += `\n`;

    return out;
}

function generateExosPkg(typName,libName,fileName) {
    let out = "";

    out += `<?xml version="1.0" encoding="utf-8"?>\n`;
    out += `<ArtefactPackage ErrorHandling="Ignore" StartupTimeout="0">\n`;
    out += `    <File Name="exar-${typName.toLowerCase()}" FileName="exar-${typName.toLowerCase()}-1.0.0.deb" Type="Project"/>\n`;
    out += `    <Service Name="${typName} Runtime Service" Executable="/home/user/${typName.toLowerCase()}" Arguments=""/>\n`;
    out += `    <Interface Name="${typName}"/>\n`;
    out += `    <GenerateHeader FileName="${libName}\\${fileName}" TypeName="${typName}" OutputPath="${libName}"/>\n`;
    out += `</ArtefactPackage>\n`;
    out += ``;

    return out;
}

function generateTerminationHeader() {
    let out = "";

    out += `#ifndef _TERMINATION_H_\n`;
    out += `#define _TERMINATION_H_\n`;
    out += `\n`;
    out += `#include <stdbool.h>\n`;
    out += `\n`;
    out += `void catch_termination();\n`;
    out += `bool is_terminated();\n`;
    out += `\n`;
    out += `#endif//_TERMINATION_H_\n`;

    return out;
}

function generateTermination() {
    let out = "";

    out += `#include "termination.h"\n`;
    out += `#include <signal.h>\n`;
    out += `#include <stdlib.h>\n`;
    out += `\n`;
    out += `static bool terminate_process = false;\n`;
    out += `\n`;
    out += `bool is_terminated()\n`;
    out += `{\n`;
    out += `    return terminate_process;\n`;
    out += `}\n`;
    out += `\n`;
    out += `static void handle_term_signal(int signum)\n`;
    out += `{\n`;
    out += `    switch (signum)\n`;
    out += `    {\n`;
    out += `    case SIGINT:\n`;
    out += `    case SIGTERM:\n`;
    out += `    case SIGQUIT:\n`;
    out += `        terminate_process = true;\n`;
    out += `        break;\n`;
    out += `\n`;
    out += `    default:\n`;
    out += `        break;\n`;
    out += `    }\n`;
    out += `}\n`;
    out += `\n`;
    out += `void catch_termination()\n`;
    out += `{\n`;
    out += `    struct sigaction new_action;\n`;
    out += `\n`;
    out += `    // Register termination handler for signals with termination semantics\n`;
    out += `    new_action.sa_handler = handle_term_signal;\n`;
    out += `    sigemptyset(&new_action.sa_mask);\n`;
    out += `    new_action.sa_flags = 0;\n`;
    out += `\n`;
    out += `    // Sent via CTRL-C.\n`;
    out += `    sigaction(SIGINT, &new_action, NULL);\n`;
    out += `\n`;
    out += `    // Generic signal used to cause program termination.\n`;
    out += `    sigaction(SIGTERM, &new_action, NULL);\n`;
    out += `\n`;
    out += `    // Terminate because of abnormal condition.\n`;
    out += `    sigaction(SIGQUIT, &new_action, NULL);\n`;
    out += `}\n`;

    return out;
}

function generateCMakeLists(typName) {
    let out = "";

    out += `cmake_minimum_required(VERSION 3.0)\n`;
    out += `\n`;
    out += `project(${typName.toLowerCase()} C)\n`;
    out += `\n`;
    out += `add_executable(${typName.toLowerCase()} ${typName.toLowerCase()}.c termination.c)\n`;
    out += `target_include_directories(${typName.toLowerCase()} PUBLIC ..)\n`;
    out += `target_link_libraries(${typName.toLowerCase()} zmq exos-api)\n`;
    out += `\n`;
    out += `install(TARGETS ${typName.toLowerCase()} RUNTIME DESTINATION /home/user)\n`;
    out += `\n`;
    out += `set(CPACK_GENERATOR "DEB")\n`;
    out += `set(CPACK_PACKAGE_NAME exar-${typName.toLowerCase()})\n`;
    out += `set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${typName.toLowerCase()} summary")\n`;
    out += `set(CPACK_PACKAGE_DESCRIPTION "Some description")\n`;
    out += `set(CPACK_PACKAGE_VENDOR "Your Organization")\n`;
    out += `\n`;
    out += `set(CPACK_PACKAGE_VERSION_MAJOR 1)\n`;
    out += `set(CPACK_PACKAGE_VERSION_MINOR 0)\n`;
    out += `set(CPACK_PACKAGE_VERSION_PATCH 0)\n`;

    out += `set(CPACK_PACKAGE_FILE_NAME exar-${typName.toLowerCase()}-`;
    out += '${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})\n';
    
    out += `set(CPACK_DEBIAN_PACKAGE_MAINTAINER "your name")\n`;
    out += `\n`;
    out += `set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)\n`;
    out += `\n`;
    out += `include(CPack)\n`;
    out += `\n`;

    return out;
}

function generateExosIncludes(template) {
    let out = "";

    out += `#define EXOS_ASSERT_LOG &${template.logname}\n`;
    out += `#include "exos_log.h"\n`;
    out += `#include "exos_${template.artefact.dataType.toLowerCase()}.h"\n\n`;

    out += `#define SUCCESS(_format_, ...) exos_log_success(&${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
    out += `#define INFO(_format_, ...) exos_log_info(&${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
    out += `#define VERBOSE(_format_, ...) exos_log_debug(&${template.logname}, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);\n`;
    out += `#define ERROR(_format_, ...) exos_log_error(&${template.logname}, _format_, ##__VA_ARGS__);\n`;
    out += `\nexos_log_handle_t ${template.logname};\n\n`;

    return out;
}

function generateExosCallbacks(template) {
    let out = "";

    out += `static void valueChanged(exos_value_handle_t *value)\n{\n`;
    out += `    VERBOSE("value %s changed!", value->name);\n`;
    out += `    //handle each subscription value separately\n`;
    var atleastone = false;
    for (let value of template.values) {
        // initialize non-string comments to "" to avoid crashes in the next if...
        if (typeof value.comment !== 'string') {
            value.comment = "";
        }

        if (value.comment.includes("SUB")) {
            if (atleastone) {
                out += `    else `;
            }
            else {
                out += `    `;
                atleastone = true;
            }
            out += `if(0 == strcmp(value->name,"${value.structName}"))\n`;
            out += `    {\n`;
            out += `        ${header.convertPlcType(value.dataType)} *${value.varName} = (${header.convertPlcType(value.dataType)} *)value->data;\n`;
            out += `    }\n`;
        }
    }
    out += `}\n\n`;

    out += `static void valuePublished(exos_value_handle_t *value, uint32_t queue_items)\n{\n`;
    out += `    VERBOSE("value %s published! queue size:%i", value->name, queue_items);\n`;
    out += `    //handle each published value separately\n`;
    atleastone = false;
    for (let value of template.values) {
        if (value.comment.includes("PUB")) {
            if (atleastone) {
                out += `    else `;
            }
            else {
                out += `    `;
                atleastone = true;
            }
            out += `if(0 == strcmp(value->name,"${value.structName}"))\n`;
            out += `    {\n`;
            out += `        ${header.convertPlcType(value.dataType)} *${value.varName} = (${header.convertPlcType(value.dataType)} *)value->data;\n`;
            out += `    }\n`;
        }
    }
    out += `}\n\n`;

    out += `static void valueConnectionChanged(exos_value_handle_t *value)\n{\n`;
    out += `    INFO("value %s changed state to %s", value->name, exos_state_string(value->connection_state));\n\n`;
    out += `    switch (value->connection_state)\n`;
    out += `    {\n`;
    out += `    case EXOS_STATE_DISCONNECTED:\n`;
    out += `        break;\n`;
    out += `    case EXOS_STATE_CONNECTED:\n`;
    out += `        //call the value changed event to update the value\n`;
    out += `        //valueChanged(value);\n`;
    out += `        break;\n`;
    out += `    case EXOS_STATE_OPERATIONAL:\n`;
    out += `        break;\n`;
    out += `    case EXOS_STATE_ABORTED:\n`;
    out += `        ERROR("value %s error %d (%s) occured", value->name, value->error, exos_error_string(value->error));\n`;
    out += `        break;\n`;
    out += `    }\n`;

    out += `}\n\n`;

    out += `static void connectionChanged(exos_artefact_handle_t *artefact)\n{\n`;
    out += `    INFO("application changed state to %s", exos_state_string(artefact->connection_state));\n\n`;
    out += `    switch (artefact->connection_state)\n`;
    out += `    {\n`;
    out += `    case EXOS_STATE_DISCONNECTED:\n`;
    out += `        break;\n`;
    out += `    case EXOS_STATE_CONNECTED:\n`;
    out += `        break;\n`;
    out += `    case EXOS_STATE_OPERATIONAL:\n`;
    out += `        SUCCESS("${template.artefact.structName} operational!");\n`
    out += `        break;\n`;
    out += `    case EXOS_STATE_ABORTED:\n`;
    out += `        ERROR("application error %d (%s) occured", artefact->error, exos_error_string(artefact->error));\n`;
    out += `        break;\n`;
    out += `    }\n`;
    out += `}\n\n`;

    return out;
}

function generateExosInit(template) {
    var out = "";

    out += `    ${template.artefact.structName} data;\n\n`;
    out += `    exos_artefact_handle_t ${template.artefact.varName};\n\n`;
    for (let value of template.values) {
        if (value.comment.includes("PUB") || value.comment.includes("SUB")) {
            out += `    exos_value_handle_t ${value.varName};\n`;
        }
    }
    out += `    \n`;
    out += `    exos_log_init(&${template.logname}, "${template.artefact.structName}");\n\n`;
    out += `    SUCCESS("starting ${template.artefact.structName} application..");\n\n`;

    //initialization
    out += `    EXOS_ASSERT_OK(exos_artefact_init(&${template.artefact.varName}, "${template.artefact.structName}"));\n\n`;
    out += `    //set the user_context to access custom data in the callbacks\n`;
    out += `    ${template.artefact.varName}.user_context = NULL; //user defined\n`;
    out += `    ${template.artefact.varName}.user_tag = 0; //user defined\n\n`;

    for (let value of template.values) {
        if (value.comment.includes("PUB") || value.comment.includes("SUB")) {
            out += `    EXOS_ASSERT_OK(exos_value_init(&${value.varName}, &${template.artefact.varName}, "${value.structName}", &data.${value.structName}, sizeof(data.${value.structName})));\n`;
            out += `    ${value.varName}.user_context = NULL; //user defined\n`;
            out += `    ${value.varName}.user_tag = 0; //user defined\n\n`;
        }
    }
    out += `    //register the artefact\n`;
    out += `    EXOS_ASSERT_OK(exos_artefact_register_${template.artefact.structName.toLowerCase()}(&${template.artefact.varName}, connectionChanged));\n`;
    out += `    \n`;

    out += `    //register values\n`;
    for (let value of template.values) {
        if (value.comment.includes("PUB")) {
            out += `    EXOS_ASSERT_OK(exos_value_register_publisher(&${value.varName}, valueConnectionChanged, valuePublished));\n`;
        }
    }
    for (let value of template.values) {
        if (value.comment.includes("SUB")) {
            if (value.comment.includes("PUB")) {
                out += `    EXOS_ASSERT_OK(exos_value_register_subscription(&${value.varName}, NULL, valueChanged));\n`;
            }
            else {
                out += `    EXOS_ASSERT_OK(exos_value_register_subscription(&${value.varName}, valueConnectionChanged, valueChanged));\n`;
            }
        }
    }
    out += `    \n`;

    return out;
}

function generateExosCyclic(template) {
    var out = "";
    out += `        EXOS_ASSERT_OK(exos_artefact_cyclic(&${template.artefact.varName}));\n`;
    out += `        exos_log_cyclic(&${template.logname});\n\n`;
    out += `        //put your cyclic code here!\n\n`;

    return out;
}

function generateExosExit(template) {
    var out = "";

    out += `    //first unregister the values, then the artefact\n`;
    for (let value of template.values) {
        if (value.comment.includes("PUB")) {
            out += `    EXOS_ASSERT_OK(exos_value_unregister_publisher(&${value.varName}));\n`;
        }
    }
    for (let value of template.values) {
        if (value.comment.includes("SUB")) {
            out += `    EXOS_ASSERT_OK(exos_value_unregister_subscription(&${value.varName}));\n`;
        }
    }
    out += `\n`;
    out += `    EXOS_ASSERT_OK(exos_artefact_unregister(&${template.artefact.varName}));\n\n`;

    out += `    //first delete the values, then the artefact\n`;
    for (let value of template.values) {
        if (value.comment.includes("PUB") || value.comment.includes("SUB")) {
            out += `    EXOS_ASSERT_OK(exos_value_delete(&${value.varName}));\n`;
        }
    }

    out += `    \n`;
    out += `    EXOS_ASSERT_OK(exos_artefact_delete(&${template.artefact.varName}));\n\n`;

    out += `    //finish with deleting the log\n`;
    out += `    exos_log_delete(&${template.logname});\n`;

    return out;
}

function generateTemplate(fileName, typName) {
    let out = "";

    let template = configTemplate(fileName, typName);

    //includes

    out += `#include <unistd.h>\n`;
    out += `#include <string.h>\n`;
    out += `#include "termination.h"\n\n`;
    out += generateExosIncludes(template);

    out += generateExosCallbacks(template);

    //declarations
    out += `int main()\n{\n`

    out += generateExosInit(template);

    out += `    catch_termination();\n`;

    //main loop
    out += `    while (true)\n    {\n`;

    out += generateExosCyclic(template);
    out += `        if (is_terminated())\n`;
    out += `        {\n`;
    out += `            SUCCESS("${template.artefact.structName} application terminated, closing..");\n`;
    out += `            break;\n`;
    out += `        }\n`;
    out += `    }\n\n`;

    //unregister

    out += generateExosExit(template);

    out += `    return 0;\n`
    out += `}\n`

    return out;
}

function configTemplate(fileName, typName) {
    var template = {
        headerName: "",
        artefact: {
            structName: "",
            varName: "",
            dataType: "",
            comment: ""
        },
        values: [],
        logname: ""
    }

    if (fs.existsSync(fileName)) {

        var types = header.parseTypFile(fileName, typName);

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


    } else {
        throw(`file '${fileName}' not found.`);
    }

    return template;
}

if (require.main === module) {
    if (process.argv.length > 3) {
        let outPath = process.argv[4];
        if (outPath == "" || outPath == undefined) {
            outPath = ".";
        }
        let fileName = process.argv[2];
        let structName = process.argv[3];

        try {
            let out = generateTemplate(fileName, structName);
            fs.writeFileSync(`${outPath}/exos_template_${structName.toLowerCase()}_linux.c`, out);
            process.stdout.write(`${outPath}/exos_template_${structName.toLowerCase()}_linux.c generated`);    
        } catch (error) {
            process.stderr.write(error);
        }
    }
    else {
        process.stderr.write(" - usage: ./exos_template_linux.js <filename.typ> <structname> <template output folder>\n");
    }
}

module.exports = {
    generateExosPkg,
    generateTemplate,
    generateTerminationHeader,
    generateTermination,
    generateCMakeLists,
    generateWSLBuild
}
