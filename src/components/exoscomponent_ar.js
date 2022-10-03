/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { TemplateARDynamic } = require('./templates/ar/template_ar_dynamic');
const { TemplateARStaticCLib } = require('./templates/ar/template_ar_static_c_lib');
const { TemplateARCpp } = require('./templates/ar/template_ar_cpp');
const { ExosComponent, ExosComponentUpdate } = require('./exoscomponent');
const { getVSCodeDownloadUrl } = require('vscode-test/out/util');
const { window } = require('vscode');

class ExosComponentAR extends ExosComponent {

    /**
     * @type {string} `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api` 
     */
    _template;

    /**
     * @type {TemplateARStaticCLib | TemplateARCpp | TemplateARDynamic}
     */
    _templateAR;

    /**
     * 
     * @param {*} fileName 
     * @param {*} typeName 
     * @param {string} template `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api` 
     */
    constructor(fileName, typeName, template) {
        
        super(fileName, typeName, template);

        this._template = template;
        switch(this._template)
        {
            case "c-static":
                this._templateAR = new TemplateARStaticCLib(this._datamodel);
                break;
            case "cpp":
                this._templateAR = new TemplateARCpp(this._datamodel);
                break;
            case "deploy-only":
                this._templateAR = undefined;
                break;
            case "c-api":
            default:
                this._templateAR = new TemplateARDynamic(this._datamodel);
                break;
        }
    }

    makeComponent(location) {
    
        this._cLibrary.addNewFileObj(this._templateAR.libraryFun);
        this._cLibrary.addNewFileObj(this._templateAR.librarySource);
        this._cLibrary.addNewFileObj(this._templateAR.heap.heapSource);

        this._iecProgram.addNewFileObj(this._templateAR.iecProgramVar);
        this._iecProgram.addNewFileObj(this._templateAR.iecProgramST);

        switch(this._template)
        {
            case "c-static":
                if(this._datamodel.hasBuffered)
                    window.showWarningMessage("BUFFERED is defined in typ file but is not supported for c-static");
                this._cLibrary.addNewFileObj(this._templateAR.staticLibraryHeader);
                this._cLibrary.addNewFileObj(this._templateAR.staticLibrarySource);
                break;
            case "cpp":
                if(this._datamodel.hasBuffered)
                    window.showWarningMessage("BUFFERED is defined in typ file but is not supported for cpp");
                this._cLibrary.addNewFileObj(this._templateAR.datasetHeader);
                this._cLibrary.addNewFileObj(this._templateAR.datamodelHeader);
                this._cLibrary.addNewFileObj(this._templateAR.datamodelSource);
                this._cLibrary.addNewFileObj(this._templateAR.loggerHeader);
                this._cLibrary.addNewFileObj(this._templateAR.loggerSource);
                break;
            case "c-api":
                if(this._datamodel.hasBuffered)
                    this._iecProgram.addNewFileObj(this._templateAR.iecBufferedActionST);
            case "deploy-only":
            default:
                break;
        }

        this._exospackage.exospkg.addGeneratorOption("templateAR",this._template);

        super.makeComponent(location);
    }
}

class ExosComponentARUpdate extends ExosComponentUpdate {

   /**
     * @type {string} `c-static` | `cpp` | `c-api` | `deploy-only` - default: `c-api` 
     */
    _template;

    /**
     * @type {TemplateARStaticCLib | TemplateARCpp} note: not TemplateARDynamic
     */
    _templateAR;

    /**
     * @param {string} exospkgFileName absolute path to the .exospkg file stored on disk
     * @param {boolean} updateAll update main library sources as well
     */
    constructor(exospkgFileName, updateAll) {
        super(exospkgFileName);

        if(this._exosPkgParseResults.componentFound == true && this._exosPkgParseResults.componentErrors.length == 0) {
            if(this._exospackage.exospkg.componentOptions.templateAR) {
                this._template = this._exospackage.exospkg.componentOptions.templateAR;

                switch(this._template)
                {
                    case "c-static":
                        this._templateAR = new TemplateARStaticCLib(this._datamodel);
                        this._cLibrary.addNewFileObj(this._templateAR.staticLibraryHeader);
                        this._cLibrary.addNewFileObj(this._templateAR.staticLibrarySource);
                        if(updateAll == true) {
                            this._cLibrary.addNewFileObj(this._templateAR.librarySource);
                        }        
                        break;
                    case "cpp":
                        this._templateAR = new TemplateARCpp(this._datamodel);
                        this._cLibrary.addNewFileObj(this._templateAR.datasetHeader);
                        this._cLibrary.addNewFileObj(this._templateAR.datamodelHeader);
                        this._cLibrary.addNewFileObj(this._templateAR.datamodelSource);
                        this._cLibrary.addNewFileObj(this._templateAR.loggerHeader);
                        this._cLibrary.addNewFileObj(this._templateAR.loggerSource);
                        if(updateAll == true) {
                            this._cLibrary.addNewFileObj(this._templateAR.librarySource);
                        }        
                        break;
                    case "c-api":
                        if(updateAll == true) {
                            this._templateAR = new TemplateARDynamic(this._datamodel);
                            this._cLibrary.addNewFileObj(this._templateAR.librarySource);
                        }
                        break;
                    default:
                        break;
                }
            }
            else {
                this._exosPkgParseResults.componentErrors.push("ExosComponentARUpdate: missing option: templateAR");
            }
        }
    }


}

module.exports = { ExosComponentAR, ExosComponentARUpdate};