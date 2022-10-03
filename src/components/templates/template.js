/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const {Datamodel, Dataset} = require('../../datamodel');

/**
 * Class used to generate a template object for code-generators
 * 
 * It uses {@link Datamodel} class as input for creating the {@link ApplicationTemplate} object
 * The {@link Datamodel} that passed to the contructor can be accessed via the {@link Template.datamodel} property
 * 
 * Structure of the {@link ApplicationTemplate} object:
 * 
 * @typedef {Object} ApplicationTemplateHandle 
 * @property {string} dataType typename of a handle structure => `MyApplicationHandle_t`
 * @property {string} name name of a handle instance, hardcoded => `handle`
 * 
 * @typedef {Object} ApplicationTemplateDatamodel
 * @property {string} dataType typename of the datamodel structure or enum => `MyApplication`
 * @property {string} varName name of a datamodel instance, as lowercase => `myapplication` or `myapplication_datamodel` if the dataType is all lowercase
 * @property {string} structName typename of the datamodel structure or enum, same as dataType => `MyApplication`
 * @property {string} libStructName typename of a library wrapper datamodel - same as dataType with a "lib" prefix => `libMyApplication`
 * @property {string} handleName name of a handle used for the datamodel in the library wrapper => `h_MyApplication`
 * @property {string} className name of a generated class for this datamodel => `MyApplicationDatamodel`
 * @property {string} datasetClassName name of a generated class for its datasets => `MyApplicationDataset`
 * 
 * @typedef {Object} ApplicationTemplateDataset
 * @property {string} type `struct` | `variable` | `enum` | `value` type of dataset, whereas enums contains only values
 * @property {string} dataType typename of the dataset structure enum or variable, => `MyConfig`, `BOOL` or `USINT`
 * @property {string} structName name of the dataset structure enum or variable, => `Config`, `Enable` or `Buffer`
 * @property {string} varName name of a dataset instance, in lowercase => `config`, `enable` or `config_dataset`, `enable_dataset` if the dataType is all lowercase
 * @property {string} libDataType typename of a library wrapper dataset structure enum or variable => `libMyApplicationConfig` or `libMyApplicationEnable`
 * @property {number} arraySize number of array elements if the dataset is an array, otherwise 0. => `Config:0`, `Enable:0`, `Buffer:10`
 * @property {number} stringLength for `STRING` datatypes, this is the allocated size for a string. e.g. `81` for a `STRING[80]`
 * @property {string} comment comment as it appears in the TYPE declaration file => `(*PUB*)` or `(*PUB SUB*)` or `(*this is a counter SUB*)`
 * @property {boolean} isPub set via the comment to `true` if the dataset should be published. (platform dependent - this is `(*PUB*)` for AR and `(*SUB*)` for GPOS)
 * @property {boolean} isSub set via the comment to `true` if the dataset should be subscribed to. (platform dependent - this is `(*SUB*)` for AR and `(*PUB*)`for GPOS)
 * @property {boolean} isPrivate set to true if the comment includes the word `private`
 * @property {boolean} isBuffered set to true if the comment includes the word `BUFFERED`
 * 
 * @typedef {Object} ApplicationTemplate
 * @property {string} targetName default name of the target, set in the TP => `gTarget_0`
 * @property {string} headerName name of the generated headerfile to be included in applications => `Datamodel.headerFile.name`
 * @property {string} libHeaderName name of a library wrapper headerfile to be included `libmyapplication.h`
 * @property {string} logname name of a `exos_log_handle_t` instance in the application, hardcoded => `logger`
 * @property {string} aliasName the 'alias' name use in the `exos_datamodel_init` and name for the `exos_log_init` - meaning the name that the Application gets in the Logger => `gMyApplication_0`
 * @property {string} loggerClassName name of a generated logger class => `MyApplicationLogger`
 * @property {string} datamodelInstanceName name of the datamodel instance (aka shared memory name) for the configuration and the `exos_datamodel_connect()` => `MyApplication_0`
 * @property {ApplicationTemplateHandle} handle handle structure for used for AR libraries (to overcome downloads)
 * @property {ApplicationTemplateDatamodel} datamodel datamodel related types and instance names for the application
 * @property {ApplicationTemplateDataset[]} datasets dataset type and instance names for the application  
 */
class Template
{

    /**
     * Template structure for creating applications 
     * 
     * The structure description is based on the following datatype:
     * 
     * @example
     * TYPE
     *   MyApplication : 	STRUCT 
     *        Enable : BOOL; (*PUB*)
     *        Counter : INT; (*SUB*)
     *        Buffer : ARRAY[0..9]OF USINT;
     *        Config : MyConfig; (*PUB SUB*)
     *   END_STRUCT;
     *   MyConfig : 	STRUCT 
     *        Message : STRING[80];
     *        SleepTime : LREAL;
     *   END_STRUCT;
     * END_TYPE
     * 
     * @type {ApplicationTemplate}
     */
    template;
    
    /**
     * Whether the template is generated for Linux or not
     * @type {boolean}
     */
    isLinux;

    /**
     * Create an {@link ApplicationTemplate} object from the given {@link Datamodel} for Linux or AR.
     * The generated {@link ApplicationTemplate} is platform specific
     * so that `datasets` that are published (via `isPub=true`) in Automation Runtime 
     * are subscribed to  (via `isSub=true`) in Linux, and vice versa.
     *
     * @param {Datamodel} datamodel existing {@link Datamodel} class that should be used for this template 
     * @param {boolean} Linux generate structure for Linux (`true`), otherwise AR (`false`) when used in Linux, the `datasets[].isPub` and `datasets[].isSub` are reversed
     * @param {boolean} [recurse] generate a recursive template structure for datasets (at all sublevels) - with this you get `datasets[].datasets[].datasets[]` and so on
     */
    constructor(datamodel, Linux, recurse) {
        
        /**
         * create the template structure form the Dataset structure
         * 
         * @param {Dataset} types generated {@link Dataset} structure from the {@link Datamodel} class
         * @param {string} headerName predefined `headerFile.name` from the {@link Datamodel} class
         * @param {boolean} Linux when used in Linux, the `datasets[].isPub` and `datasets[].isSub` are reversed
         * @param {boolean} [recurse] generate a recursive template structure for datasets (at all sublevels)
         * @returns {ApplicationTemplate}
         */
        function configTemplate(types, headerName, Linux, recurse) {

            /**
             * @param {ApplicationTemplateDataset[]} datasets 
             * @param {Dataset} type 
             * @param {boolean} [recurse] generate a recursive template structure for datasets (at all sublevels)
             */
            function readDatasets(datasets, type, recurse) {
                for (let child of type.children) {
                    let object = {};
                    object["type"] = child.name;
                    object["structName"] = child.attributes.name;
                    object["varName"] = child.attributes.name.toLowerCase() + (child.attributes.name == child.attributes.name.toLowerCase() ? "_dataset" : "");
                    object["dataType"] = child.attributes.dataType;
                    object["libDataType"] = template.datamodel.libStructName + child.attributes.name;
                    if (typeof child.attributes.arraySize === "number") {
                        object["arraySize"] = child.attributes.arraySize;
                    } else {
                        object["arraySize"] = 0;
                    }
                    object["comment"] = child.attributes.comment.replace("PUB","").replace("SUB","");
                    if (typeof child.attributes.comment === "string") {
                        if(Linux)
                        {
                            object["isPub"] = child.attributes.comment.includes("SUB");
                            object["isSub"] = child.attributes.comment.includes("PUB");
                            object["isBuffered"] = child.attributes.comment.includes("BUFFERED"); // not used in Linux
                        }
                        else
                        {
                            object["isPub"] = child.attributes.comment.includes("PUB");
                            object["isSub"] = child.attributes.comment.includes("SUB");
                            object["isBuffered"] = child.attributes.comment.includes("BUFFERED");
                            if(object["isBuffered"])
                            {
                                datamodel.hasBuffered = true;
                                template.datamodel.hasBuffered = true;
                            }
                        }
                        object["isPrivate"] = child.attributes.comment.includes("private");
                    } else {
                        object["comment"] = "";
                        object["isPub"] = false;
                        object["isSub"] = false;
                        object["isPrivate"] = false;
                        object["isBuffered"] = false;
                    }
                    if (child.attributes.hasOwnProperty("stringLength")) { object["stringLength"] = child.attributes.stringLength; }
                    
                    if(recurse !== undefined && recurse)
                    {
                        object["datasets"] = [];
                        if(child.name == "struct") {
                            readDatasets(object["datasets"], child, recurse);
                        }
                    }
                    datasets.push(object);

                }
            }

            var template = {
                targetName: "",
                headerName: "",
                datamodelInstanceName: "",
                logname: "",
                aliasName: "",
                loggerClassName: "",
                datasetClassName: "",
                handle: {
                    dataType: "",
                    name: "",
                },
                libHeaderName: "",
                datamodel: {
                    structName: "",
                    varName: "",
                    dataType: "",
                    comment: "",
                    libStructName: "",
                    handleName: "",
                    className: "",
                    hasBuffered: false
                },
                datasets: []
            }
            template.targetName = "gTarget_0";
            template.logname = "logger";
            template.aliasName = `g${types.attributes.dataType}_0`;
            template.loggerClassName = `${types.attributes.dataType}Logger`;
            template.datamodelInstanceName = `${types.attributes.dataType}_0`;
            template.headerName = headerName;
            template.libHeaderName = `lib${types.attributes.dataType.toLowerCase()}.h`
            template.handle.dataType = `${types.attributes.dataType}Handle_t`;
            template.handle.name = "handle";

            template.datamodel.dataType = types.attributes.dataType;
            template.datamodel.structName = types.attributes.dataType;
            template.datamodel.className = `${types.attributes.dataType}Datamodel`;
            template.datamodel.datasetClassName = `${types.attributes.dataType}Dataset`;
            //check if toLowerCase is equal to datatype name, then extend it with _datamodel
            if (types.attributes.dataType == types.attributes.dataType.toLowerCase()) {
                template.datamodel.varName = types.attributes.dataType.toLowerCase() + "_datamodel";
            }
            else {
                template.datamodel.varName = types.attributes.dataType.toLowerCase();
            }
    
            template.datamodel.libStructName = "lib" + types.attributes.dataType;
            template.datamodel.handleName = "h_" + types.attributes.dataType;
    
            readDatasets(template.datasets, types, recurse);
        
            return template;
        }

        this.isLinux = Linux;
        this.datamodel = datamodel;
        if (datamodel != undefined)
            this.template = configTemplate(this.datamodel.dataset, this.datamodel.headerFile.name, Linux, recurse);
    }

}

const fs = require('fs');
const path = require('path');

if (require.main === module) {

    process.stdout.write(`exOS Template \n`);

    if (process.argv.length > 3) {

        let fileName = process.argv[2];
        let structName = process.argv[3];

        if (fs.existsSync(fileName)) {

                let datamodel = new Datamodel(fileName, structName, [`${structName}.h`]);
                let outDir = path.join(__dirname,path.dirname(fileName));
                let template = new Template(datamodel, true, true);

                process.stdout.write(`Writing ${structName} to folder: ${outDir}\r\n`);

                fs.writeFileSync(path.join(outDir,`exos_${structName.toLowerCase()}.json`),JSON.stringify(template.template,null,4));

        } else {
            process.stderr.write(`file '${fileName}' not found.`);
        }

    }
    else {
        process.stderr.write("usage: ./template.js <filename.typ> <structname>\r\n");
    }
}

module.exports = {Template};