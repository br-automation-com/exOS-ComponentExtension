/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { TemplateLinuxC } = require('./templates/linux/template_linux_c');
const { TemplateLinuxStaticCLib } = require('./templates/linux/template_linux_static_c_lib');
const { TemplateLinuxCpp } = require('./templates/linux/template_linux_cpp');
const { BuildOptions } = require('./templates/linux/template_linux_build');
const { ExosComponentAR, ExosComponentARUpdate } = require('./exoscomponent_ar');
const { EXOS_COMPONENT_VERSION } = require("./exoscomponent");
const { ExosPkg } = require('../exospkg');

const path = require('path');

/**
 * @typedef {Object} ExosComponentCOptions
 * @property {string} packaging  package format: `deb` | `none` - default: `none`
 * @property {string} destinationDirectory destination for the packaging. default: `/home/user/{typeName.toLowerCase()}`
 * @property {string} templateAR template used for AR: `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api`
 * @property {string} templateLinux template used for Linux: `c-static` | `cpp` | `c-api` - default: `c-api`
 */
class ExosComponentC extends ExosComponentAR {

    /**
     * @type {TemplateLinuxStaticCLib | TemplateLinuxCpp | TemplateLinuxC}
     */
    _templateLinux;

    /**
     * @type {ExosComponentCOptions}
     */
    _options;

    /**
     * Create a C/C++ Component template
     * 
     * @param {string} fileName 
     * @param {string} typeName 
     * @param {ExosComponentCOptions} options
     */
    constructor(fileName, typeName, options) {
        
        let _options = {packaging: `none`, destinationDirectory: `/home/user/${typeName.toLowerCase()}`, templateAR: "c-api", templateLinux: "c-api"};

        if(options) {
            if(options.destinationDirectory) {
                _options.destinationDirectory = options.destinationDirectory;
            }
            if(options.templateAR) {
                _options.templateAR = options.templateAR;
            }
            if(options.templateLinux) {
                _options.templateLinux = options.templateLinux;
            }
            if(options.packaging) {
                _options.packaging = options.packaging;
            }
        }

        super(fileName, typeName, _options.templateAR);
        this._options = _options;

        if(this._options.packaging == "none") {
            this._options.destinationDirectory = undefined;
        }

        switch(this._options.templateLinux)
        {
            case "c-static":
                this._templateLinux = new TemplateLinuxStaticCLib(this._datamodel);
                break;
            case "cpp":
                this._templateLinux = new TemplateLinuxCpp(this._datamodel);
                break;
            case "c-api":
            default:
                this._templateLinux = new TemplateLinuxC(this._datamodel);
                break;
        }
        
            
    }

    makeComponent(location) {

        this._templateBuild.options.executable.enable = true;

        switch(this._options.templateLinux)
        {
            case "c-static":
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.staticLibraryHeader);
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.staticLibrarySource);
                this._templateBuild.options.executable.staticLibrary.enable = true;
                this._templateBuild.options.executable.staticLibrary.sourceFiles = [this._templateLinux.staticLibrarySource.name]
                break;
            case "cpp":
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.datasetHeader);
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.datamodelHeader);
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.datamodelSource);
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.loggerHeader);
                this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.loggerSource);
                break;
            case "c-api":
            default:
                break;
        }

        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.mainSource);
        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.termination.terminationHeader);
        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateLinux.termination.terminationSource);

        
        this._templateBuild.options.executable.sourceFiles = [this._templateLinux.termination.terminationSource.name, this._templateLinux.mainSource.name]
        if (this._datamodel != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._datamodel.sourceFile.name)
        }
        if (this._templateLinux.datamodelSource != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._templateLinux.datamodelSource.name)
        }
        if (this._templateLinux.loggerSource != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._templateLinux.loggerSource.name)
        }
        if(this._options.packaging == "deb") {
            this._templateBuild.options.debPackage.enable = true;
            this._templateBuild.options.debPackage.destination = this._options.destinationDirectory;
            this._exospackage.exospkg.addService("Runtime", `./${this._templateBuild.options.executable.executableName}`, this._templateBuild.options.debPackage.destination);
        }
        else {
            this._templateBuild.options.debPackage.enable = false;
            this._exospackage.exospkg.addService("Startup", `chmod +x ${this._templateBuild.options.executable.executableName}`);
            this._exospackage.exospkg.addService("Runtime", `./${this._templateBuild.options.executable.executableName}`);
        }

        this._templateBuild.makeBuildFiles();

        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateBuild.CMakeLists);
        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateBuild.buildScript);
        
        if(this._options.packaging == "deb") {
            this._linuxPackage.addExistingTransferDebFile(this._templateBuild.options.debPackage.fileName, this._templateBuild.options.debPackage.packageName, `${this._typeName} debian package`);
            this._linuxPackage.addExistingFile(this._templateBuild.options.executable.executableName, `${this._typeName} application`)
        }
        else {
            this._linuxPackage.addExistingTransferFile(this._templateBuild.options.executable.executableName, "Restart", `${this._typeName} application`);
        }

        if (this._templateAR != undefined)
            this._exospackage.exospkg.addDatamodelInstance(`${this._templateAR.template.datamodelInstanceName}`);

        this._exospackage.exospkg.setComponentGenerator("ExosComponentC", EXOS_COMPONENT_VERSION, []);
        this._exospackage.exospkg.addGeneratorOption("templateLinux",this._options.templateLinux);

        if(this._options.packaging == "deb") {
            this._exospackage.exospkg.addGeneratorOption("exportLinux",[this._templateBuild.options.debPackage.fileName]);
        }
        else {
            this._exospackage.exospkg.addGeneratorOption("exportLinux",[this._templateBuild.options.executable.executableName]);
        }

        if (this._templateAR == undefined)
            this._exospackage.makePackage(location);
        else
            super.makeComponent(location);
    }
}

class ExosComponentCUpdate extends ExosComponentARUpdate {

    /**
     * @type {TemplateLinuxStaticCLib | TemplateLinuxCpp | TemplateLinuxC}
     */
    _templateLinux;

    /**
     * @type {ExosComponentCOptions}
     */
    _options;

    /**
     * Update class for Linux C/C++ applications, only updates the sourcefiles of the datamodel-wrappers
     * @param {string} exospkgFileName absolute path to the .exospkg file stored on disk
     * @param {boolean} updateAll update main application sources as well
     */
    constructor(exospkgFileName, updateAll) {
        super(exospkgFileName, updateAll);
     
        if(this._exosPkgParseResults.componentFound == true && this._exosPkgParseResults.componentErrors.length == 0) {
            if(this._exospackage.exospkg.componentOptions.templateLinux) {
                this._options = {packaging: "", destinationDirectory: "", templateAR: "", templateLinux: this._exospackage.exospkg.componentOptions.templateLinux};
               
                switch(this._options.templateLinux)
                {
                    case "c-static":
                        this._templateLinux = new TemplateLinuxStaticCLib(this._datamodel);
                        this._linuxPackage.addNewFileObj(this._templateLinux.staticLibraryHeader);
                        this._linuxPackage.addNewFileObj(this._templateLinux.staticLibrarySource);
                        if(updateAll) {
                            this._linuxPackage.addNewFileObj(this._templateLinux.mainSource);
                        }
                        break;
                    case "cpp":
                        this._templateLinux = new TemplateLinuxCpp(this._datamodel);
                        this._linuxPackage.addNewFileObj(this._templateLinux.datasetHeader);
                        this._linuxPackage.addNewFileObj(this._templateLinux.datamodelHeader);
                        this._linuxPackage.addNewFileObj(this._templateLinux.datamodelSource);
                        this._linuxPackage.addNewFileObj(this._templateLinux.loggerHeader);
                        this._linuxPackage.addNewFileObj(this._templateLinux.loggerSource);
                        if(updateAll) {
                            this._linuxPackage.addNewFileObj(this._templateLinux.mainSource);
                        }
                        break;
                    case "c-api":
                        if(updateAll) {
                            this._templateLinux = new TemplateLinuxC(this._datamodel);
                            this._linuxPackage.addNewFileObj(this._templateLinux.mainSource);
                        }
                    default:
                        break;
                }
                
            }
            else {
                this._exosPkgParseResults.componentErrors.push("ExosComponentCUpdate: missing option: templateLinux");
            }
        }
    }
}

if (require.main === module) {

    process.stdout.write(`exOS C Template\n`);

    if (process.argv.length > 3) {

        let fileName = process.argv[2];
        let structName = process.argv[3];

        let template = new ExosComponentC(fileName, structName, {templateAR:"c-api", templateLinux:"cpp"});
        let outDir = path.join(__dirname,path.dirname(fileName));

        process.stdout.write(`Writing ${structName} to folder: ${outDir}\n`);
        
        template.makeComponent(outDir);

        let updater = new ExosComponentCUpdate(path.join(outDir,structName,`${structName}.exospkg`),true);
        let results  = updater.updateComponent();
        console.log(results);

    }
    else {
        process.stderr.write("usage: ./exoscomponent_c.js <filename.typ> <structname>\n");
    }
}

module.exports = {ExosComponentC, ExosComponentCUpdate};