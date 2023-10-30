/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// const { TemplateLinuxC } = require('./templates/linux/template_linux_c');
// const { TemplateLinuxStaticCLib } = require('./templates/linux/template_linux_static_c_lib');
// const { TemplateLinuxCpp } = require('./templates/linux/template_linux_cpp');
const { TemplateLinuxJuliet } = require('./templates/linux/template_linux_jlt');
const { BuildOptions } = require('./templates/linux/template_linux_build');
const { ExosComponentAR, ExosComponentARUpdate } = require('./exoscomponent_ar');
const { TemplateLinuxBuild } = require('./templates/linux/template_linux_build');
const { EXOS_COMPONENT_VERSION } = require("./exoscomponent");
const { ExosPkg } = require('../exospkg');

const path = require('path');

/**
 * @typedef {Object} ExosComponentJulietOptions
 * @property {string} packaging  package format: `deb` | `none` - default: `none`
 * @property {string} destinationDirectory destination for the packaging. default: `/home/user/{typeName.toLowerCase()}`
 * @property {string} templateAR template used for AR: `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api`
 */

class ExosComponentJuliet extends ExosComponentAR {

    /**
     * @type {TemplateLinuxJuliet}
     */
    _templateJuliet;

        /**
     * @type {ExosComponentJulietOptions}
     */
    _options;

        /**
     * Create a Juliet Component template
     * 
     * @param {string} fileName 
     * @param {string} typeName 
     * @param {ExosComponentJulietOptions} options 
     */
    constructor(fileName, typeName, options) {
        let _options = {packaging:"deb", destinationDirectory: `/home/user/${typeName.toLowerCase()}`, templateAR: "c-static"};

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

        // fourth argument se useJuliet true
        super(fileName, typeName, _options.templateAR, true);
        this._options = _options;
    

        if(this._options.packaging == "none") {
            this._options.destinationDirectory = undefined;
        }
        this._templateBuild = new TemplateLinuxBuild(typeName);
        this._templateJuliet = new TemplateLinuxJuliet(this._datamodel);
    }
    
    makeComponent(location) {
        this._linuxPackage.addNewBuildFileObj(this._linuxBuild, this._templateJuliet.mainSource);

        this._templateBuild.options.juliet.enable = true;
        this._templateBuild.options.executable.sourceFiles = [this._templateJuliet.mainSource.name]
        if (this._datamodel != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._datamodel.sourceFile.name)
        }
        if (this._templateJuliet.datamodelSource != undefined) {
            this._templateBuild.options.executable.sourceFiles.push(this._templateJuliet.datamodelSource.name)
        }

        if(this._options.packaging == "deb") {
            this._templateBuild.options.debPackage.enable = true;
            this._templateBuild.options.debPackage.destination = this._options.destinationDirectory;
            this._exospackage.exospkg.addService("Runtime", `./../language/rvm_build/bin/rvm ${this._templateBuild.options.executable.executableName}.rvm`, this._templateBuild.options.debPackage.destination);
        }
        else {
            this._templateBuild.options.debPackage.enable = false;
            this._exospackage.exospkg.addService("Startup", `chmod +x ${this._templateBuild.options.executable.executableName}.rvm`);
            this._exospackage.exospkg.addService("Runtime", `./../language/rvm_build/bin/rvm ${this._templateBuild.options.executable.executableName}.rvm`);
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

class ExosComponentJulietUpdate extends ExosComponentARUpdate {
    /**
     * Update class for Julia applications, only updates the sourcefile of the datamodel-wrapper
     * @param {string} exospkgFileName absolute path to the .exospkg file stored on disk
     * @param {boolean} updateAll update main application sources as well
     */
    constructor(exospkgFileName, updateAll) {
        super(exospkgFileName, updateAll);
     
        if(this._exosPkgParseResults.componentFound == true && this._exosPkgParseResults.componentErrors.length == 0) {
            this._templateJuliet = new TemplateLinuxJuliet(this._datamodel);

            if(updateAll) {
                this._linuxPackage.addNewFileObj(this._templateJuliet.mainSource);
            }
        }
    }
}       

if (require.main === module) {
    process.stdout.write(`exOS Juliet Template\n`);

    if (process.argv.length > 3) {

        let fileName = process.argv[2];
        let structName = process.argv[3];

        let template = new ExosComponentJuliet(fileName, structName);
        let outDir = path.join(__dirname,path.dirname(fileName));

        process.stdout.write(`Writing ${structName} to folder: ${outDir}\n`);
        
        template.makeComponent(outDir);

        let updater = new ExosComponentJulietUpdate(path.join(outDir,structName,`${structName}.exospkg`),true);
        let results  = updater.updateComponent();
        console.log(results);

    }
    else {
        process.stderr.write("usage: ./exoscomponent_juliet.js <filename.typ> <structname>\n");
    }
}
module.exports = {ExosComponentJuliet, ExosComponentJulietUpdate}