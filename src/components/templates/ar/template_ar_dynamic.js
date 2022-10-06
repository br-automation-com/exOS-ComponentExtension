/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { Datamodel, GeneratedFileObj } = require('../../../datamodel');
const { Template, ApplicationTemplate } = require('../template');
const {TemplateARHeap } = require('./template_ar_heap');

class TemplateARDynamic extends Template {

    /** 
     * source code with all functionality for the libary
     * @type {GeneratedFileObj} 
     */
    librarySource;
    
    /** 
     * function block declaration for the AR library
     * @type {GeneratedFileObj} 
    */
    libraryFun;
    
    /** 
     * varaible declaration for the ST program
     * @type {GeneratedFileObj} 
     */
    iecProgramVar;

    /**
     * application implementation code in ST
     * @type {GeneratedFileObj} 
     */
    iecProgramST;

    /**
     * application implementation code for buffered datasets in in action ST
     * @type {GeneratedFileObj} 
     */
     iecBufferedActionST;

    /**
     * @type {TemplateARHeap}
     */
    heap;

    /**
     * {@linkcode TemplateARDynamic} Generate source code for dynamic AR C-Library and ST-Application
     * 
     * Generates following {@link GeneratedFileObj}
     * - {@linkcode librarySource}
     * - {@linkcode libraryFun}
     * - {@linkcode iecProgramVar} 
     * - {@linkcode iecProgramST}
     * 
     * Using {@linkcode TemplateARHeap}
     * - `heap.heapSource` declaring the dynamic heap
     * 
     * @param {Datamodel} datamodel
     */
    constructor(datamodel) {
        super(datamodel,false);

        this.librarySource = {name:`${this.datamodel.typeName.toLowerCase()}.c`, contents:this._generateSource(), description:`${this.datamodel.typeName} library source`};
        this.libraryFun = {name:`${this.datamodel.typeName.substr(0,10)}.fun`, contents:this._generateFun(), description:`${this.datamodel.typeName} function blocks`}; // Avoid Error in AS: The name of the .fun file is not equal to the name of the library.	(9348)
        this.iecProgramVar = {name:`${this.datamodel.typeName}.var`, contents:this._generateIECProgramVar(), description:`${this.datamodel.typeName} variable declaration`};
        this.iecProgramST = {name:`${this.datamodel.typeName}.st`, contents:this._generateIECProgramST(), description:`${this.datamodel.typeName} application`};
        if(datamodel.hasBuffered)
            this.iecBufferedActionST = {name:`UpdateBufferActions.st`, contents:this._generateIECBufferedActionST(), description:`${this.datamodel.typeName} buffer actions`};
        this.heap = new TemplateARHeap(100000);
    }
    
    /**
     * @returns {string} `{LibraryName}.c`: source code with all functionality for the libary
     */
    _generateSource() {

        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateIncludes(template) {
            let out = "";
            out += `#include <${template.datamodel.dataType.substring(0, 10)}.h>\n\n`;
            out += `#define EXOS_ASSERT_LOG &${template.handle.name}->${template.logname}\n`;
            out += `#define EXOS_ASSERT_CALLBACK inst->_state = 255;\n`;
            out += `#include "exos_log.h"\n`;
            out += `#include "${template.headerName}"\n`;
            out += `#include <string.h>\n`;
            out += `\n`;
        
            out += `#define SUCCESS(_format_, ...) exos_log_success(&${template.handle.name}->${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
            out += `#define INFO(_format_, ...) exos_log_info(&${template.handle.name}->${template.logname}, EXOS_LOG_TYPE_USER, _format_, ##__VA_ARGS__);\n`;
            out += `#define VERBOSE(_format_, ...) exos_log_debug(&${template.handle.name}->${template.logname}, EXOS_LOG_TYPE_USER + EXOS_LOG_TYPE_VERBOSE, _format_, ##__VA_ARGS__);\n`;
            out += `#define ERROR(_format_, ...) exos_log_error(&${template.handle.name}->${template.logname}, _format_, ##__VA_ARGS__);\n`;
            out += `\n`;
            return out;
        }

        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateHandle(template) {
            let out = "";
        
            if(template.datamodel.hasBuffered)
            {
                out += `typedef struct\n`;
                out += `{\n`;
                out += `    void *self;\n`;
                out += `    void* pData;\n`;
                out += `    size_t dataSize;\n`;
                out += `    void* databuffer;\n`;
                out += `    size_t bufferSize;\n`;
                out += `    size_t head;\n`;
                out += `    size_t tail;\n`;
                out += `    size_t overflowErrors;\n`;
                out += `} ${template.datamodel.structName}BufferHandle_t;\n`;
                out += `\n`;
            }

            out += `typedef struct\n{\n`;
            out += `    void *self;\n`
            out += `    exos_log_handle_t ${template.logname};\n`;
            out += `    ${template.datamodel.structName} data;\n\n`;
            out += `    exos_datamodel_handle_t ${template.datamodel.varName};\n\n`;
            for (let dataset of template.datasets) {
                // initialize non-string comments to "" to avoid crashes in the next if...
                if (typeof dataset.comment !== 'string') {
                    dataset.comment = "";
                }
        
                if (dataset.isSub || dataset.isPub) {
                    out += `    exos_dataset_handle_t ${dataset.varName};\n`;
                }
                if (dataset.isBuffered) {
                    out += `    ${template.datamodel.structName}BufferHandle_t ${dataset.varName}_buffer;\n`;
                }
            }
        
            out += `} ${template.handle.dataType};\n\n`;
            return out;
        }

        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateCallbacks(template) {
            let out = "";

            if(template.datamodel.hasBuffered)
            {
                // Helper function for datasetEvent 
                out += `static int ${template.datamodel.structName}BufferPush(${template.datamodel.structName}BufferHandle_t* bufferHandle, void* data)\n`;
                out += `{\n`;
                out += `    if (NULL == bufferHandle)\n`;
                out += `    {\n`;
                out += `        return -1;\n`;
                out += `    }\n`;
                out += `    if ((void *)bufferHandle != bufferHandle->self)\n`;
                out += `    {\n`;
                out += `        return -1;\n`;
                out += `    }\n`;
                out += `    if (((bufferHandle->head + 1) % bufferHandle->bufferSize) == bufferHandle->tail)\n`;
                out += `    {\n`;
                out += `        return -1;\n`;
                out += `    }\n\n`;

                out += `    memcpy(bufferHandle->databuffer + (bufferHandle->head * bufferHandle->dataSize), data, bufferHandle->dataSize);\n`;
                out += `    bufferHandle->head = (bufferHandle->head + 1) % bufferHandle->bufferSize;\n\n`;

                out += `    return 0;\n`;
                out += `}\n\n`;
            }
        
            out += `static void datasetEvent(exos_dataset_handle_t *dataset, EXOS_DATASET_EVENT_TYPE event_type, void *info)\n{\n`;
            out += `    struct ${template.datamodel.structName}Cyclic *inst = (struct ${template.datamodel.structName}Cyclic *)dataset->datamodel->user_context;\n`;
            out += `    ${template.handle.dataType} *${template.handle.name} = (${template.handle.dataType} *)inst->Handle;\n\n`;
            out += `    switch (event_type)\n    {\n`;
            out += `    case EXOS_DATASET_EVENT_UPDATED:\n`;
            out += `        VERBOSE("dataset %s updated! latency (us):%i", dataset->name, (exos_datamodel_get_nettime(dataset->datamodel) - dataset->nettime));\n`;
            out += `        //handle each subscription dataset separately\n`;
            var atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isSub) {
                    if (atleastone) {
                        out += `        else `;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    out += `if(0 == strcmp(dataset->name, "${dataset.structName}"))\n`;
                    out += `        {\n`;
                    if(dataset.isBuffered)
                    {
                        out += `            if(${template.datamodel.structName}BufferPush(&handle->${dataset.varName}_buffer, dataset->data))\n`;
                        out += `            {\n`;
                        out += `                handle->${dataset.varName}_buffer.overflowErrors++;\n`;
                        out += `                ERROR("Buffer overflow in ${dataset.structName} update");\n`;
                        out += `            }\n`;
                    }
                    else
                    {
                        if(Datamodel.isScalarType(dataset) && (dataset.arraySize == 0)) {
                            out += `            inst->p${template.datamodel.structName}->${dataset.structName} = *(${dataset.dataType} *)dataset->data;\n`;
                        }
                        else {
                            out += `            memcpy(&inst->p${template.datamodel.structName}->${dataset.structName}, dataset->data, dataset->size);\n`;
                        }
                    }
                    out += `        }\n`;
                }
            }
            out += `        break;\n\n`;
        
            out += `    case EXOS_DATASET_EVENT_PUBLISHED:\n`;
            out += `        VERBOSE("dataset %s published to local server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);\n`;
            out += `        //handle each published dataset separately\n`;
            atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isPub) {
                    if (atleastone) {
                        out += `        else `;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    out += `if(0 == strcmp(dataset->name, "${dataset.structName}"))\n`;
                    out += `        {\n`;
                    out += `            // ${dataset.dataType} *${dataset.varName} = (${dataset.dataType} *)dataset->data;\n`;
                    out += `        }\n`;
                }
            }
            out += `        break;\n\n`;
        
            out += `    case EXOS_DATASET_EVENT_DELIVERED:\n`;
            out += `        VERBOSE("dataset %s delivered to remote server for distribution! send buffer free:%i", dataset->name, dataset->send_buffer.free);\n`;
            out += `        //handle each published dataset separately\n`;
            atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isPub) {
                    if (atleastone) {
                        out += `        else `;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    out += `if(0 == strcmp(dataset->name, "${dataset.structName}"))\n`;
                    out += `        {\n`;
                    out += `            // ${dataset.dataType} *${dataset.varName} = (${dataset.dataType} *)dataset->data;\n`;
                    out += `        }\n`;
                }
            }
            out += `        break;\n\n`;
        
            out += `    case EXOS_DATASET_EVENT_CONNECTION_CHANGED:\n`;
            out += `        INFO("dataset %s changed state to %s", dataset->name, exos_get_state_string(dataset->connection_state));\n\n`;
            out += `        switch (dataset->connection_state)\n`;
            out += `        {\n`;
            out += `        case EXOS_STATE_DISCONNECTED:\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_CONNECTED:\n`;
            out += `            //call the dataset changed event to update the dataset when connected\n`;
            out += `            //datasetEvent(dataset,EXOS_DATASET_UPDATED,info);\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_OPERATIONAL:\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_ABORTED:\n`;
            out += `            ERROR("dataset %s error %d (%s) occured", dataset->name, dataset->error, exos_get_error_string(dataset->error));\n`;
            out += `            break;\n`;
            out += `        }\n`;
            out += `        break;\n`;
            out += `    }\n\n`;
        
            out += `}\n\n`;
        
            out += `static void datamodelEvent(exos_datamodel_handle_t *datamodel, const EXOS_DATAMODEL_EVENT_TYPE event_type, void *info)\n{\n`;
            out += `    struct ${template.datamodel.structName}Cyclic *inst = (struct ${template.datamodel.structName}Cyclic *)datamodel->user_context;\n`;
            out += `    ${template.handle.dataType} *${template.handle.name} = (${template.handle.dataType} *)inst->Handle;\n\n`;
            out += `    switch (event_type)\n    {\n`;
            out += `    case EXOS_DATAMODEL_EVENT_CONNECTION_CHANGED:\n`;
            out += `        INFO("application changed state to %s", exos_get_state_string(datamodel->connection_state));\n\n`;
            out += `        inst->Disconnected = 0;\n`;
            out += `        inst->Connected = 0;\n`;
            out += `        inst->Operational = 0;\n`;
            out += `        inst->Aborted = 0;\n\n`;
            out += `        switch (datamodel->connection_state)\n`;
            out += `        {\n`;
            out += `        case EXOS_STATE_DISCONNECTED:\n`;
            out += `            inst->Disconnected = 1;\n`;
            out += `            inst->_state = 255;\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_CONNECTED:\n`;
            out += `            inst->Connected = 1;\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_OPERATIONAL:\n`;
            out += `            SUCCESS("${template.datamodel.structName} operational!");\n`
            out += `            inst->Operational = 1;\n`;
            out += `            break;\n`;
            out += `        case EXOS_STATE_ABORTED:\n`;
            out += `            ERROR("application error %d (%s) occured", datamodel->error, exos_get_error_string(datamodel->error));\n`;
            out += `            inst->_state = 255;\n`;
            out += `            inst->Aborted = 1;\n`;
            out += `            break;\n`;
            out += `        }\n`;
            out += `        break;\n`;
            out += `    case EXOS_DATAMODEL_EVENT_SYNC_STATE_CHANGED:\n`;
            out += `        break;\n\n`;
            out += `    default:\n`;
            out += `        break;\n\n`;
           
            out += `    }\n\n`;
           
            out += `}\n\n`;
        
            return out;
        }
        
        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateInit(template) {
            let out = "";
        
            out += `_BUR_PUBLIC void ${template.datamodel.structName}Init(struct ${template.datamodel.structName}Init *inst)\n{\n`;
            out += `    ${template.handle.dataType} *${template.handle.name};\n`;
            out += `    TMP_alloc(sizeof(${template.handle.dataType}), (void **)&${template.handle.name});\n`;
            out += `    if (NULL == handle)\n`;
            out += `    {\n`;
            out += `        inst->Handle = 0;\n`;
            out += `        return;\n`;
            out += `    }\n\n`;
            out += `    memset(&${template.handle.name}->data, 0, sizeof(${template.handle.name}->data));\n`;
            out += `    ${template.handle.name}->self = ${template.handle.name};\n\n`;

            if(template.datamodel.hasBuffered)
            {
                out += `    // Allocation and initialization of buffer(s)\n`;
                template.datasets.filter((dataset) => {
                    if(dataset.isBuffered)
                    {
                        out += `    handle->${dataset.varName}_buffer.pData = 0; // set in ${template.datamodel.structName}Cyclic\n`;
                        out += `    handle->${dataset.varName}_buffer.dataSize = sizeof(handle->data.${dataset.structName});\n`;
                        out += `    handle->${dataset.varName}_buffer.bufferSize = 20; // Note that empty is head==tail, thus only bufferSize-1 entries may be used.\n`;
                        out += `    TMP_alloc(handle->${dataset.varName}_buffer.dataSize * handle->${dataset.varName}_buffer.bufferSize, (void **)&handle->${dataset.varName}_buffer.databuffer);\n`;
                        out += `    if (NULL == handle->${dataset.varName}_buffer.databuffer)\n`;
                        out += `    {\n`;
                        out += `        inst->${dataset.structName}BufferHandle = 0;\n`;
                        out += `        TMP_free(sizeof(${template.datamodel.structName}Handle_t), (void *)handle);\n`;
                        out += `        inst->Handle = 0;\n`;
                        out += `        return;\n`;
                        out += `    }\n`;
                        out += `    \n`;
                        out += `    handle->${dataset.varName}_buffer.head = 0;\n`;
                        out += `    handle->${dataset.varName}_buffer.tail = 0;\n`;
                        out += `    handle->${dataset.varName}_buffer.overflowErrors = 0;\n`;
                        out += `    memset(handle->${dataset.varName}_buffer.databuffer, 0, handle->${dataset.varName}_buffer.dataSize * handle->${dataset.varName}_buffer.bufferSize);\n`;
                        out += `    inst->${dataset.structName}BufferHandle = (UDINT)&handle->${dataset.varName}_buffer;\n`;
                        out += `    handle->${dataset.varName}_buffer.self = &handle->${dataset.varName}_buffer;\n`;
                        out += `    \n`;
                    }
                });
            }

            out += `    exos_log_init(&${template.handle.name}->${template.logname}, "${template.aliasName}");\n\n`;
            out += `    \n`;
            out += `    \n`;
            out += `    exos_datamodel_handle_t *${template.datamodel.varName} = &${template.handle.name}->${template.datamodel.varName};\n`;
            for (let dataset of template.datasets) {
                if (dataset.isSub || dataset.isPub) {
                    out += `    exos_dataset_handle_t *${dataset.varName} = &${template.handle.name}->${dataset.varName};\n`;
                }
            }
        
            //initialization
            out += `    EXOS_ASSERT_OK(exos_datamodel_init(${template.datamodel.varName}, "${template.datamodelInstanceName}", "${template.aliasName}"));\n\n`;
            for (let dataset of template.datasets) {
                if (dataset.isSub || dataset.isPub) {
                    out += `    EXOS_ASSERT_OK(exos_dataset_init(${dataset.varName}, ${template.datamodel.varName}, "${dataset.structName}", &${template.handle.name}->data.${dataset.structName}, sizeof(${template.handle.name}->data.${dataset.structName})));\n`;
                }
            }
            out += `    \n`;
            out += `    inst->Handle = (UDINT)${template.handle.name};\n`;
            out += `}\n\n`;
        
            return out;
        }
        
        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateCyclic(template) {
            let out = "";

            if(template.datamodel.hasBuffered)
            {
                // BufferUpdate is kind of a cyclic thing
                out += `_BUR_PUBLIC void ${template.datamodel.structName}BufferUpdate(struct ${template.datamodel.structName}BufferUpdate* inst)\n`;
                out += `{\n`;
                out += `    inst->DatasetUpdated = false;\n`;
                out += `    inst->Error = false;\n`;
                out += `    inst->OverflowErrors = 0;\n`;
                out += `    inst->PendingUpdates = 0;\n`;
                out += `\n`;
                out += `    ${template.datamodel.structName}BufferHandle_t *bufferHandle =  (${template.datamodel.structName}BufferHandle_t *)inst->BufferHandle;\n`;
                out += `    if (NULL == bufferHandle)\n`;
                out += `    {\n`;
                out += `        inst->Error = true;\n`;
                out += `        return;\n`;
                out += `    }\n`;
                out += `    if ((void *)bufferHandle != bufferHandle->self)\n`;
                out += `    {\n`;
                out += `        inst->Error = true;\n`;
                out += `        return;\n`;
                out += `    }\n`;
                out += `\n`;
                out += `    inst->Error = false;\n`;
                out += `    inst->OverflowErrors = bufferHandle->overflowErrors;\n`;
                out += `\n`;
                out += `    if(inst->UpdateDataset)\n`;
                out += `    {\n`;
                out += `        if (bufferHandle->tail == bufferHandle->head) // empty\n`;
                out += `        {\n`;
                out += `            inst->DatasetUpdated = false;\n`;
                out += `        }\n`;
                out += `        else\n`;
                out += `        {\n`;
                out += `            if(NULL != bufferHandle->pData)\n`;
                out += `            {\n`;
                out += `                memcpy(bufferHandle->pData, bufferHandle->databuffer + (bufferHandle->tail * bufferHandle->dataSize), bufferHandle->dataSize);\n`;
                out += `                //void* handle = bufferHandle->data[bufferHandle->tail];\n`;
                out += `                //should we set 0? bufferHandle->data[bufferHandle->tail] = NULL;\n`;
                out += `                bufferHandle->tail = (bufferHandle->tail + 1) % bufferHandle->bufferSize;\n`;
                out += `                inst->DatasetUpdated = true;\n`;
                out += `            }\n`;
                out += `            else\n`;
                out += `            {\n`;
                out += `                inst->Error = true;\n`;
                out += `                inst->DatasetUpdated = false;\n`;
                out += `            }\n`;
                out += `        }\n`;
                out += `    }\n`;
                out += `    else\n`;
                out += `    {\n`;
                out += `        inst->DatasetUpdated = false;\n`;
                out += `    }\n`;
                out += `\n`;
                out += `    if (bufferHandle->tail > bufferHandle->head)\n`;
                out += `        inst->PendingUpdates = bufferHandle->bufferSize - (bufferHandle->tail - bufferHandle->head);\n`;
                out += `    else\n`;
                out += `        inst->PendingUpdates = bufferHandle->head - bufferHandle->tail;\n`;
                out += `}\n\n`;
            }

            out += `_BUR_PUBLIC void ${template.datamodel.structName}Cyclic(struct ${template.datamodel.structName}Cyclic *inst)\n{\n`;
        
            out += `    ${template.handle.dataType} *${template.handle.name} = (${template.handle.dataType} *)inst->Handle;\n\n`;
            out += `    inst->Error = false;\n`;
            out += `    if (NULL == ${template.handle.name} || NULL == inst->p${template.datamodel.structName})\n`;
            out += `    {\n`;
            out += `        inst->Error = true;\n`;
            out += `        return;\n`;
            out += `    }\n`;
            out += `    if ((void *)${template.handle.name} != ${template.handle.name}->self)\n`;
            out += `    {\n`;
            out += `        inst->Error = true;\n`;
            out += `        return;\n`;
            out += `    }\n\n`;
        
            if(template.datamodel.hasBuffered)
            {
                out += `    // setup buffer address(es) for ${template.datamodel.structName}Update:\n`;
                template.datasets.filter((dataset) => {
                    if(dataset.isBuffered)
                    {
                        out += `    handle->${dataset.varName}_buffer.pData = &inst->p${template.datamodel.structName}->${dataset.structName};\n`;
                    }
                });
                out += `    \n`;
            }
            out += `    ${template.datamodel.dataType} *data = &${template.handle.name}->data;\n`;
            out += `    exos_datamodel_handle_t *${template.datamodel.varName} = &${template.handle.name}->${template.datamodel.varName};\n`;
            out += `    //the user context of the datamodel points to the ${template.datamodel.structName}Cyclic instance\n`;
            out += `    ${template.datamodel.varName}->user_context = inst; //set it cyclically in case the program using the FUB is retransferred\n`;
            out += `    ${template.datamodel.varName}->user_tag = 0; //user defined\n`;
            out += `    //handle online download of the library\n`;
            out += `    if(NULL != ${template.datamodel.varName}->datamodel_event_callback && ${template.datamodel.varName}->datamodel_event_callback != datamodelEvent)\n`;
            out += `    {\n`;
            out += `        ${template.datamodel.varName}->datamodel_event_callback = datamodelEvent;\n`;
            out += `        exos_log_delete(&${template.handle.name}->${template.logname});\n`;
            out += `        exos_log_init(&${template.handle.name}->${template.logname}, "${template.aliasName}");\n`;
            out += `    }\n\n`;
        
            for (let dataset of template.datasets) {
                if (dataset.isSub || dataset.isPub) {
                    out += `    exos_dataset_handle_t *${dataset.varName} = &${template.handle.name}->${dataset.varName};\n`;
                    out += `    ${dataset.varName}->user_context = NULL; //user defined\n`;
                    out += `    ${dataset.varName}->user_tag = 0; //user defined\n`;
                    out += `    //handle online download of the library\n`;
                    out += `    if(NULL != ${dataset.varName}->dataset_event_callback && ${dataset.varName}->dataset_event_callback != datasetEvent)\n`;
                    out += `    {\n`;
                    out += `        ${dataset.varName}->dataset_event_callback = datasetEvent;\n`;
                    out += `    }\n\n`;
                }
            }
            out += `    //unregister on disable\n`;
            out += `    if (inst->_state && !inst->Enable)\n`;
            out += `    {\n`;
            out += `        inst->_state = 255;\n`;
            out += `    }\n\n`;
            out += `    switch (inst->_state)\n`;
            out += `    {\n`;
            out += `    case 0:\n`;
            out += `        inst->Disconnected = 1;\n`;
            out += `        inst->Connected = 0;\n`;
            out += `        inst->Operational = 0;\n`;
            out += `        inst->Aborted = 0;\n\n`;
            out += `        if (inst->Enable)\n`;
            out += `        {\n`;
            out += `            inst->_state = 10;\n`;
            out += `        }\n`;
            out += `        break;\n\n`;
            out += `    case 10:\n`;
            out += `        inst->_state = 100;\n`;
            out += `\n        SUCCESS("starting ${template.datamodel.structName} application..");\n\n`;
            out += `        //connect the datamodel, then the datasets\n`;
            out += `        EXOS_ASSERT_OK(exos_datamodel_connect_${template.datamodel.structName.toLowerCase()}(${template.datamodel.varName}, datamodelEvent));\n`;
            
            for (let dataset of template.datasets) {
                if (dataset.isSub) {
                    if (dataset.isPub) {
                        out += `        EXOS_ASSERT_OK(exos_dataset_connect(${dataset.varName}, EXOS_DATASET_PUBLISH + EXOS_DATASET_SUBSCRIBE, datasetEvent));\n`;
                    }
                    else {
                        out += `        EXOS_ASSERT_OK(exos_dataset_connect(${dataset.varName}, EXOS_DATASET_SUBSCRIBE, datasetEvent));\n`;
                    }
                }
                else if (dataset.isPub) {
                    out += `        EXOS_ASSERT_OK(exos_dataset_connect(${dataset.varName}, EXOS_DATASET_PUBLISH, datasetEvent));\n`;
                }   
            }
            out += `\n        inst->Active = true;\n`;
            out += `        break;\n\n`;
        
            out += `    case 100:\n`;
            out += `    case 101:\n`;
            out += `        if (inst->Start)\n`;
            out += `        {\n`;
            out += `            if (inst->_state == 100)\n`;
            out += `            {\n`;
            out += `                EXOS_ASSERT_OK(exos_datamodel_set_operational(${template.datamodel.varName}));\n`;
            out += `                inst->_state = 101;\n`;
            out += `            }\n`;
            out += `        }\n`;
            out += `        else\n`;
            out += `        {\n`;
            out += `            inst->_state = 100;\n`;
            out += `        }\n\n`;
            out += `        EXOS_ASSERT_OK(exos_datamodel_process(${template.datamodel.varName}));\n`;
            out += `        //put your cyclic code here!\n\n`;
            for (let dataset of template.datasets) {
                if (dataset.isPub) {        
                    if(Datamodel.isScalarType(dataset) && (dataset.arraySize == 0)) {
                        out += `        //publish the ${dataset.varName} dataset as soon as there are changes\n`;
                        out += `        if (inst->p${template.datamodel.structName}->${dataset.structName} != data->${dataset.structName})\n`;
                        out += `        {\n`;
                        out += `            data->${dataset.structName} = inst->p${template.datamodel.structName}->${dataset.structName};\n`;
                        out += `            exos_dataset_publish(${dataset.varName});\n`;
                        out += `        }\n`;
                    } 
                    else {
                        out += `        //publish the ${dataset.varName} dataset as soon as there are changes\n`;
                        out += `        if (0 != memcmp(&inst->p${template.datamodel.structName}->${dataset.structName}, &data->${dataset.structName}, sizeof(data->${dataset.structName})))\n`;
                        out += `        {\n`;
                        out += `            memcpy(&data->${dataset.structName}, &inst->p${template.datamodel.structName}->${dataset.structName}, sizeof(data->${dataset.structName}));\n`;
                        out += `            exos_dataset_publish(${dataset.varName});\n`;
                        out += `        }\n`;
                    }
                }   
            }
            out += `\n        break;\n\n`;
            out += `    case 255:\n`;
            out += `        //disconnect the datamodel\n`;
            out += `        EXOS_ASSERT_OK(exos_datamodel_disconnect(${template.datamodel.varName}));\n\n`;
            out += `        inst->Active = false;\n`;
            out += `        inst->_state = 254;\n`;
            out += `        //no break\n\n`;
            out += `    case 254:\n`;
            out += `        if (!inst->Enable)\n`;
            out += `            inst->_state = 0;\n`;
            out += `        break;\n`;
            out += `    }\n\n`;
            out += `    exos_log_process(&${template.handle.name}->${template.logname});\n\n`;
            out += `}\n\n`;
        
            return out;
        }
        
        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateExit(template) {
            let out = "";
            out += `_BUR_PUBLIC void ${template.datamodel.structName}Exit(struct ${template.datamodel.structName}Exit *inst)\n{\n`;
        
            out += `    ${template.handle.dataType} *${template.handle.name} = (${template.handle.dataType} *)inst->Handle;\n\n`;
            out += `    if (NULL == handle)\n`;
            out += `    {\n`;
            out += `        ERROR("${template.datamodel.structName}Exit: NULL handle, cannot delete resources");\n`;
            out += `        return;\n`;
            out += `    }\n`;
            out += `    if ((void *)handle != handle->self)\n`;
            out += `    {\n`;
            out += `        ERROR("${template.datamodel.structName}Exit: invalid handle, cannot delete resources");\n`;
            out += `        return;\n`;
            out += `    }\n\n`;
        
            out += `    exos_datamodel_handle_t *${template.datamodel.varName} = &${template.handle.name}->${template.datamodel.varName};\n`;
            out += `\n`;
            out += `    EXOS_ASSERT_OK(exos_datamodel_delete(${template.datamodel.varName}));\n\n`;

            if(template.datamodel.hasBuffered)
            {
                template.datasets.filter((dataset) => {
                    if(dataset.isBuffered)
                    {
                        out += `    //delete buffer(s):\n`;
                        out += `    ${template.datamodel.structName}BufferHandle_t *bufferHandle =  (${template.datamodel.structName}BufferHandle_t *)&handle->bufferedsample_buffer;\n`;
                        out += `    if (NULL == bufferHandle)\n`;
                        out += `    {\n`;
                        out += `        ERROR("${template.datamodel.structName}Exit: NULL bufferhandle, cannot delete resources");\n`;
                        out += `        return;\n`;
                        out += `    }\n`;
                        out += `    if ((void *)bufferHandle != bufferHandle->self)\n`;
                        out += `    {\n`;
                        out += `        ERROR("${template.datamodel.structName}Exit: invalid bufferhandle, cannot delete resources");\n`;
                        out += `        return;\n`;
                        out += `    }\n\n`;

                    }
                });
            }
            
            out += `    //finish with deleting the log\n`;
            out += `    exos_log_delete(&${template.handle.name}->${template.logname});\n`;
        
            out += `    //free the allocated handle\n`;
            out += `    TMP_free(sizeof(${template.handle.dataType}), (void *)handle);\n`;
            out += `}\n\n`;
        
            return out;
        }        

        let out = "";
    
        out += generateIncludes(this.template);
    
        out += generateHandle(this.template);
    
        out += generateCallbacks(this.template);
    
        out += generateInit(this.template);
    
        out += generateCyclic(this.template);
    
        out += generateExit(this.template);
    
        return out;
    }

    /**
     * @returns {string} `{ProgramName}.var`: varaible declaration for the ST program
     */
    _generateIECProgramVar() {
        function generateIECProgramVar(template) {
            let out = "";
        
            out += `VAR\n`;
            out += `    ${template.datamodel.structName}Init_0 : ${template.datamodel.structName}Init;\n`;
            out += `    ${template.datamodel.structName}Cyclic_0 : ${template.datamodel.structName}Cyclic;\n`;
            out += `    ${template.datamodel.structName}Exit_0 : ${template.datamodel.structName}Exit;\n`;
            if(template.datamodel.hasBuffered)
            {
                out += `    ${template.datamodel.structName}BufferUpdate_0 : ${template.datamodel.structName}BufferUpdate;\n`;
            }
            out += `    ${template.datamodel.structName}_0 : ${template.datamodel.structName};\n`;
            out += `    ExComponentInfo_0 : ExComponentInfo;\n`;
            out += `    ExDatamodelInfo_0 : ExDatamodelInfo;\n`;
            out += `END_VAR\n`;
        
            return out;
        }
        return generateIECProgramVar(this.template);
    }

    /**
     * @returns {string} `{ProgramName}.st`: application implementation code in ST
     */
    _generateIECProgramST() {
        /**
         * @param {ApplicationTemplate} template 
         */
        function generateIECProgramST(template) {
            let out = "";
        
            out += `\n`;
            out += `PROGRAM _INIT\n`;
            out += `\n`;
            out += `    ${template.datamodel.structName}Init_0();\n`;
            out += `\n`;
            out += `END_PROGRAM\n`;
            out += `\n`;
            out += `PROGRAM _CYCLIC\n`;
            out += `    \n`;
            out += `    //Auto connect:\n`;
            out += `    //${template.datamodel.structName}Cyclic_0.Enable := ExComponentInfo_0.Operational; // Component has been deployed and started up successfully\n`;
            out += `    \n`;
            out += `    ${template.datamodel.structName}Cyclic_0(Handle := ${template.datamodel.structName}Init_0.Handle, p${template.datamodel.structName} := ADR(${template.datamodel.structName}_0));\n`;
            if(template.datamodel.hasBuffered)
            {
                template.datasets.filter((dataset) => {
                    if(dataset.isBuffered)
                    {
                        out += `    \n`;
                        out += `    ${template.datamodel.structName}BufferUpdate_0.BufferHandle := ${template.datamodel.structName}Init_0.${dataset.structName}BufferHandle;\n`;
                        out += `    UpdateBufferSampleSingle;\n`;
                        out += `    //UpdateBufferSampleAll;\n`;
                    }
                });
            }
            out += `    \n`;
            out += `    ExComponentInfo_0(ExTargetLink := ADR(${template.targetName}), ExComponentLink := ADR(${template.aliasName}), Enable := TRUE);\n`;
            out += `    \n`;
            out += `    ExDatamodelInfo_0(ExTargetLink := ADR(${template.targetName}), Enable := TRUE, InstanceName := '${template.datamodelInstanceName}');\n`;
            out += `    \n`;
            out += `END_PROGRAM\n`;
            out += `\n`;
            out += `PROGRAM _EXIT\n`;
            out += `\n`;
            out += `    ${template.datamodel.structName}Exit_0(Handle := ${template.datamodel.structName}Init_0.Handle);\n`;
            out += `\n`;
            out += `END_PROGRAM\n`;
        
            return out;
        }
        return generateIECProgramST(this.template);
    }

    /**
     * @returns {string} `UpdateBufferActions.st`: buffer action implementation code in ST
     */
     _generateIECBufferedActionST() {
        /**
         * @param {ApplicationTemplate} template 
         */
        function generateIECBufferedActionST(template) {
            let out = "";

            out += `\n`;
            out += `(*Read a single change on ${template.datamodel.structName}BufferUpdate_0.BufferHandle per scan*)\n`;
            out += `ACTION UpdateBufferSampleSingle:\n`;
            out += `    ${template.datamodel.structName}BufferUpdate_0(UpdateDataset := TRUE);\n`;
            out += `    \n`;
            out += `    IF ${template.datamodel.structName}BufferUpdate_0.DatasetUpdated THEN\n`;
            out += `    \n`;
            out += `        (* Your code here *)\n`;
            out += `        \n`;
            out += `    END_IF\n`;
            out += `END_ACTION\n`;
            out += `\n`;
            out += `\n`;
            out += `(*Read all changes on ${template.datamodel.structName}BufferUpdate_0.BufferHandle within a single scan*)\n`;
            out += `ACTION UpdateBufferSampleAll:\n`;
            out += `    \n`;
            out += `    ${template.datamodel.structName}BufferUpdate_0(UpdateDataset := FALSE); // update PendingUpdates without requesting UpdateDataset\n`;
            out += `    \n`;
            out += `    WHILE ${template.datamodel.structName}BufferUpdate_0.PendingUpdates > 0 DO\n`;
            out += `    \n`;
            out += `        ${template.datamodel.structName}BufferUpdate_0(UpdateDataset := TRUE);\n`;
            out += `        \n`;
            out += `        (* Your code here *)\n`;
            out += `        \n`;
            out += `    END_WHILE\n`;
            out += `END_ACTION\n`;
            return out;
        }
        return generateIECBufferedActionST(this.template);
    }
    
    /**
     * @returns {string} `{LibraryName}.fun`: function block declaration for the AR library (needs to have the same name as the library itself)
     */
    _generateFun() {
        /**
         * @param {ApplicationTemplate} template 
         * @returns {string}
         */
        function generateFun(template) {
            let out = "";

            out += `FUNCTION_BLOCK ${template.datamodel.structName}Init\n`;
            out += `	VAR_OUTPUT\n`;
            out += `		Handle : UDINT;\n`;
            template.datasets.filter((dataset) => {
                if(dataset.isBuffered)
                {
                    out += `		${dataset.structName}BufferHandle : UDINT; (*Initialized BufferHandle for bufferedSample. Bufferhandle is linked to data in Cyclic*)\n`;
                }
            });
            out += `	END_VAR\n`;
            out += `	VAR\n`;
            out += `		_state : USINT;\n`;
            out += `	END_VAR\n`;
            out += `END_FUNCTION_BLOCK\n`;
            out += `\n`;
        
            out += `FUNCTION_BLOCK ${template.datamodel.structName}Cyclic\n`;
            out += `	VAR_INPUT\n`;
            out += `		Enable : BOOL;\n`;
            out += `		Handle : UDINT;\n`;
            out += `		Start : BOOL;\n`;
            out += `		p${template.datamodel.structName} : REFERENCE TO ${template.datamodel.structName};\n`;
            out += `	END_VAR\n`;
            out += `	VAR_OUTPUT\n`;
            out += `		Active : BOOL;\n`;
            out += `		Error : BOOL;\n`;
            out += `		Disconnected : BOOL;\n`;
            out += `		Connected : BOOL;\n`;
            out += `		Operational : BOOL;\n`;
            out += `		Aborted : BOOL;\n`;
            out += `	END_VAR\n`;
            out += `	VAR\n`;
            out += `		_state : USINT;\n`;
            out += `	END_VAR\n`;
            out += `END_FUNCTION_BLOCK\n`;
            out += `\n`;
        
            out += `FUNCTION_BLOCK ${template.datamodel.structName}Exit\n`;
            out += `	VAR_INPUT\n`;
            out += `		Handle : UDINT;\n`;
            out += `	END_VAR\n`;
            out += `	VAR\n`;
            out += `		_state : USINT;\n`;
            out += `	END_VAR\n`;
            out += `END_FUNCTION_BLOCK\n`;

            if(template.datamodel.hasBuffered)
            {
                out += `\n`;
                out += `FUNCTION_BLOCK ${template.datamodel.structName}BufferUpdate (*Must be called in same TC as Cyclic. This function block finalizes in one scan and can be used for several different buffers*)\n`;
                out += `	VAR_INPUT\n`;
                out += `		BufferHandle : UDINT; (*Set this to the correct bufferhandle (from Init function block) before calling action UpdateBufferSampleSingle or UpdateBufferSampleAll*)\n`;
                out += `		UpdateDataset : BOOL; (*Set to TRUE to update dataset, PendingUpdates and OverflowErrors. Set to FALSE to only update PendingUpdates and OverflowErrors for BufferHandle without updating dataset*)\n`;
                out += `	END_VAR\n`;
                out += `	VAR_OUTPUT\n`;
                out += `		Error : BOOL; (*Set in case BufferHandle is invalid*)\n`;
                out += `		DatasetUpdated : BOOL; (*Set if dataset was updated successfully. Only when UpdateDataset is set to TRUE and there is something in the buffer*)\n`;
                out += `		PendingUpdates : UDINT; (*Number of datasets waiting in the buffer*)\n`;
                out += `		OverflowErrors : UDINT; (*Number of overflow errors which occurs when the buffer is full and new data arrives from GPOS*)\n`;
                out += `	END_VAR\n`;
                out += `END_FUNCTION_BLOCK\n`;
            }

        
            return out;
        }
        return generateFun(this.template);
    }
}

module.exports = {TemplateARDynamic};