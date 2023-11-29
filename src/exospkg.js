/*
 * Copyright (C) 2021 B&R Danmark
 * All rights reserved
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

const { rootCertificates } = require('tls');
const parser = require('xml-parser');
const fs = require('fs');
const path = require('path');

const EXOSPKG_VERSION = "2.1.0";

/**
 * 
 * @typedef ExosPkgParseFileResults
 * @property {number} parseErrors number of errors occured while parsing
 * @property {string} originalVersion the original version of the file, empty if no version could be found
 * @property {boolean} fileParsed whether or not the ExosPkg class could be populated
 * @property {boolean} componentFound set if the .exospkgFile if the ComponentGenerator section is found, and the corresopinding `component..` fields have been populated
 * @property {string[]} componentErrors list of errors that occured within the different Component classes when creating components from the ComponentGenerator section
 * 
 * @typedef ExosPkgComponentGeneratorOption
 * @property {string} name component specific name for this option
 * @property {string} value value of this option
 * 
 */
 class ExosPkg {

    /**
     * User specific name of the component class that generated this package
     * @type {string}
     */
    componentClass;

    /**
     * User specific version of the component that generated this package, in the format X.X.X
     * @type {string}
     */
    componentVersion;

    /**
     * Component specific options from the generator. This is stored as a normal object for simpler access, meaning
     * if the option `{name:'height', value:'10'}` is set via {@link setComponentGenerator} or {@link addGeneratorOption},
     * then this option is retrieved via `componentOptions.height`
     * 
     * @type {object}
     */
    componentOptions;

    /**
     * Class for the .exospkg file, created within an {@link ExosPackage} and accessed via {@link ExosPackage.exospkg}
     * It contains metods to populate the contents of an .exospkg, and returns the XML contents
     * via {@link getContents} - whereas this function is called implicitly by the {@link ExosPackage.makePackage}
     * 
     * It can also be created out of a file using the {@link parseFile}
     * 
     * @example
     * //make new from ExosPackage
     * let myPackage = new ExosPackage("MyPackage");
     * myPackage.exospkg.addService("Runtime","/home/user/myexecutable");
     * myPackage.makePackage("C:\\Temp");
     * 
     * //read from file
     * let myExosPkg = new ExosPkg();
     * let result = myExosPkg.parseFile("C:\\Temp\\MyPackage\\MyPackage.exospkg")
     * if(result.fileParsed) {
     *      ..
     * }
     */
    constructor() {
        this._initialize();
    }

    _initialize() {
        this._files = [];
        this._services = [];
        this._datamodels = [];
        this._generateDatamodels = [];
        this._buildCommands = [];
        this._startupTimeout = "0";
        this._errorHandling = "Component";
        this._restartEvent = undefined;
        this.componentClass = undefined;
        this.componentVersion = undefined;
        this.componentOptions = {};
    }

    set startupTimeout(value) {
        if(!isNaN(value)) {
            this._startupTimeout = value.toString();
        }
    }

    get startupTimeout() {
        return this._startupTimeout;
    }

    get errorHandling() {
        return this._errorHandling;
    }

    set errorHandling(value) {
        switch(value) {
            case "Ignore":
            case "Component":
            case "Target":
                this._errorHandling = value;
                break;
        }
    }

    get restartEvent() {
        return this._restartEvent;
    }

    set restartEvent(value) {
        switch(value) {
            case "Ignore":
            case "Component":
            case "Target":
                this._restartEvent = value;
                break;
        }
    }

    /**
     * @returns {string} version of the ExosPkg file
     */
    get version() {
        return EXOSPKG_VERSION;
    }

    /**
     * Open an existing .exospkg file and populate the contents of the ExosPkg class.
     * 
     * This can be used to open older versions of the package file and convert them to new ones,
     * or simply to get a data representation of the current .exospkg file
     * 
     * @param {string} fileName 
     * @returns {ExosPkgParseFileResults} results of parsing the file
     */
    parseFile(fileName) {

        if (!fs.existsSync(fileName)) {
            throw(`ExosPkg: file does not exist: ${fileName}`);
        }

        /**
         * @type {ExosPkgParseFileResults}
         */
        let parseResults = {parseErrors:0, originalVersion:"n/a", fileParsed:false, componentFound:false, componentErrors:[]};
        let exosPkgFileContents = fs.readFileSync(fileName).toString();
        let exosPkgJson = parser(exosPkgFileContents);

        if(!exosPkgJson.root || !exosPkgJson.root.attributes || !exosPkgJson.root.attributes.Version) {
            return parseResults;
        }

        this._initialize();
        parseResults.originalVersion = exosPkgJson.root.attributes.Version;

        switch(exosPkgJson.root.attributes.Version) {
            case "1.1.0":
            case "2.1.0":
                //TODO populate after new syntax
                if(exosPkgJson.root.attributes.ErrorHandling) {
                    this.errorHandling = exosPkgJson.root.attributes.ErrorHandling;
                }
                if(exosPkgJson.root.attributes.StartupTimeout) {
                    this.startupTimeout = exosPkgJson.root.attributes.StartupTimeout;
                }

                parseResults.fileParsed = true;

                if(!exosPkgJson.root.children)
                    break;
                
                for(let child of exosPkgJson.root.children) {
                    if(child.name == "File") {
                        if(!child.attributes || !child.attributes.FileName) {
                            parseResults.parseErrors ++;
                            continue;
                        }
                        let changeEvent = "Ignore";
                        if(child.attributes.ChangeEvent) {
                            changeEvent = child.attributes.ChangeEvent;
                        }
                        this.addFile(child.attributes.FileName,changeEvent);
                    }
                    else if(child.name == "Service") {
                        if(!child.attributes || !child.attributes.Type || !child.attributes.Command) {
                            parseResults.parseErrors ++;
                            continue;
                        }

                        //WorkingDirectory can be undefined, thats ok
                        this.addService(child.attributes.Type,child.attributes.Command,child.attributes.WorkingDirectory);
                    }
                    else if(child.name == "DatamodelInstance") {
                        if(!child.attributes || !child.attributes.Name) {
                            parseResults.parseErrors ++;
                            continue;
                        }
                        this.addDatamodelInstance(child.attributes.Name);
                    }
                    else if(child.name == "ComponentGenerator") {
                        if(!child.attributes || !child.attributes.Class || !child.attributes.Version) {
                            parseResults.parseErrors ++;
                            continue;
                        }
                        let options = [];
                        if(child.children) {
                            for(let option of child.children) {
                                if(option.name == "Option" && option.attributes && option.attributes.Name && option.attributes.Value) {
                                    options.push({name:option.attributes.Name, value:option.attributes.Value})
                                }
                            }
                        }
                        this.setComponentGenerator(child.attributes.Class, child.attributes.Version, options);
                        parseResults.componentFound = true;
                    }
                    else if (child.name == "Build") {
                        if(!child.children)
                            continue;
                        for (let build of child.children) {
                            if(build.name == "GenerateDatamodel") {
                                if(!build.attributes || !build.attributes.FileName || !build.attributes.TypeName) {
                                    parseResults.parseErrors ++;
                                    continue;
                                }
                                let SG4Includes = [];
                                let outputPaths = [];
                                if(build.children) {
                                    for(let args of build.children) {
                                        if(args.name == "SG4") {
                                            if(!args.attributes || !args.attributes.Include) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            SG4Includes.push(args.attributes.Include);
                                        }
                                        else if(args.name == "Output") {
                                            if(!args.attributes || !args.attributes.Path) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            outputPaths.push(args.attributes.Path);
                                        }
                                    }
                                }
                                this.addGenerateDatamodel(build.attributes.FileName,build.attributes.TypeName,SG4Includes,outputPaths);
                            }
                            else if(build.name == "BuildCommand") {
                                if(!build.attributes || !build.attributes.Command || !build.attributes.WorkingDirectory || !build.attributes.Arguments) {
                                    parseResults.parseErrors ++;
                                    continue;
                                }
                                let builder = this.getNewBuildCommand(build.attributes.Command, build.attributes.WorkingDirectory, build.attributes.Arguments);
                                if (build.children) {
                                    for(let dep of build.children) {
                                        if (dep.name == "Dependency") {
                                            if(!dep.attributes || !dep.attributes.FileName) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            this.addBuildDependency(builder, dep.attributes.FileName);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
    
            case "1.0.0":
                //we have an old version
                if(exosPkgJson.root.attributes.ErrorHandling) {
                    switch(exosPkgJson.root.attributes.ErrorHandling) {
                        case "Ignore":
                            this.errorHandling = "Ignore";
                            break;
                        case "Break":
                            this.errorHandling = "Component";
                            break;
                        case "Abort":
                            this.errorHandling = "Target";
                            break;
                    }
                }

                if(exosPkgJson.root.attributes.StartupTimeout) {
                    this.startupTimeout = exosPkgJson.root.attributes.StartupTimeout;
                }

                parseResults.fileParsed = true;

                if(!exosPkgJson.root.children)
                    break;

                for(let child of exosPkgJson.root.children) {
                    //check for files
                    if(child.name == "File") {
                        if(!child.attributes || !child.attributes.FileName) {
                            parseResults.parseErrors ++;
                            continue;
                        }

                        //check if it is the special .deb thing
                        if(child.attributes.FileName.endsWith(".deb")) {
                            if(!child.attributes.Name) {
                                parseResults.parseErrors ++;
                                continue;
                            }
                            this.addFile(child.attributes.FileName,"Reinstall");
                            this.addService("Install",`dpkg -i ${path.basename(child.attributes.FileName)}`);
                            if(!child.attributes.Type || child.attributes.Type == "Project") {
                                this.addService("Remove",`dpkg --purge ${child.attributes.Name}`);
                            }
                        }
                        //otherwise just add the file
                        else {
                            this.addFile(child.attributes.FileName,"Restart");
                        }
                    }
                    //check for runtime services
                    else if(child.name == "Service") {
                        if(!child.attributes || !child.attributes.Executable) {
                            parseResults.parseErrors ++;
                            continue;
                        }

                        let cmd = child.attributes.Executable;
                        if(child.attributes.Arguments)
                        {
                            cmd += " " + child.attributes.Arguments;
                        }
                        //add an empty working directory that we can pinpoint the user to maybe change the service
                        this.addService("Runtime",cmd,"");
                    }
                    //check for other services
                    else if(child.Name == "Installation") {
                        if(!child.attributes || !child.attributes.Type || !child.attributes.Command) {
                            parseResults.parseErrors ++;
                            continue;
                        }
                        
                        switch(child.attributes.Type) {
                            case "Prerun":
                                this.addService("Startup", child.attributes.Command, "");
                                break;
                            case "Preinst":
                                this.addService("Install", child.attributes.Command, "");
                                break;
                            case "Postinst":
                                this.addService("Install", child.attributes.Command, "");
                                break;
                            case "Prerm":
                                this.addService("Remove", child.attributes.Command, "");
                                break;
                            case "Postrm":
                                this.addService("Remove", child.attributes.Command, "");
                                break;
                        }
                    }
                    else if(child.name == "DatamodelInstance") {
                        if(!child.attributes || !child.attributes.Name) {
                            parseResults.parseErrors ++;
                            continue;
                        }
                        this.addDatamodelInstance(child.attributes.Name);
                    }
                    else if (child.name == "Build") {
                        if(!child.children)
                            continue;
                        for (let build of child.children) {
                            if(build.name == "GenerateHeader" || build.name == "GenerateDatamodel") { //we have a strange mix here.. allow both
                                if(!build.attributes || !build.attributes.FileName || !build.attributes.TypeName) {
                                    parseResults.parseErrors ++;
                                    continue;
                                }
                                let SG4Includes = [];
                                let outputPaths = [];
                                if(build.children) {
                                    for(let args of build.children) {
                                        if(args.name == "SG4") {
                                            if(!args.attributes || !args.attributes.Include) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            SG4Includes.push(args.attributes.Include);
                                        }
                                        else if(args.name == "Output") {
                                            if(!args.attributes || !args.attributes.Path) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            outputPaths.push(args.attributes.Path);
                                        }
                                    }
                                }
                                this.addGenerateDatamodel(build.attributes.FileName,build.attributes.TypeName,SG4Includes,outputPaths);
                            }
                            else if(build.name == "BuildCommand") {
                                if(!build.attributes || !build.attributes.Command || !build.attributes.WorkingDirectory || !build.attributes.Arguments) {
                                    parseResults.parseErrors ++;
                                    continue;
                                }
                                let builder = this.getNewBuildCommand(build.attributes.Command, build.attributes.WorkingDirectory, build.attributes.Arguments);
                                if (build.children) {
                                    for(let dep of build.children) {
                                        if (dep.name == "Dependency") {
                                            if(!dep.attributes || !dep.attributes.FileName) {
                                                parseResults.parseErrors ++;
                                                continue;
                                            }
                                            this.addBuildDependency(builder, dep.attributes.FileName);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            
            default:
                break;
        }

        return parseResults;
    }


    /**
     * Add a file to be transferred to the target system
     * 
     * @param {string} fileName name and relative location of the file, like "Linux/myFile.txt"
     * @param {string} changeEvent `Ignore` | `Restart` | `Reinstall` - behaviour of the target component when file is added, removed or changed. 
     */
    addFile(fileName, changeEvent) {
        this._files.push({fileName:fileName, changeEvent:changeEvent});
    }

    /**
     * Add a service to be executed on the target system
     * 
     * @param {string} type `Install` | `Remove` | `Startup` | `Runtime`
     * @param {string} command commandline to be executed, like "/home/user/myexecutable" or "cp myFile.txt /home/user/"
     * @param {string} [workingDirectory] working directory of the service, like "/home/user". If omitted, the deployment directory of the component is used
     */
    addService(type, command, workingDirectory) {
        this._services.push({type:type,command:command,workingDirectory:workingDirectory});
    }

    /**
     * Add a communication channel (datamodel instance) to the datacommunication for exchanging data with the target system
     * 
     * @param {string} name The `datamodel_instance_name` provided in the `exos_datamodel_connect_{type}()` function of the application
     */
    addDatamodelInstance(name) {
        if (name != undefined)
            this._datamodels.push({name:name});
    }

    /**
     * Add a compiler command to generate datamodel header/source files for datacommunication in the exOS TP.
     * 
     * **Regarding `SG4Includes`:**
     * 
     * In most cases, exos components are created from IEC .typ files which also exist in AS and that generate a C-declaration
     * of this type which can be included in C-programs. For example, if the .typ file is within "TypeLib", then "TypeLib.h"
     * should be added as SG4 include, that the C-Program can utilize the library functions without clashing with the generated
     * C-datatypes in the generated c/h datamodel files in AR.
     * 
     * **Regarding `outputPaths`:**
     * 
     * The exOS TP GenerateDatamodel needs an output directory for the generated files, like the name of a library or a linux folder.
     * The in each of these directories, the functionality creates two files. The filenames are accessed via the `Datamodel` class (see example).
     * These files need to be added as {@link addExistingFile()} (currently without content) or {@link addNewFile} (with generated content) within 
     * the packages defined as `OutputPaths`
     *
     * **Note:**
     * 
     * This method does not actually *generate* the files, it only populates the .exospkg file with the instruction for the exOS TP.
     * If the files should be created together with creating the {@link ExosPackage}, the {@link Datamodel} class should be used to create the datamodel file contents.
     * 
     * @example
     * 
     * let myPackage = new ExosPackage("MyPackage");
     * let linux = myPackage.getNewLinuxPackage("Linux");
     * let lib = myPackage.getNewCLibrary("TypeLib");
     * //generate the datamodel file contents
     * let datamodel = new Datamodel("TypeLib/Types.typ", "Types", ["TypeLib.h"]);
     * //set exOS TP to generate files in the "TypeLib" and "Linux" directories, and include the "TypeLib" library in AR
     * myPackage.exospkg.addGenerateDatamodel("TypeLib/Types.typ", "Types", ["TypeLib.h"], ["TypeLib", "Linux"]);
     * //add the generated files to the "TypeLib" and "Linux" packages, and populate the contents from the parsed datamodel
     * linux.addNewFile(datamodel.headerFile.name, datamodel.headerFile.contents, datamodel.headerFile.description); 
     * linux.addNewFile(datamodel.sourceFile.name, datamodel.sourceFile.contents, datamodel.sourceFile.description);
     * lib.addNewFile(datamodel.headerFile.name, datamodel.headerFile.contents, datamodel.headerFile.description);
     * lib.addNewFile(datamodel.sourceFile.name, datamodel.sourceFile.contents, datamodel.sourceFile.description);
     *      
     * 
     * @param {string} fileName Name of the IEC .typ file that contains the datatype, relative to the exospkg location
     * @param {string} typeName Name of the datatype withing the .typ file
     * @param {string[]} SG4Includes list of specific `_SG4` include directives to include the datatype generated from the IEC `typeName` file in Automation Studio
     * @param {string[]} outputPaths list of directories relative to the .exospkg where the c/h files are generated to 
     */
    addGenerateDatamodel(fileName, typeName, SG4Includes, outputPaths) {
        if(!Array.isArray(SG4Includes)) {
            SG4Includes = [];
        }
        if(!Array.isArray(outputPaths)) {
            outputPaths = [];
        }
        this._generateDatamodels.push({fileName:fileName,typeName:typeName,SG4Includes:SG4Includes,outputPaths:outputPaths});
    }

    /**
     * The exOS TP can call an external build command before building the rest of the system. This can be
     * utilized to compile linux sources on the local computer.
     * 
     * exOS in itself has no dependency to an external build system, and only forwards the output, or 
     * possible errors (stderr) to the AS Build Output. In most cases, WSL is a good option for building
     * Linux sources, and this build command is added via the simplified {@link getNewWSLBuildCommand}
     * 
     * @param {string} command Command to be executed on the local computer
     * @param {string} workingDirectory Working directory of the build command on the local computer
     * @param {string} args Arguments passed to the build command
     * @returns {object} object that can be further populated with file dependencies using `addBuildDependency()`
     */
    getNewBuildCommand(command, workingDirectory, args) {
        this._buildCommands.push({command:command, workingDirectory:workingDirectory, args:args, Dependencies:[]});
        return this._buildCommands[this._buildCommands.length-1];
    }

    /**
     * Call a predefined wsl build command that runs a script on the local Debian WSL distribution 
     * 
     * @example
     * let myPackage = new ExosPackage("MyPackage");
     * let linux = myPackage.getNewLinuxPackage("Linux")    
     * let buildFile = linux.getNewFile("build.sh", "Build Script");
     * buildFile.contents = "echo 'this is a test'\n";
     * let build = myPackage.exospkg.getNewWSLBuildCommand("Linux","build.sh");
     * 
     * @param {string} buildScript name of the scriptfile that is being executed in the `linuxPackage`, e.g. `build.sh`
     * @param {string} linuxPackage name of the Linux package where the `buildScript` is located
     * @returns {object} object that can be further populated with file dependencies using `addBuildDependency()`
     */
    getNewWSLBuildCommand(linuxPackage, buildScript) {
        return this.getNewBuildCommand("C:\\Windows\\Sysnative\\wsl.exe",linuxPackage,`-d Debian -e sh ${buildScript} $(EXOS_VERSION)`);
    }

    /**
     * Get a list of wsl distributions used in the build commands
     * 
     * @returns {string[]} empty list [] if no wsl.exe is found as build command, empty string list [""] if wsl.exe is found, but -d {Distro} isnt used, otherwise eg. ["Debian","DebianTest"]
     */ 
    getWSLBuildDistros() {
        let distros = [];
        for (const buildCommand of this._buildCommands) {
            if (buildCommand.command.includes("wsl.exe")) {
                let distro = "";
                //pull out the distro name if provided
                if (buildCommand.args.includes("-d")) {
                    distro = buildCommand.args.replace(/-d\s(\S+)\s.*/, '$1');
                }
                //add the distro name if not already in the list
                if (distros.indexOf(distro) == -1) {
                    distros.push(distro);
                }
            }
        }
        return distros;
    }
     
    /**
     * Add a dependency for the exOS TP build process, that a file can be monitored for changes that will trigger the build command to run.
     * The LinuxPackage has a shortcut to add files including adding them as build dependencies with {@link getNewBuildFile()}
     * 
     * @example
     * let myPackage = new ExosPackage("MyPackage");
     * let linux = myPackage.getNewLinuxPackage("Linux")    
     * let buildFile = linux.getNewFile("build.sh");
     * buildFile.contents = "echo 'this is a test'\n";
     * let sourceFile = linux.getNewFile("source.c", "Source Code");
     * sourceFile.contents = ..*      
     * let build = myPackage.exospkg.getNewWSLBuildCommand("Linux","build.sh");
     * myPackage.exospkg.addBuildDependency(build,"Linux/source.c"); //monitor source.c for changes
     * 
     * 
     * @param {object} buildCommand object returned from {@link getNewBuildCommand()} or {@link getNewWSLBuildCommand()}
     * @param {string} fileName filename including the path relative to the .exospkg that should be added as build dependency
     */
    addBuildDependency(buildCommand, fileName) {
        buildCommand.Dependencies.push(fileName);
    }

    /**
     * Function to generate a {@linkcode ExosPkgComponentGeneratorOption} compatible list from an object
     * 
     * @example
     * let myObj = {width:10, flat:true};
     * let options = ExosPkg.getComponentOptions(myObj);
     * console.log(options);
     * //Output:
     * //[{name:'width', value:'10'},{name:'flat', value:'true'}]
     * 
     * @param {object} optionObject any kind of flat option structure, with members
     * @returns {ExosPkgComponentGeneratorOption[]} componentOptions list that can be used in the {@linkcode setComponentGenerator} or {@linkcode addGeneratorOption}
     */
    static getComponentOptions(optionObject) {
        let options = [];

        for(const [name, value] of Object.entries(optionObject))
        {
            //dont push undefined members (keys) in the options array
            if(value) {
                options.push({name:name,value:value.toString()});
            }
        }

        return options;
    }

    /**
     * Add an additional option to the ComponentGenerator section.
     * 
     * This can be helpful if not all options are available at the time of calling the {@linkcode setComponentGenerator}
     * 
     * Note that the {@linkcode setComponentGenerator} needs to be called in order to have any option information in the .exospkg file.
     * It can however be called before or after this method.
     * 
     * @param {string} name 
     * @param {string} value 
     */
    addGeneratorOption(name, value) {
        if(name && value) {

            this.componentOptions[name] = value.toString();
        }
    }

    /**
     * Specify which Component was used to generate this package. This allows to make updates and exports of the package (by reproducing the initial setting).
     * 
     * There can only be one ComponentGenerator per ExosPkg, so its set rather than added. options is a prefilled struct array of name/value pairs
     * 
     * The fields can be accessed via
     * - {@linkcode ExosPkg.componentClass}
     * - {@linkcode ExosPkg.componentVersion}
     * - {@linkcode ExosPkg.componentOptions}
     * 
     * @example
     * let myPackage = new ExosPackage("MyPackage");
     * myPackage.exospkg.setComponentGenerator("MyComponentClass", "1.0.0", [{name:'width', value:'10'},{name:'flat', value:'true'}]);
     * console.log(myPackage.exospkg.componentOptions)
     * 
     * 
     * @param {string} componentClass mandatory. Component class name
     * @param {string} componentVersion mandatory. Version in the form "X.X.X"
     * @param {ExosPkgComponentGeneratorOption[]} componentOptions mandatory 
     */
    setComponentGenerator(componentClass, componentVersion, componentOptions) {
        if(!componentClass || !componentVersion || !componentOptions || !Array.isArray(componentOptions)) {
            return;
        }

        this.componentClass = componentClass;
        this.componentVersion = componentVersion;
        for(let option of componentOptions) {
            if(option.name && option.value) {
                this.componentOptions[option.name] = option.value.toString();
            }
        }
    }


    /**
     * When all files, services, datamodels etc. have been added, the .exospkg file can be created using {@link getContents()}.
     * This function is called implicitly by {@link ExosPackage.makePackage}, which contains an internal ExosPkg object.
     * It can be used for custom `ExosPkg` objects, or just to get the contents of the file
     * 
     * @example
     * let myPackage = new ExosPackage("MyPackage");
     * myPackage.exospkg.addService("Runtime","/home/user/myexecutable");
     * console.log(myPackage.exospkg.getContents()); //display the output of the .exospkg file
     * 
     * @returns {string} the XML file contents of the .exospkg file
     */
    getContents()
    {
        let out = ``;

        out += `<?xml version="1.0" encoding="utf-8"?>\n`;
        out += `<ComponentPackage Version="${EXOSPKG_VERSION}" ErrorHandling="${this._errorHandling}" StartupTimeout="${this._startupTimeout}"`
        if(this._restartEvent) {
            out += `RestartEvent="${this._restartEvent}"`;
        }
        out += `>\n`;
        for(const file of this._files) {
            out += `    <File FileName="${file.fileName}"`;
            if(file.changeEvent !== undefined) {
                out += ` ChangeEvent="${file.changeEvent}"`;
            }
            out += `/>\n`;
        }
        for(const service of this._services) {
            out += `    <Service Type="${service.type}" Command="${service.command}"`;
            if(service.workingDirectory !== undefined) {
                out += ` WorkingDirectory="${service.workingDirectory}"`;
            }
            out += `/>\n`;
        }
        for(const datamodel of this._datamodels) {
            out += `    <DatamodelInstance Name="${datamodel.name}"/>\n`;
        }
        
        if(this._generateDatamodels.length > 0 || this._buildCommands.length > 0)
        {
            out += `    <Build>\n`;
            for(const generateDatamodel of this._generateDatamodels) {
                out += `        <GenerateDatamodel FileName="${generateDatamodel.fileName}" TypeName="${generateDatamodel.typeName}">\n`;
                for(const SG4Include of generateDatamodel.SG4Includes) {
                    out += `            <SG4 Include="${SG4Include}"/>\n`;
                }
                for(const outputPath of generateDatamodel.outputPaths) {
                    out += `            <Output Path="${outputPath}"/>\n`;
                }
                out += `        </GenerateDatamodel>\n`;
            }
            for(const buildCommand of this._buildCommands) {
                out += `        <BuildCommand Command="${buildCommand.command}" WorkingDirectory="${buildCommand.workingDirectory}" Arguments="${buildCommand.args}">\n`;
                for(const dependency of buildCommand.Dependencies) {
                    out += `            <Dependency FileName="${dependency}"/>\n`;
                }
                if(this.componentClass == "ExosComponentJulia")
                {
                    out += `            <Dependency FileName="Linux/Manifest.toml"/>\n`;
                    out += `            <Dependency FileName="Linux/Project.toml"/>\n`;
                    out += `            <Dependency FileName="Linux/EXOS.jl"/>\n`;
                    out += `            <Dependency FileName="Linux/api.jl"/>\n`;
                    out += `            <Dependency FileName="Linux/enums.jl"/>\n`;
                    out += `            <Dependency FileName="Linux/types.jl"/>\n`;
                    out += `            <Dependency FileName="Linux/README.md"/>\n`;
                }
                out += `        </BuildCommand>\n`;
            }

            out += `    </Build>\n`;
        }

        if(this.componentClass && this.componentVersion) {
            out += `    <!-- ComponentGenerator info - do not change! -->\n`;
            out += `    <ComponentGenerator Class="${this.componentClass}" Version="${this.componentVersion}">\n`;
            for(const [name, value] of Object.entries(this.componentOptions)) {
                if(name && value) {
                    out += `        <Option Name="${name}" Value="${value}"/>\n`;
                }
            }
            out += `    </ComponentGenerator>\n`;
        }
        out += `</ComponentPackage>\n`;
        return out;
    }
}

if (require.main === module) {
    
    if (process.argv.length > 1) {
        let fileName = process.argv[2];
        let exospkg = new ExosPkg();
		exospkg.openExosPkgFile(fileName);
		console.log(exospkg.getContents());
    }
    else {
        process.stderr.write("usage: ./exospkg.js <filename>\n");
    }
}

module.exports = {ExosPkg, EXOSPKG_VERSION};