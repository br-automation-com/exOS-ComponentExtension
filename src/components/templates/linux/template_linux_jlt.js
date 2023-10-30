
const { Template, ApplicationTemplate } = require('../template')
const { Datamodel, GeneratedFileObj } = require('../../../datamodel');

class TemplateLinuxJuliet extends Template {
    /**
     * main sourcefile for the application
     * @type {GeneratedFileObj}
     */
    mainSource;

    /**
     * {@linkcode TemplateLinuxJuliet} Generates code for a Linux Juliet
     *
     * - {@linkcode mainSource} main sourcefile for the application
     *
     * @param {Datamodel} datamodel
     */

    constructor(datamodel) {
        super(datamodel, true);
        if (datamodel == undefined) {
            this.mainSource = {name:`main.jlt`, contents:this._generateSourceNoDatamodel(false), description:"Linux application"};
        }
        else {
            this.mainSource = {name:`${this.datamodel.typeName.toLowerCase()}.jlt`, contents:this._generateSource(), description:"Linux application"};
        }
    }
    /**
     * @returns `{main}.c`: main sourcefile for the application when creating without datamodel
     */
    _generateSourceNoDatamodel() {
        let out = ``;

        out += `using exoscomlib\n\n`;
        out += `function main()\n`;
        out += `end\n\n`;

        return out;
    }


    /**
     * @returns `{main}.jlt`: main sourcefile for the application
     */
    _generateSource() {
        /**
         * @param {ApplicationTemplate} template
         * @returns {string}
         */
        function _generateJulietMain(template) {
            let out = ``;

            out += `    init_data_model_handle("${template.datamodelInstanceName}", "${template.aliasName}")\n\n`;

            for (let dataset of template.datasets) {
                if (dataset.isSub || dataset.isPub) {
                    out += `    var ${dataset.structName} = `;
                    if(dataset.type=="struct"){
                        out += `exos_${template.datamodel.structName.toLowerCase()}.`;
                    } else {
                        out  += `exoscomlib.`;
                    }
                    if (dataset.dataType=="STRING"){
                        out += `init_data_set_handle("${dataset.structName}", data.${dataset.structName}, ${dataset.stringLength})\n`;
                    } else {
                        out += `init_data_set_handle("${dataset.structName}", data.${dataset.structName})\n`;
                    }
                }
            }

            out += `\n`;
            let varName = template.datamodel.varName;
            // varName doesnt always ends with _datamodel
            if (!varName.endsWith("_datamodel")) {
                varName += "_datamodel";
            }

            out += `    connect_${varName}()\n\n`;

            for (let dataset of template.datasets) {
                if (dataset.isSub) {
                    if (dataset.isPub) {
                        out += `    connect_data_set(${dataset.structName}, datasetEvent, EXOS_DATASET_SUBSCRIBE + EXOS_DATASET_PUBLISH)\n`;
                    }
                    else {
                        out += `    connect_data_set(${dataset.structName}, datasetEvent, EXOS_DATASET_SUBSCRIBE)\n`;
                    }
                }
                else if (dataset.isPub) {
                    out += `    connect_data_set(${dataset.structName}, datasetEvent, EXOS_DATASET_PUBLISH)\n`;
                }
            }
            out += `\n`;

            out += `    var termination = 0\n`;
            out += `    while termination == 0\n`;
            out += `        termination = process_data()\n\n`;
            out += `        # put your cyclic code here!\n\n`;
            out += `    end\n\n`;
            out += `    delete_data_model_handle()\n`;
            return out;
        }
        /**
         * @param {ApplicationTemplate} template
         * @returns {string}
         */
        function _generateJulietCallback(template) {

            function _generateDecodes(dataset){
                let type, size;
                if(dataset.type == "variable"){
                    ({type, size} = Datamodel.convertPlcTypeToJulietTypes(dataset.dataType));
                    if (type == "String"){
                        size = dataset.stringLength;
                    }
                    if(dataset.arraySize > 0 ){
                        type = `Array{${type}, 1}`;
                        size = dataset.arraySize * size - 1;
                        if (type.includes("String")) {
                            type += `, ${dataset.stringLength}`;
                        }
                    } else {
                        size -= 1;
                    }
                } else if(dataset.type == "struct"){
                    type = `${Datamodel.convertPlcType(dataset.dataType)}` ;
                    size = `sizeOf(${type})`;
                    if(dataset.arraySize > 0 ){
                        type = `Array{${type}, 1}`;
                        size = `(${dataset.arraySize} * ${size} - 1)`;
                    } else {
                        size = `${size} - 1`;
                    }
                } else if(dataset.type == "enum"){
                    type = `enum`;
                    size = 1;
                }
                return {type, size};
            }
            let out = ``;
            out += `function datasetEvent(dataset::String, event_type::Int64, state::Int64, bytearray::Array{UInt8, 1})\n`;
            out += `    if event_type == EXOS_DATASET_EVENT_UPDATED\n`;

            let atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isSub) {
                    if (atleastone) {
                        out += `        else`;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    let {type, size} = _generateDecodes(dataset);
                    out += `if dataset == "${dataset.structName}"\n`;
                    out += `            var ${dataset.varName} = `;
                    if(dataset.type=="struct"){
                        out += `exos_${template.datamodel.structName.toLowerCase()}.`;
                    } else {
                        out  += `exoscomlib.`;
                    }
                    out += `decode_dataset(bytearray, ${type})\n`;
                }
            }
            out += `        end\n`;

            out += `    elseif event_type == EXOS_DATASET_EVENT_PUBLISHED\n`;
            atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isPub) {
                    if (atleastone) {
                        out += `        else`;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    let {type, size} = _generateDecodes(dataset);
                    out += `if dataset == "${dataset.structName}"\n`;
                    out += `            var ${dataset.varName} = `;
                    if(dataset.type=="struct"){
                        out += `exos_${template.datamodel.structName.toLowerCase()}.`;
                    } else {
                        out  += `exoscomlib.`;
                    }
                    out += `decode_dataset(bytearray, ${type})\n`;
                }
            };
            out += `        end\n`;

            out += `    elseif event_type == EXOS_DATASET_EVENT_DELIVERED\n`;
            atleastone = false;
            for (let dataset of template.datasets) {
                if (dataset.isPub) {
                    if (atleastone) {
                        out += `        else`;
                    }
                    else {
                        out += `        `;
                        atleastone = true;
                    }
                    let {type, size} = _generateDecodes(dataset);
                    out += `if dataset == "${dataset.structName}"\n`;
                    out += `            var ${dataset.varName} = `;
                    if(dataset.type=="struct"){
                        out += `exos_${template.datamodel.structName.toLowerCase()}.`;
                    } else {
                        out  += `exoscomlib.`;
                    }
                    out += `decode_dataset(bytearray, ${type})\n`;
                }
            }
            out += `        end\n`;

            out += `    elseif event_type == EXOS_DATASET_EVENT_CONNECTION_CHANGED\n`;
            out += `        if state == EXOS_STATE_DISCONNECTED\n`;
            out += `        elseif state == EXOS_STATE_CONNECTED\n`;
            out += `        elseif state == EXOS_STATE_OPERATIONAL\n`;
            out += `        elseif state ==  EXOS_STATE_ABORTED\n`;
            out += `        end\n`;
            out += `    end\n`;
            out += `    nothing\n`;
            out += `end\n\n`;

            return out;
        }

        let out = ``;

        out += `using exoscomlib\n`;
        out += `using Main.exos_${this.template.datamodel.structName.toLowerCase()}\n\n`;

        out += _generateJulietCallback(this.template);

        out += `function main()\n`;
        out += _generateJulietMain(this.template);
        out += `end\n`;

        return out;
    }
}
module.exports = {TemplateLinuxJuliet};