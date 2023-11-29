/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { Template, ApplicationTemplate } = require('../template')
const { Datamodel, GeneratedFileObj } = require('../../../datamodel');
const { isatty } = require('tty');

let infoString = [];

class TemplateLinuxJulia extends Template {

    /**
     * main sourcefile for the application
     * @type {GeneratedFileObj}
     */
    mainSource;
    mainSourceCopied;

    /**
     * {@linkcode TemplateLinuxJulia} Generates code for a Linux Julia
     *
     * - {@linkcode mainSource} main sourcefile for the application
     *
     * @param {Datamodel} datamodel
     */

    constructor(datamodel) {
        super(datamodel, true);
        if (datamodel == undefined) {
            this.mainSource = {name:`main.jl`, contents:this._generateSourceNoDatamodel(false), description:"Linux application"};
        }
        else {
            this.mainSource = {name:`${this.datamodel.typeName.toLowerCase()}.jl`, contents:this._generateJulia(), description:"Linux application"};
            this.mainSourceCopied = {name:`${this.datamodel.typeName.toLowerCase()}.jlt`, contents:this._generateJulia(), description:"Linux application"};

        }
    }



    /**
     * It create the first part of the Julia file. Here you import and use any desire module also it creates a string that contains the different structures names
     * @param {ApplicationTemplate} template
     * @returns {string}
     */
    generateInit(template) {
        let out = "";
        out += `include("/home/user/${this.datamodel.typeName.toLowerCase()}/EXOS/src/EXOS.jl")\n\n`;
        out += `using .EXOS\n`;
        out += `using Match\n\n`;
        out += `exos_set_path("/usr/lib/libexos-api.so")\n\n`;

        out += `const config_${this.datamodel.typeName.toLowerCase()} = ${this.JSONString()}\n\n`;
        return out;
    }

    JSONString(){
        let out = "";
        out += `${this.datamodel.sourceFile.contents}\n`;
        out = out.split("=")[1].trim();
        out = out.split(";")[0].trim();
        return out
    }

    /**
     * @returns `{main}.jl`: main sourcefile for the application when creating without datamodel
     */
    _generateSourceNoDatamodel() {
        let out = "    ";
        out += `#include <stdio.h>\n`;

        //declarations
        out += `int main()\n{\n`
        out += `    catch_termination();\n\n`;

        //main loop
        out += `    while (true)\n    {\n`;
        out += `        //put your cyclic code here!\n\n`;
        out += `        if (is_terminated())\n`;
        out += `        {\n`;
        out += `            printf("Application terminated, closing..");\n`;
        out += `            break;\n`;
        out += `        }\n`;
        out += `    }\n\n`;

        out += `    return 0;\n`
        out += `}\n`

        return out;
    }
    /**
     * @returns {string} datatype (BOOL, UDINT) as stdint.h/stdbool.h datatype (bool, uint32_t) or struct/enum (unchanged)
     * @param {string} type IEC type, like BOOL or UDINT
     */

    convertPlcType(type) {
        switch (type) {
            case "BOOL": return "Cuchar";
            case "USINT": return "Cuchar";
            case "SINT": return "Cchar";
            case "UINT": return "Cushort";
            case "INT": return "Cshort";
            case "UDINT": return "Cuint";
            case "DINT": return "Cint";
            case "REAL": return "Cfloat";
            case "LREAL": return "Cdouble";
            case "BYTE": return "Cuchar";
            case "STRING": return "Cchar";
            default: //returning the type makes the function valid even if you insert a struct or enum
                return type;
        }
    }
    _makeDataTypes(swig) {

        let fileLines = this.datamodel.fileLines;
        let sortedStructs = this.datamodel.sortedStructs;
        let index = 0;

        /**return the code for a datatype member with the given properties */
        function _outputMember(type, name, arrays, comment) {
            let out = "";
            let isString = (type == "String") ? true : false 
            let isArray = (arrays[0] > 0) ? true : false
            let value = ""
            if(isString == 0){
                value = isArray ? `    ${name}::NTuple{${arrays[0]}, ${type}}` : `    ${name}::${type}`;
            }else {
                value = isArray ? `    ${name}::NTuple{${arrays[0]}, NTuple{${arrays[1]}, Cchar}}` : `    ${name}::NTuple{${arrays[1]}, Cchar}`;
                let newString = [index, [arrays[0], arrays[1]]]
                infoString.push(newString);
            }
                out += `${value}`;
            
            if (comment != "") out += ` #${comment}`;
            index = index + 1;
            out += `\r\n`;
            return out;
        }

        /**search the fileLines to see if name is a structure, returns true if name is a structure*/
        function _isStructType(fileLines, name) {
            for (let line of fileLines) {
                if (line.includes("STRUCT") && line.includes(":")) {
                    if (name == line.split(":")[0].trim()) return true;
                }
            }
            return false;
        }

        /** return the string between two delimiters (start,end) of a given line. return null if one of the delimiters isn't found*/
        function _takeout(line, start, end) {
            if (line.includes(start) && line.includes(end)) {
                return line.split(start)[1].split(end)[0];
            }
            else return null;
        }

        let out = ``;
        let structname = "";
        let members = 0;
        let cmd = "find_struct_enum";
        let structs = [];

        for (let line of fileLines) {
            let comment = ""
            if (line.includes("(*")) {
                comment = _takeout(line, "(*", "*)");
            }

            switch (cmd) {
                case "find_struct_enum":
                    //analyze row check for struct, enum and directly derived types

                    line = line.split("(*")[0];
                    line = line.split(":");
                    for (let i = 0; i < line.length; i++) line[i] = line[i].trim();

                    if (line[1] == ("STRUCT")) {
                        cmd = "read_struct";
                        if (comment != "") out += "#" + comment + "\r\n";
                        structname = line[0];
                        out += `struct ${structname}\r\n`;
                        structs.push({ name: structname, out: "", depends: [] });
                    }
                    else if (line[1] == ("")) {
                        cmd = "read_enum";
                        if (comment != "") out += "#" + comment + "\r\n";
                        structname = line[0];
                        out += `@enum ${structname} begin\r\n`;
                        members = 0;
                        structs.push({ name: structname, out: "", depends: [] });
                    }
                    //"else" line[1] is not "" (enum) and not "STRUCT" then it have to be a derived type = do nothing
                    break;

                case "read_enum":
                     if(comment != "") {
                        if (line.includes(",")) {
                            if (line.includes(":=")) {
                                let name = line.split(":=")[0].trim();
                                let enumValue = line.split(":=")[1].trim();
                                enumValue = parseInt(enumValue.split(",")[0].trim());
                                out += `    ${name.toUpperCase()} = ${enumValue} # ${comment}\r\n`;
                            }
                            else {
                                let name = line.split(",")[0].trim();
                                out += `    ${name.toUpperCase()} # ${comment}\r\n`;
                            }
                        } else {
                            if (line.includes(":=")) {
                                let name = line.split(":=")[0].trim();
                                let enumValue = line.split(":=")[1].trim();
                                enumValue = parseInt(enumValue.split("(*")[0].trim());
                                out += `    ${name.toUpperCase()} = ${enumValue} # ${comment}\r\n`;
                            }
                            else {
                                let name = line.split(" (*")[0].trim();
                                out += `    ${name.toUpperCase()} # ${comment}\r\n`;
                            }
                        }
                        members++;
                    }
                    else if (line.includes(")")) {
                        cmd = "find_struct_enum";
                        if (members > 0) {
                            out = out.slice(0, -2); //remove the last ,\r\n STATE
                            out += `\r\n`;
                        }
                        out += `end\r\n\r\n`;
                        structs[structs.length - 1].out = out;
                        out = "";
                    }
                    else if (!line.includes("(")) {
                        if (line.includes(":=")) {
                            let name = line.split(":=")[0].trim();
                            let enumValue = line.split(":=")[1].trim();
                            enumValue = parseInt(enumValue.split(",")[0].trim());
                            out += `    ${name.toUpperCase()} = ${enumValue}\r\n`;
                        }
                        else {
                            let name = line.split(",")[0].trim();
                            out += `    ${name.toUpperCase()}\r\n`;
                        }
                        members++;
                    }
                    break;

                case "read_struct":
                    if (line.includes("END_STRUCT")) {
                        cmd = "find_struct_enum";
                        out += `end\r\n\r\n`;
                        structs[structs.length - 1].out = out;
                        out = "";
                    }
                    else {
                        let arraySize = 0;
                        let stringSize = 0;
                        if (line.includes("ARRAY")) {
                            let range = _takeout(line, "[", "]")
                            if (range != null) {
                                let from = parseInt(range.split("..")[0].trim());
                                let to = parseInt(range.split("..")[1].trim());
                                arraySize = to - from + 1;
                            }
                        }
                        if (line.includes(":")) {
                            let name = line.split(":")[0].trim();
                            let type = "";
                            if (arraySize > 0) {
                                type = line.split(":")[1].split("OF")[1].trim();
                            }
                            else {
                                type = line.split(":")[1].trim();
                            }
                            //let comment = "";
                            if (type.includes("(*")) {
                                //comment = _takeout(type, "(*", "*)");
                                type = type.split("(*")[0].trim();
                            }

                            if (arraySize > 0 && swig !== undefined && swig) {
                                out += `    # array not exposed directly:`
                            }

                            let typeForSwig = "";
                            let dataset = {dataType: type, type: "notenum"};
                            if (type.includes("STRING")) {
                                let length = _takeout(type, "[", "]");
                                if (length != null) {
                                    typeForSwig = "char";
                                    stringSize = parseInt(length) + 1;
                                    out += _outputMember("String", name, [arraySize, stringSize], comment);
                                }
                            }
                            else if (Datamodel.isScalarType(dataset)) {
                                let stdtype = this.convertPlcType(type);
                                typeForSwig = stdtype;
                                out += _outputMember(stdtype, name, [arraySize], comment);
                            }
                            else {
                                structs[structs.length - 1].depends.push(type); // push before adding "struct "
                                typeForSwig = type;
                                if (_isStructType(fileLines, type)) {
                                    typeForSwig = type;
                                    type = "" + type;
                                }
                                out += _outputMember(type, name, [arraySize], comment);
                            }

                            if (arraySize > 0 && swig !== undefined && swig) {
                                // add sai=swig array info
                                out += `<sai>{"structname": "${structname}", "membername": "${name}", "datatype": "${typeForSwig}", "arraysize": "${arraySize}", "stringsize": "${stringSize}"}</sai>`
                            }
                        }
                    }
                    break;
            }
        }

        //output the sorted structures
        out = "";

        for(let i=0; i<sortedStructs.length; i++) {
            for (let struct of structs) {
                if(sortedStructs[i].name ==struct.name) {
                    sortedStructs[i].dependencies = struct.depends;
                    // find and extract all swig array info stuff and add them last to be able to replace it correctly in swig template generator
                    if (swig !== undefined && swig) {
                        // do not include the last one (top-level struct) as it already exists as struct lib<typname>
                        if(i < sortedStructs.length-1) {
                            let swigInfo = ""
                            let matches = struct.out.matchAll(/<sai>(.*)<\/sai>/g);
                            let swigInfoResult = Array.from(matches, x => x[1]);
                            if (swigInfoResult.length > 0)
                                swigInfo = `<sai>{"swiginfo": [` + swigInfoResult.join(",") + `]}</sai>`;
                            out += struct.out.replace(/<sai>.*<\/sai>/g, "") + swigInfo;
                        }
                    }
                    else {
                        out += struct.out;
                    }
                }
            }
        }

        return out;
        }

   /**
     * Creates the script for the callbacks
     */
    _generateCallbacks(template) {
        let out = "";

        out += `function exos_dataset_event_cb(dataset::ExosDatasetHandle,\n`;
        out += `                               event_type::EXOS_DATASET_EVENT_TYPE,\n`;
        out += `                               info::Ptr{Cvoid})\n`;
        out += `    @match event_type begin\n`;
        out += `        EXOS_DATASET_EVENT_UPDATED =>\n`;

        let firstone = false;

        for (let dataset of template.datasets) {
            if (dataset.isSub){
                if (firstone)
                {
                    out += `            else`;
                }
                else {
                    out += `            `;
                    firstone = true;
                }
                out += `if dataset.name == "${dataset.structName}"\n`;
                out += `                ptr_${dataset.structName}_ref = dataset.data\n`;
            }
        }
        out += `    end\n`;

        firstone = false

        out += `        EXOS_DATASET_EVENT_PUBLISHED =>\n`;
        for (let dataset of template.datasets) {
            if (dataset.isPub){
                if (firstone)
                {
                    out += `            else`;
                }
                else {
                    out += `            `;
                    firstone = true;
                }
                out += `if dataset.name == "${dataset.structName}"\n`;
                out += `                ptr_${dataset.structName}_ref = dataset.data\n`;
            }
        }
        out += `    end\n`;

        firstone = false

        out += `        EXOS_DATASET_EVENT_DELIVERED =>\n`;
        for (let dataset of template.datasets) {
            if (dataset.isPub){
                if (firstone)
                {
                    out += `            else`;
                }
                else {
                    out += `            `;
                    firstone = true;
                }
                out += `if dataset.name == "${dataset.structName}"\n`;
                out += `                ptr_${dataset.structName}_ref = dataset.data\n`;
            }
        }
        out += `    end\n`;

        firstone = false

        out += `        EXOS_DATASET_EVENT_CONNECTION_CHANGED =>\n`;
        out += `            @match dataset.connection_state begin\n`;
        out += `                EXOS_STATE_DISCONNECTED => nothing\n`;
        out += `                EXOS_STATE_CONNECTED    => nothing\n`;
        out += `                EXOS_STATE_OPERATIONAL  => nothing\n`;
        out += `                EXOS_STATE_ABORTED      => exos_log_info(log_handle, EXOS_LOG_TYPE_USER, "Error")\n`;
        out += `            end\n`;
        out += `    end\n`;
        out += `    nothing\n`

        out += `end\n`;
        out += `\n`
        out += `dataset_event_cb_c = @cfunction(exos_dataset_event_cb, Cvoid,\n`;
        out += `                                (Ref{ExosDatasetHandle},\n`;
        out += `                                EXOS_DATASET_EVENT_TYPE, Ptr{Cvoid}))\n`;
        out += `\n`
        out += `function exos_datamodel_event_cb(datamodel::ExosDatamodelHandle,\n`
        out += `                                 event_type::EXOS_DATAMODEL_EVENT_TYPE,\n`
        out += `                                 info::Ptr{Cvoid})\n`
        out += `    @match event_type begin\n`
        out += `        EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED => begin\n`
        out += `            exos_log_info(log_handle, EXOS_LOG_TYPE_USER,\n`
        out += `                          "JULIA: Application changed to $(datamodel.connection_state)")\n`
        out += `            @match datamodel.connection_state begin\n`
        out += `                EXOS_STATE_DISCONNECTED => nothing\n`
        out += `                EXOS_STATE_CONNECTED    => nothing\n`
        out += `                EXOS_STATE_OPERATIONAL  => exos_log_info(log_handle, EXOS_LOG_TYPE_USER,\n`
        out += `                                                         "JULIA: Application operational!")\n`
        out += `                EXOS_STATE_ABORTED      => exos_log_info(log_handle, EXOS_LOG_TYPE_USER,\n`
        out += `                                                         "JULIA: Application error $(datamodel.error)")\n`
        out += `            end\n`
        out += `        end\n`
        out += `        EXOS_DATAMODEL_EVENT_SYNC_STATE_CHANGED =>\n`
        out += `        _ => exos_log_info(log_handle, EXOS_LOG_TYPE_USER,\n`
        out += `                           "JULIA: Unknown datamodel event")\n`
        out += `    end\n`
        out += `    nothing\n`
        out += `end\n`
        out += `\n`
        out += `exos_datamodel_event_cb_c = @cfunction(exos_datamodel_event_cb, Cvoid,\n`
        out += `                                      (Ref{ExosDatamodelHandle}, EXOS_DATAMODEL_EVENT_TYPE,\n`
        out += `                                      Ptr{Cvoid}))\n`
        out += `\n`

        return out;
    }
    _datasetInit(template)
    {
        let out = "";

        for (let dataset of template.datasets) {
            out += `    exos_dataset_init(${dataset.varName}, ${template.datamodel.structName.toLowerCase()}, "${dataset.structName}", temp_${dataset.structName}_ref, Csize_t(sizeof(data.${dataset.structName})))\n`;
        }

        out += `\n`;
        return out;
    }
    _datasetInfo(template)
    {
        let out = "";
        let count = 0;
        out += `    dataset${count} = JExosDatasetInfo("", temp_data, UInt64(sizeof(data)), 0, ntuple(x->Cuint(0), EXOS_ARRAY_DEPTH))\n`;
        count += 1;
        for (let dataset of template.datasets) {
            if (dataset.arraySize == 0){
                out += `    dataset${count} = JExosDatasetInfo("${dataset.structName}", temp_${dataset.structName}_ref, UInt64(sizeof(data.${dataset.structName})), 0, ntuple(x->Cuint(0), EXOS_ARRAY_DEPTH))\n`;
            } else {
                out += `    dataset${count} = JExosDatasetInfo("${dataset.structName}", temp_${dataset.structName}_ref, UInt64(sizeof(data.${dataset.structName})), 0, ntuple(x->Cuint(0), EXOS_ARRAY_DEPTH))\n`;
                count += 1;
                out += `    dataset${count} = JExosDatasetInfo("${dataset.structName}[0]", temp_${dataset.structName}_ref, UInt64(sizeof(data.${dataset.structName}[1])), 0, ntuple(i -> i == 1 ? Cuint(${dataset.arraySize}) : Cuint(0), EXOS_ARRAY_DEPTH))\n`;
            }
            count += 1;
            }

        out += `\n`;
        out += `    info = [`;
        for (let i = 0; i < count; i++) {
            if (i < count-1){
                out += `dataset${i}, `;
            } else {
                out += `dataset${i}]\n`;
            }
        }
        out += `\n`;
        out += `    exos_datamodel_calc_dataset_info(info)\n`;
        out += `\n`;

        return out;
    }

    _init_handles(template)
    {
        let out = "";
        out += `function init_handles()\n`
        out += `    _private_log = JExosLogPrivate(UInt32(0),\n`;
        out += `                                   Ref(0),\n`;
        out += `                                   ntuple(x->Ref(C_NULL), 4))\n`;
        out += `\n`;
        out += `    log_handle = JExosLogHandle("", UInt8(0), UInt8(0), UInt8(0),\n`
        out += `                                config_change_cb_c, Ref(0),\n`;
        out += `                                ntuple(x->UInt8(0), 4),\n`;
        out += `                                ntuple(x->UInt32(0), 4),\n`;
        out += `                                ntuple(x->Ref(C_NULL), 4),\n`;
        out += `                                _private_log)\n`;
        out += `\n`;
        out += `    exos_log_init(log_handle, "gStringAndArray_0")\n`;
        out += `    exos_log_info(log_handle, EXOS_LOG_TYPE_USER, "----------STARTING THE APP----------")\n`;
        out += `\n`;

        out += `    data = ${template.datamodel.structName}(`;
        let numDatasets = template.datasets.length
        let stringCount = 0;

        for (let [index, dataset] of template.datasets.entries()) {
            let dataType = this.convertPlcType(dataset.dataType);
            let isString = (dataset.dataType == "STRING") ? true : false;
            let isArray = dataset.arraySize > 0;
            let defaultValue = '0';
            let value = " ";

            let arr = [];
            if (isString == 0){
                value = isArray ? `ntuple(x->${dataType}(${defaultValue}), ${dataset.arraySize})` : `${dataType}(${defaultValue})`;
            } else {
                    arr = infoString[stringCount];
                    value = isArray ? `ntuple(x->ntuple(x->Cchar(${defaultValue}), ${arr[1][1]}), ${arr[1][0]})` : `ntuple(x->Cchar(${defaultValue}), ${arr[1][1]})`;
                    stringCount++; 
            }   

            if (index === 0) {
                out += `${value},\n`;
            } else if (index !== numDatasets - 1) {
                out += `                    ${value},\n`;
            } else {
                out += `                    ${value})\n`;
            }

        }
        stringCount = 0;
        out += `\n`;
        out += `    temp_data = Ref(data)\n`;

        for (let [index, dataset] of template.datasets.entries()) {
            let dataType = this.convertPlcType(dataset.dataType);
            let isString = (dataset.dataType == "STRING") ? true : false;
            let isArray = dataset.arraySize > 0;
            let defaultValue = '0';
            let value = " ";

            let arr = [];
            if (isString == 0){
                value = isArray ? `ntuple(x->${dataType}(${defaultValue}), ${dataset.arraySize})` : `${dataType}(${defaultValue})`;
            } else {
                    arr = infoString[stringCount];
                    value = isArray ? `ntuple(x->ntuple(x->Cchar(${defaultValue}), ${arr[1][1]}), ${arr[1][0]})` : `ntuple(x->Cchar(${defaultValue}), ${arr[1][1]})`;
                    stringCount++; 
            }   

            if (index !== numDatasets - 1) {
                out += `    temp_${dataset.structName}_ref = Ref(${value})\n`;
            } else {
                out += `    temp_${dataset.structName}_ref = Ref(${value})\n`;
            }

        }

        out += `\n`;
        out += `    sync_info = JExosDatamodelSyncInfo(UInt8(0),\n`;
        out += `                                       ntuple(x->UInt8(0), 8),\n`;
        out += `                                       UInt32(0),\n`;
        out += `                                       UInt32(0),\n`;
        out += `                                       EXOS_DATAMODEL_PROCESS_NON_BLOCKING,\n`;
        out += `                                       ntuple(x->UInt32(0), 7))\n`;
        out += `\n`;
        out += `    datamodel_private = JExosDatamodelPrivate(UInt32(0),\n`;
        out += `                                              Ref(0),\n`;
        out += `                                              ntuple(x->Ref(C_NULL), 8))\n`;
        out += `\n`;
        out += `    ${template.datamodel.structName.toLowerCase()} = JExosDatamodelHandle("",\n`;
        out += `                                            EXOS_STATE_DISCONNECTED,\n`
        out += `                                            EXOS_ERROR_OK,\n`
        out += `                                            Ref(0),\n`
        out += `                                            Int(0),\n`
        out += `                                            "",\n`
        out += `                                            C_NULL,\n`
        out += `                                            sync_info,\n`
        out += `                                            ntuple(x->UInt8(0), 8),\n`
        out += `                                            ntuple(x->UInt32(0), 8),\n`
        out += `                                            ntuple(x->Ref(C_NULL), 8),\n`
        out += `                                            datamodel_private)\n`
        out += `\n`;
        out += `    send_buffer = JExosBufferInfo(UInt32(0),\n`;
        out += `                                  UInt32(0),\n`;
        out += `                                  UInt32(0))\n`;
        out += `\n`;
        out += `    dataset_private = JExosDatasetPrivate(UInt32(0),\n`;
        out += `                                          Ref(0),\n`;
        out += `                                          ntuple(x->Ref(C_NULL), 8))\n`;
        out += `\n`;

        for (let dataset of template.datasets) {
            out += `    ${dataset.varName} = JExosDatasetHandle("${dataset.structName}",\n`;
            out += `                                        EXOS_DATASET_SUBSCRIBE,\n`;
            out += `                                        ${template.datamodel.structName.toLowerCase()},\n`;
            out += `                                        temp_${dataset.structName}_ref,\n`;
            out += `                                        UInt64(0),\n`;
            out += `                                        EXOS_ERROR_OK,\n`;
            out += `                                        EXOS_STATE_DISCONNECTED,\n`;
            out += `                                        send_buffer,\n`;
            out += `                                        Int32(0),\n`;
            out += `                                        Int32(0),\n`;
            out += `                                        Ref(0),\n`;
            out += `                                        dataset_event_cb_c,\n`;
            out += `                                        ntuple(x->UInt8(0), 8),\n`;
            out += `                                        ntuple(x->UInt32(0), 8),\n`;
            out += `                                        ntuple(x->Ref(C_NULL), 8),\n`;
            out += `                                        dataset_private)\n`;
            out += `\n`;
        }

        out += `    exos_datamodel_init(${template.datamodel.structName.toLowerCase()}, "${template.datamodelInstanceName}", "${template.aliasName}")\n`;
        out += `\n`;
        out += this._datasetInit(this.template);
        out += this._datasetInfo(this.template);

        out += `    (${template.datamodel.structName.toLowerCase()}, log_handle, info`;

        for (let dataset of template.datasets)
        {
            out += `, ${dataset.varName}, temp_${dataset.structName}_ref`;
        }

        out += `)\n`;
        out += `end\n`;
        out += `\n`;

        return out;
    }

    _cleanup(template)
    {
        let out = "";
        out += `function cleanup(${template.datamodel.structName.toLowerCase()}::ExosDatamodelHandle, log_handle::ExosLogHandle)\n`;
        out += `    exos_datamodel_delete(${template.datamodel.structName.toLowerCase()})\n`;
        out += `    exos_log_delete(log_handle)\n`;
        out += `    exos_datamodel_disconnect(${template.datamodel.structName.toLowerCase()})\n`;
        out += `end\n`;
        out += `\n`;

        return out;
    }
    /**
     * Creates the script for the initialising the log handle
     */
    _generateInitLogHandle() {
        let out = "";
        out += `function init_log_handle()\n`;
        out += `    _private_log = JExosLogPrivate(UInt32(0),\n`;
        out += `                                   Ref(0),\n`;
        out += `                                   ntuple(x->Ref(C_NULL), 4))\n`;
        out += `\n`;
        out += `    log_handle = JExosLogHandle("", UInt8(0), UInt8(0), UInt8(0),\n`;
        out += `                                config_change_cb_c, Ref(0),\n`;
        out += `                                ntuple(x->UInt8(0), 4),\n`;
        out += `                                ntuple(x->UInt32(0), 4),\n`;
        out += `                                ntuple(x->Ref(C_NULL), 4),\n`;
        out += `                                _private_log)\n`;
        out += `end\n`;
        out += `\n`;
        out += `log_handle = init_log_handle()\n`;
        out += `\n`;
        return out;
    }
    /**
     * Creates the script for the main function
     */
    _generateMain(template) {
        let out = "";
        out += `function main()\n`;
        out += `    ${template.datamodel.structName.toLowerCase()}, log_handle, info`;

        for (let dataset of template.datasets)
        {
            out += `, ${dataset.varName}, temp_${dataset.structName}_ref`;
        }

        out += ` = init_handles()\n`;

        out += `\n`;
        out += `    exos_datamodel_connect(${template.datamodel.structName.toLowerCase()}, config_${this.datamodel.typeName.toLowerCase()},\n`;
        out += `                           info, exos_datamodel_event_cb_c)\n`;
        out += `\n`
        for (let dataset of template.datasets) {
            if (dataset.isSub) {
                if (dataset.isPub) {
                    out += `    exos_dataset_connect(${dataset.varName}, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, dataset_event_cb_c)\n`;
                }
                else {
                    out += `    exos_dataset_connect(${dataset.varName}, EXOS_DATASET_SUBSCRIBE, dataset_event_cb_c)\n`;
                }
            }
            else if (dataset.isPub) {
                out += `    exos_dataset_connect(${dataset.varName}, EXOS_DATASET_PUBLISH, dataset_event_cb_c)\n`;
            }
        }

        out += `\n`

        out += `    while true\n`;
        out += `        exos_datamodel_process(${template.datamodel.structName.toLowerCase()})\n`;
        out += `        exos_log_process(log_handle)\n`;
        out += `        #put your cyclic code here!\n\n`;
        out += `    end\n`;
        out += `    cleanup(${template.datamodel.structName.toLowerCase()}, log_handle)\n`;
        out += `\n`;
        out += `end\n`;
        out += `\n`;

        return out;
    }


    /**
     * Creates the Julia file (main source)
     */

    _generateJulia() {

        let out = "";
        out += this.generateInit(this.template);
        out += this._makeDataTypes(false);
        out += this._generateInitLogHandle();

        out += this._generateCallbacks(this.template);
        out += this._init_handles(this.template);
        out += this._cleanup(this.template);
        out += this._generateMain(this.template);

        out += `main()\n`;
        return out;
    }

}

module.exports = {TemplateLinuxJulia};