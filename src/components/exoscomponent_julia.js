/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { TemplateLinuxJulia } = require('./templates/linux/template_linux_julia');
const { ExosComponentAR, ExosComponentARUpdate } = require('./exoscomponent_ar');
const { EXOS_COMPONENT_VERSION } = require("./exoscomponent");
const { ExosPkg } = require('../exospkg');
const { TemplateLinuxBuild } = require('./templates/linux/template_linux_build');

const path = require('path');

/**
 * @typedef {Object} ExosComponentJuliaOptions
 * @property {string} packaging  package format: `deb` | `none` - default: `none`
 * @property {string} destinationDirectory destination for the packaging. default: `/home/user/{typeName.toLowerCase()}`
 * @property {string} templateAR template used for AR: `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api`
 * @property {string} templateLinux template used for Linux: `c-static` | `cpp` | `c-api` - default: `c-api`
 */
class ExosComponentJulia extends ExosComponentAR {

    /**
     * @type {TemplateLinuxJulia}
     */
    _templateJulia;

    /**
     * @type {ExosComponentJuliaOptions}
     */
    _options;

    /**
     * Create a Julia Component template
     * 
     * @param {string} fileName 
     * @param {string} typeName 
     * @param {ExosComponentJuliaOptions} options
     */
    constructor(fileName, typeName, options) {
        
        let _options = {packaging: `deb`, destinationDirectory: `/home/user/${typeName.toLowerCase()}`, templateAR: "c-api", templateLinux: "julia"};

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

        // fourth argument useJulia true
        super(fileName, typeName, _options.templateAR, true);
        this._options = _options;

        if(this._options.packaging == "none") {
            this._options.destinationDirectory = undefined;
        }
        
        this._templateBuild = new TemplateLinuxBuild(typeName);
        this._templateJulia = new TemplateLinuxJulia(this._datamodel);

        
            
    }

    makeComponent(location) {

        this._templateBuild.options.executable.enable = true;
        this._templateBuild.options.julia.enable = true;
        this._templateBuild.options.executable.sourceFiles = [this._templateJulia.mainSource.name]

        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateJulia.mainSource);
                             this._linuxPackage.addNewBuildFileObj(this._linuxBuild);
        if (this._datamodel != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._datamodel.sourceFile.name)
        }
        if (this._templateJulia.datamodelSource != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._templateJulia.datamodelSource.name)
        }
        if (this._templateJulia.loggerSource != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._templateJulia.loggerSource.name)
        }
        if(this._options.packaging == "deb") {
            this._templateBuild.options.debPackage.enable = true;
            this._templateBuild.options.debPackage.destination = this._options.destinationDirectory;
            this._linuxPackage.addExistingTransferDebFile(this._templateBuild.options.debPackage.fileName, this._templateBuild.options.debPackage.packageName, `${this._typeName} debian package`);
            this._exospackage.exospkg.addService("Runtime", `julia ${this._templateBuild.options.debPackage.destination}/${this._templateJulia.mainSource.name}`, this._templateBuild.options.debPackage.destination);
        }
        else {
            this._templateBuild.options.debPackage.enable = false;
            this._exospackage.exospkg.addService("Runtime", `julia ${this._templateJulia.mainSource.name}`);
            this._exospackage.exospkg.addService("Startup", `chmod +x ${this._templateBuild.options.executable.executableName}`);
        }

        this._templateBuild.makeBuildFiles();

        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateBuild.CMakeLists);
        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateBuild.buildScript);

        if (this._templateAR != undefined)
            this._exospackage.exospkg.addDatamodelInstance(`${this._templateAR.template.datamodelInstanceName}`);

        this._exospackage.exospkg.setComponentGenerator("ExosComponentJulia", EXOS_COMPONENT_VERSION, []);
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

class ExosComponentJuliaUpdate extends ExosComponentARUpdate {

    /**
     * @type {TemplateLinuxJulia}
     */
    _templateJulia;

    /**
     * @type {ExosComponentJuliaOptions}
     */
    _options;

    /**
     * Update class for Linux Julia applications, only updates the sourcefiles of the datamodel-wrappers
     * @param {string} exospkgFileName absolute path to the .exospkg file stored on disk
     * @param {boolean} updateAll update main application sources as well
     */
    constructor(exospkgFileName, updateAll) {
        super(exospkgFileName, updateAll);

        if(this._exosPkgParseResults.componentFound == true && this._exosPkgParseResults.componentErrors.length == 0) {
            if(this._exospackage.exospkg.componentOptions.templateLinux) {
                this._options = {packaging: "", destinationDirectory: "", templateAR: "", templateLinux: this._exospackage.exospkg.componentOptions.templateLinux};

                this._templateJulia = new TemplateLinuxJulia(this._datamodel);
                this._linuxPackage.addNewFileObj(this._templateJulia.staticLibraryHeader);
                this._linuxPackage.addNewFileObj(this._templateJulia.staticLibrarySource);
                if(updateAll) {
                    this._linuxPackage.addNewFileObj(this._templateJulia.mainSource);
                }

                }

            }
            else {
                this._exosPkgParseResults.componentErrors.push("ExosComponentJuliaUpdate: missing option: templateLinux");
            }
        }
    }


if (require.main === module) {

    process.stdout.write(`exOS Julia Template\n`);

    if (process.argv.length > 3) {

        let fileName = process.argv[2];
        let structName = process.argv[3];

        let template = new ExosComponentJulia(fileName, structName, {templateAR:"c-api", templateLinux:"c-api"});
        let outDir = path.join(__dirname,path.dirname(fileName));

        process.stdout.write(`Writing ${structName} to folder: ${outDir}\n`);
        
        template.makeComponent(outDir);

        let updater = new ExosComponentJuliaUpdate(path.join(outDir,structName,`${structName}.exospkg`),true);
        let results  = updater.updateComponent();
        console.log(results);

    }
    else {
        process.stderr.write("usage: ./exoscomponent_c.js <filename.typ> <structname>\n");
    }
}

module.exports = {ExosComponentJulia, ExosComponentJuliaUpdate};