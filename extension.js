// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
const vscode = require('vscode');
const path = require('path');
const fs = require('fs');
const fse = require('fs-extra');
const { dir } = require('console');
const os = require('os');
const net = require('net');
const linuxShell = require('./src/linuxshell')

const isDebugMode = () => process.env.VSCODE_DEBUG_MODE === "true";

const { ExosPkg } = require('./src/exospkg')
const { Datamodel, DatatypeListItem } = require('./src/datamodel');
const { UpdateComponentResults } = require('./src/components/exoscomponent');
const { ExosComponentC, ExosComponentCUpdate } = require('./src/components/exoscomponent_c');
const { ExosComponentNAPI, ExosComponentNAPIUpdate } = require('./src/components/exoscomponent_napi');
const { ExosComponentSWIG, ExosComponentSWIGUpdate } = require('./src/components/exoscomponent_swig');
const { ExosComponentPython, ExosComponentPythonUpdate } = require('./src/components/exoscomponent_python');
const { ExosComponentJS, ExosComponentJSUpdate } = require('./src/components/exoscomponent_js');
const { ExosComponentJuliet, ExosComponentJulietUpdate} = require('./src/components/exoscomponent_jlt');
const { ExosExport, ASConfiguration } = require('./src/exosexport');
const { Package } = require('./src/exospackage');
const { ASProject, ASProjectConfiguration } = require('./src/asproject')

var lastIP = "127.0.0.1"

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {

	// let taskProvider = vscode.tasks.registerTaskProvider('exostest', new vscode.Task({type:"exostest"},vscode.TaskScope.Workspace,"buildme","exOS",new vscode.ShellExecution("echo hello")));
	// context.subscriptions.push(taskProvider);

	// Use the console to output diagnostic information (console.log) and errors (console.error)
	// This line of code will only be executed once when your extension is activated
	console.log('Extension "exos-component-extension" is now active! debug is: ' +isDebugMode());

	// Set a key that can be e.g. used in when-clause in package.json
	vscode.commands.executeCommand('setContext', 'exos-component-extension.isDebugMode', isDebugMode());

	// The command has been defined in the package.json file
	// Now provide the implementation of the command with  registerCommand
	// The commandId parameter must match the command field in package.json



	const COMMAND_UPDATE_BUILD = 1;
	const COMMAND_RUN_DMR = 2;

	/**
	 * @param {vscode.Uri} uri uri of the .exospkg file
	 * @param {number} command COMMAND_UPDATE_BUILD || COMMAND_RUN_DMR 
	 */
	function runExosDataFromExospkg(uri, command) {
		
		/**
		 * run the UPDATE_BUILD command by first prompting for the user 
		 * 
		 * @param {string} exosDataDir working directory of exos-data
		 * @param {string} [distro] WSL distribution, undefined if not used
		 */
		function updateBuild(exosDataDir, distro) {
			let userType = [];
			userType.push({ label: "sudo user", detail: "The default user is a sudo user" });
			userType.push({ label: "root", detail: "The default user is root" })
		
			vscode.window.showQuickPick(userType, { title: "Select default user" }).then(selectedUserType => {

				if (selectedUserType.label == "root") {
					linuxShell.runCommand('Updating exOS build environment', 'sh', ['setup_build_environment.sh'], exosDataDir, distro);
				}
				else {
					vscode.window.showInputBox({ prompt: "Enter password for default sudo user", password: true }).then(password => {
					
						//create a file passing in the password to the sudo command, this cannot be done via the spawn cmdline
						fs.writeFileSync(path.join(exosDataDir, "update.sh"), `echo "${password}" | sudo -S sh setup_build_environment.sh`)
						
						linuxShell.runCommand('Updating exOS build environment', 'sh', ['update.sh'], exosDataDir, distro);

						//remove the file with the password asap..
						setTimeout(() => {
							fs.unlinkSync(path.join(exosDataDir, "update.sh"));
						}, 3000);

					});
				}
			});
		}

		try {

			// the ExosPkg is used for getting the WSLBuildcommands
			let exospkg = new ExosPkg();
			let result = exospkg.parseFile(uri.fsPath);
			
			if (!result.fileParsed) {
				throw (`${path.basename(uri.fsPath)} could not be parsed!`);
			}

			// ASProject is used for locating the Temp directory
			let asProject = new ASProject(uri.fsPath, true);

			/** 
			* @type {ASProjectConfiguration[]}
			*/
			let configurations = asProject.getConfigurations();

			let pickConfiguration = [];

			for(let configuration of configurations) {
				pickConfiguration.push({label: configuration.name, detail: configuration.cpu, description: configuration.description});
			}

			vscode.window.showQuickPick(pickConfiguration, { title: `Select configuration` }).then(selectedConfiguration => {
				
				let exosDataDir = path.join(asProject.getApjPath(), "Temp", "Transfer", selectedConfiguration.label, selectedConfiguration.detail, "FilesToTransfer", "AddonsData", "exos", "exos-data");
				
				try {
					let stats = fs.statSync(exosDataDir);
					if (!stats.isDirectory()) {
						vscode.window.showErrorMessage("exos-data cannot be found in this configuration. Project needs to be built.");
					}
				}
				catch (e) {
					vscode.window.showErrorMessage(`exos-data cannot be found in this configuration. Project needs to be built. ${e}`);
					return;
				}

				//in linux, we dont use WSL build commands nor specific distributions
				if (os.platform().includes('linux')) {
					if (command == COMMAND_RUN_DMR) {
						linuxShell.runCommand('Running exOS data connection', '/usr/bin/exos_mr', ['-c', './exos-data.conf'], exosDataDir);
						vscode.window.showWarningMessage(`exOS data connection can currently not be gracefully shut down. Terminating the data connection (Enter) might require a system restart to properly work again`);
					}
					else if (command == COMMAND_UPDATE_BUILD) {
						updateBuild(exosDataDir);
					}
				}
				else if (os.platform().includes('win')) {

					let distros = exospkg.getWSLBuildDistros();
					if (distros.length == 0) {
						vscode.window.showErrorMessage(`The file ${path.basename(uri.fsPath)} has no WSL build command. WSL distribution is unkown`);
						return;
					}

					let distroType = [];
					for (const distro of distros) {
						if (distro.length > 0) {
							distroType.push({ label: distro, detail: `WSL distribution "${distro}"` })
						}
						else {
							distroType.push({ label: "(default)", detail: `WSL default distribution` })
						}
					}
					vscode.window.showQuickPick(distroType, { title: "Select WSL distribution" }).then(selectedDistro => {
			
						let distro = undefined;
						if (selectedDistro.label != "(default)") {
							distro = selectedDistro.label;
						}
						
						if (command == COMMAND_RUN_DMR) {
							if (distro) {
								linuxShell.runCommand('Running exOS data connection', '/usr/bin/exos_mr', ['-c', './exos-data.conf'], exosDataDir, distro, true);
								vscode.window.showWarningMessage(`exOS data connection can currently not be gracefully shut down. Terminating the data connection (Enter) will also terminate the WSL distribution "${selectedDistro.label}"`);
							}
							else {
								vscode.window.showErrorMessage(`The file ${path.basename(uri.fsPath)} has no specific WSL distribution, data connection currently cannot run on default distribution.`);
								return;
							}
						}
						else if (command == COMMAND_UPDATE_BUILD) {
							
							updateBuild(exosDataDir, distro);
						}
					});
				}
				else {
					vscode.window.showErrorMessage(`Platform ${os.platform()} is not supported`);
				}
			});
			
		}
		catch(error) {
			vscode.window.showErrorMessage(error);
		}
	}
	
	let updateBuildEnvironment = vscode.commands.registerCommand('exos-component-extension.updateBuildEnvironment', function (uri) {
	
		runExosDataFromExospkg(uri, COMMAND_UPDATE_BUILD);

	});

	context.subscriptions.push(updateBuildEnvironment);

	let runDatasetMessageRouter = vscode.commands.registerCommand('exos-component-extension.runDatasetMessageRouter', function (uri) {
	
		runExosDataFromExospkg(uri, COMMAND_RUN_DMR);

	});

	context.subscriptions.push(runDatasetMessageRouter);

	let debugTerminal = vscode.commands.registerCommand('exos-component-extension.debugConsole', function() {
		
		vscode.window.showInputBox({prompt:"IP address of the AR target:", value:lastIP}).then(selectedIP => {

			lastIP = selectedIP;
			let port = 30000;
			let ip = selectedIP;

			const writeEmitter = new vscode.EventEmitter();

			const pty = {
				onDidWrite: writeEmitter.event,
				open: () => {writeEmitter.fire(`exOS Debug Console on ${ip}\r\nPress Enter to reconnect broken connections\r\n\r\n`)},
				close: () => {},
				handleInput: data => {
					if(data === '\r')
					{
						if(!client.connecting) {
							writeEmitter.fire(`Reconnecting to ${ip}:${port}\r\n`)
							client.connect(port,ip);
						}
						else {
							writeEmitter.fire(`hold on, already connecting..\r\n`)
						}
					}
				}
			};		
			let terminal = vscode.window.createTerminal({ name: 'exOS Debug Console', pty });

			let client = new net.Socket();
			

			writeEmitter.fire(`Connecting to ${ip}:${port}\r\n`)
			client.connect(port, ip)

			client.on('timeout', () => {
				writeEmitter.fire('Disconnected\r\nPress Enter to reconnect\r\n');
				client.destroy();
			});

			client.on('connect', function () {
				client.setTimeout(3000);
				writeEmitter.fire('Connected\r\n');
			});

			client.on('data', function (data) {

				let lines = data.toString().replace("\r","").split("\n")
				for(line of lines) {
					if(line.length > 0) {
						writeEmitter.fire(`${line}\r\n`);
					}
				}
			});

			client.on('close', function () {
				client.destroy();
				writeEmitter.fire('Connection closed\r\nPress Enter to reconnect\r\n');
			});

			client.on('error', function (error) {
				client.destroy();
				writeEmitter.fire(`Connection error (${error.code})\r\n`);
			});


			terminal.show();
		});

	});

	context.subscriptions.push(debugTerminal);


	let createPackage = vscode.commands.registerCommand('exos-component-extension.createPackage', function (uri) {

		function createComponent(uri, selectedStructure, selectedASType, selectedLinuxType, selectedPackaging, destination) {
			let stats = fs.statSync(uri.fsPath);
			let makeComponentPath = uri.fsPath;
			if (!stats.isDirectory()) {
				makeComponentPath = path.dirname(uri.fsPath);
			}

			function convertLabel2Template(label) {
				switch(label) {
					case "C API":
						return "c-api";
					case "C Interface":
						return "c-static";
					case "C++ Class":
						return "cpp";
					case "deploy-only":
						return "deploy-only";
					default:
						return "c-api";
				}
			}

			/**
			 * Update the Package file if we are in an AS project
			 * If fsPath is a typ file we delete the typ file and replace it in the 
			 * package file with the new exos package folder (we created in createComponent)
			 * If fsPath is a folder we are creating a deploy only component and
			 * we add it to the package file or create a new package file if not existing
			 * 
			 * @param {*} fsPath path to the .typ file or exOS deploy only package
			 * @param {*} typeName name of the new package (the struct name everything is based on)
			 * @returns {string} result message, can be used in vscode.window.showInformationMessage
			 */
			function updatePackage(fsPath, typeName) {

				let pkgFileName = "";
				let stats = fs.statSync(fsPath);
				let typMode = false;
				let insertPackage = true;

				if (!stats.isDirectory()) {
					typMode = true;
					pkgFileName = `${path.dirname(fsPath)}/Package.pkg`;
				}
				else {
					pkgFileName = `${fsPath}/Package.pkg`;
				}
				
				pkgFileName = path.normalize(pkgFileName);
				
				if (fs.existsSync(pkgFileName)) {
					let lines = fs.readFileSync(pkgFileName).toString();
					let writeFile = false;
					lines = lines.split("\r").join("");
					lines = lines.split("\n");
					let out = "";
					for(let line of lines) 
					{
						if(typMode && line.includes(path.basename(fsPath)) && line.includes("Object") && line.includes("File")) {
							out += `    <Object Type="Package">${typeName}</Object>\r\n`;
							writeFile = true;
							insertPackage = false;
						}
						else if (line.includes(typeName) && line.includes("Object") && line.includes("Package")) {
								insertPackage = false;
						}
						else if (insertPackage && line.includes("</Objects>")) {
							out += `    <Object Type="Package" Description="Package for deployment only">${typeName}</Object>\r\n`;
							out += `${line}\r\n`;
							writeFile = true;
						} 
						else {
							out += `${line}\r\n`;
						}
					}
			
					if(writeFile)
					{
						fs.writeFileSync(pkgFileName,out);
						if (typMode) {
							fs.unlinkSync(fsPath);
							return `The file ${path.basename(uri.fsPath)} was replaced with the ${selectedStructure.label} exOS package`;
						}
						else {
							return `The exOS package ${typeName} was added to the ${path.dirname(fsPath)} package`;
						}
					}
				}
				else if (!typMode) {
					let pack = new Package(path.basename(fsPath), "Package.pkg");
					pack.addExistingPackage(typeName, "Package for deployment only");
					pack.makePackage(path.dirname(fsPath));
					return `The exOS package ${typeName} was added to newly created Package file in: ${path.dirname(fsPath)}`;
				}
				return "";
			}

			try
			{
				switch(selectedLinuxType.label) {
					case "C API":
					case "C Interface":
					case "C++ Class":
						let templateC = new ExosComponentC(uri.fsPath, selectedStructure.label, {
							packaging:selectedPackaging.label,
							templateLinux:convertLabel2Template(selectedLinuxType.label), 
							templateAR:convertLabel2Template(selectedASType.label),
							destinationDirectory:destination
						});
						templateC.makeComponent(makeComponentPath);
						break;
					case "Python Module":
						if (!stats.isDirectory()) {
							let templateSWIG = new ExosComponentSWIG(uri.fsPath, selectedStructure.label,{
								packaging:selectedPackaging.label,
								templateAR:convertLabel2Template(selectedASType.label),
								destinationDirectory:destination
							});
							templateSWIG.makeComponent(makeComponentPath);
						}
						else 
						{
							let templatePython = new ExosComponentPython(uri.fsPath, selectedStructure.label,{
								packaging:selectedPackaging.label,
								templateAR:convertLabel2Template(selectedASType.label),
								destinationDirectory:destination
							});
							templatePython.makeComponent(makeComponentPath);
						}
						break;
					case "JavaScript Module":
						if (!stats.isDirectory()) {
							let templateNAPI = new ExosComponentNAPI(uri.fsPath, selectedStructure.label,{
								packaging:selectedPackaging.label,
								templateAR:convertLabel2Template(selectedASType.label),
								destinationDirectory:destination
							});
							templateNAPI.makeComponent(makeComponentPath);
						}
						else 
						{
							let templateJS = new ExosComponentJS(uri.fsPath, selectedStructure.label,{
								packaging:selectedPackaging.label,
								templateAR:convertLabel2Template(selectedASType.label),
								destinationDirectory:destination
							});
							templateJS.makeComponent(makeComponentPath);
						}
						break;
					case "Juliet Module":
						let templateJuliet = new ExosComponentJuliet(uri.fsPath, selectedStructure.label, {
							packaging:selectedPackaging.label,
							templateLinux:convertLabel2Template(selectedLinuxType.label), 
							templateAR:convertLabel2Template(selectedASType.label),
							destinationDirectory:destination
						});
						templateJuliet.makeComponent(makeComponentPath);
						break;
					default:
						vscode.window.showErrorMessage(`The selected template for linux: ${selectedLinuxType.label} not found!`);
						return;
				}
			
				vscode.window.showInformationMessage(`Created component ${selectedStructure.label}`);
				let ret = updatePackage(uri.fsPath, selectedStructure.label);
				if(ret != "") {
					vscode.window.showInformationMessage(ret);
				}
			}
			catch(error) {
				vscode.window.showErrorMessage(error);
			}

		}

		// create package by right-clicking on a folder, there will not be generated anything for datamodel
		let stats = fs.statSync(uri.fsPath);
		if (stats.isDirectory()) {
			//vscode.window.showInformationMessage(`Create package in directory will in the future create a Linux only package`);

			vscode.window.showInputBox({prompt:"Name of the deployable component with no datamodel", value:`LinuxComponent`}).then(name => {

				if(!name)
					return;

				selectedStructure = {label: name, detail:"no datamodel"};

				
				let pickLinuxType = [];
				pickLinuxType.push({label: "C API", detail:"C application with no datamodel"});
				pickLinuxType.push({label: "C++ Class", detail:`C++ application with no datamodel`})
				pickLinuxType.push({label: "Python Module", detail:`Python application with no datamodel`})
				pickLinuxType.push({label: "JavaScript Module", detail:`nodejs JavaScript application with no datamodel`})
				pickLinuxType.push({label: "Juliet Module", detail:`Juliet application with no datamodel`})
				
				vscode.window.showQuickPick(pickLinuxType,{title:`Using no datamodel - Select which template to use for Linux`}).then(selectedLinuxType => {

					if(!selectedLinuxType)
						return;
				
					let pickPackaging = [];
					pickPackaging.push({label: "none", detail:"No packaging - files will run in the deployment folder on the target"});
					pickPackaging.push({label: "deb", detail:"Debian package - files will be packed and extracted at a given destination folder"});

					vscode.window.showQuickPick(pickPackaging,{title:`Select which packaging to use for Linux files`}).then(selectedPackaging => {

						if(!selectedPackaging)
							return;
						
						selectedASType = {label: "deploy-only", detail:"no AS"};

						if(selectedPackaging.label == "deb") {
							vscode.window.showInputBox({prompt:"Set the .deb package destination folder on the target:", value:`/home/user/${selectedStructure.label.toLowerCase()}`}).then(destination => {
								if(!destination)
									return;

								createComponent(uri, selectedStructure, selectedASType, selectedLinuxType, selectedPackaging, destination);
							});
						}
						else {
							createComponent(uri, selectedStructure, selectedASType, selectedLinuxType, selectedPackaging, "");
						}
					});
				})
			});
			
		}
		else
		{
			// create package by right-clicking on a typ file.
			let availableStructures = Datamodel.getDatatypeList(uri.fsPath);
			if(!Array.isArray(availableStructures) || availableStructures.length == 0)
			{
				vscode.window.showErrorMessage(`The file ${path.basename(uri.fsPath)} has no structure definitions!`);
			}
			else
			{
				let pickStructureList = [];
				for(let struct of availableStructures) {
					if(struct.members.length > 0) {
						pickStructureList.push({label:struct.name, detail:`datasets: ${struct.members.join(", ")}`})
					}
					else {
						pickStructureList.push({label:struct.name});
					}
				}
				vscode.window.showQuickPick(pickStructureList,{title:"Select datatype which becomes the new component datamodel"}).then(selectedStructure => {

					if(!selectedStructure)
						return;

					let pickASType = []
					pickASType.push({label: "C API", detail:"AR C library directly using the exOS C-API"});
					if(selectedStructure.detail)
					{
						pickASType.push({label: "C Interface", detail:`AR C library which uses a C interface for the ${selectedStructure.label} datamodel`})
						pickASType.push({label: "C++ Class", detail:`AR C++ library which uses a C++ class for the ${selectedStructure.label} datamodel`})
					};

					vscode.window.showQuickPick(pickASType,{title:`Using datamodel: ${selectedStructure.label} - Select which template to use for Automation Runtime`}).then(selectedASType => {
						
						if(!selectedASType)
							return;

						let pickLinuxType = [];
						pickLinuxType.push({label: "C API", detail:"C application directly using the exOS C-API"});
						if(selectedStructure.detail) {
							pickLinuxType.push({label: "C Interface", detail:`C application which uses a C interface for the ${selectedStructure.label} datamodel`})
							pickLinuxType.push({label: "C++ Class", detail:`C++ application which uses a C++ class for the ${selectedStructure.label} datamodel`})
							pickLinuxType.push({label: "Python Module", detail:`Python application which uses a SWIG module for the ${selectedStructure.label} datamodel`})
							pickLinuxType.push({label: "JavaScript Module", detail:`nodejs JavaScript application which uses an N-API module for the ${selectedStructure.label} datamodel`})
							pickLinuxType.push({label: "Juliet Module", detail:`Juliet application which uses native functions directly using the exOS C-API ${selectedStructure.label} datamodel`})
						}
						
						vscode.window.showQuickPick(pickLinuxType,{title:`Using datamodel: ${selectedStructure.label} - Select which template to use for Linux`}).then(selectedLinuxType => {

							if(!selectedLinuxType)
								return;
						
							let pickPackaging = [];
							pickPackaging.push({label: "none", detail:"No packaging - files will run in the deployment folder on the target"});
							pickPackaging.push({label: "deb", detail:"Debian package - files will be packed and extracted at a given destination folder"});

							vscode.window.showQuickPick(pickPackaging,{title:`Select which packaging to use for Linux files`}).then(selectedPackaging => {

								if(!selectedPackaging)
									return;
							
								if(selectedPackaging.label == "deb") {
									vscode.window.showInputBox({prompt:"Set the .deb package destination folder on the target:", value:`/home/user/${selectedStructure.label.toLowerCase()}`}).then(destination => {
										if(!destination)
											return;

										createComponent(uri, selectedStructure, selectedASType, selectedLinuxType, selectedPackaging, destination);
									});
								}
								else {
									createComponent(uri, selectedStructure, selectedASType, selectedLinuxType, selectedPackaging, "");
								}
							});
						})
					});
				});
			}
		}
	});
	context.subscriptions.push(createPackage);

	let exportPackage  = vscode.commands.registerCommand('exos-component-extension.exportPackage', function (uri) {
		
		try {
			
			let exosExport = new ExosExport(uri.fsPath);

			/** 
			 * @type {ASConfiguration[]}
			 */
			let configurations = exosExport.getConfigurations();

			let pickConfiguration = [];

			for(let configuration of configurations) {
			
				pickConfiguration.push({label: configuration.name, detail: configuration.cpu, description: configuration.description});
			}

			vscode.window.showQuickPick(pickConfiguration,{title:`Select the configuration where the ${exosExport.name} package was last built`}).then(selectedConfiguration => {
			
				let homeFolder = vscode.Uri.file(os.homedir());
				vscode.window.showOpenDialog({title:"Create binary export", defaultUri: homeFolder, canSelectFolders:true, openLabel:`Create ${exosExport.name} export here` }).then(selectedUri => {
						
					for(let selected of selectedUri) {
						try{
							exosExport.exportPackage(selected.fsPath, selectedConfiguration.label);
							vscode.window.showInformationMessage(`Exported component to: ${path.join(selected.fsPath, exosExport._folderName)}`);
						} catch (error) {
							vscode.window.showErrorMessage(error);
						}
					}
					
				});

			});

		} catch (error) {
			vscode.window.showErrorMessage(error);
		}
		
	});
	context.subscriptions.push(exportPackage);

	let updateExosPkg = vscode.commands.registerCommand('exos-component-extension.updateExosPkg', function (uri) {
		
		try {
			let exospkg = new ExosPkg();
			let result = exospkg.parseFile(uri.fsPath);
			
			if(result.fileParsed) 
			{
				const editor = vscode.window.activeTextEditor;

				if (editor) {
					const document = editor.document;
					editor.edit(editBuilder => {
						let firstLine = document.lineAt(0);
						let lastLine = document.lineAt(document.lineCount - 1);
						let textRange = new vscode.Range(firstLine.range.start, lastLine.range.end);

						editBuilder.replace(textRange,exospkg.getContents());
						
					})
				}
			
				if(result.parseErrors == 0) {
					vscode.window.showInformationMessage(`Updated ${path.basename(uri.fsPath)} from version ${result.originalVersion} to ${exospkg.version}`);
					if(result.originalVersion == "1.0.0") {
						vscode.window.showInformationMessage(`Verify the sequence of the Install and Remove services`);
					}
				}
				else {
					vscode.window.showErrorMessage(`Caution! ${result.parseErrors} parse error(s) occured while parsing ${path.basename(uri.fsPath)}!`);
					vscode.window.showErrorMessage(`Undo and verify the original file manually!`);
				}
			}
			else {
				vscode.window.showErrorMessage(`${path.basename(uri.fsPath)} of version ${result.originalVersion} could not be parsed!`);
			}

		} catch (error) {
			vscode.window.showErrorMessage(error);	
		}
	});
	context.subscriptions.push(updateExosPkg);

	let updateComponent = vscode.commands.registerCommand('exos-component-extension.updateComponent', function (uri) {
		let exospkg = new ExosPkg();
		let result = exospkg.parseFile(uri.fsPath);
		
		if(result.fileParsed == true && result.parseErrors == 0) 
		{
			if(result.componentFound == true) {
				
				let pickForceOption = []
				pickForceOption.push({label: "Update", detail: "Update datamodel based files"});
				pickForceOption.push({label: "Update & Recreate", detail: "Update datamodel based files, create the files if needed"});
				pickForceOption.push({label: "Update All", detail: "Update all source files including main library/application sources"});
				pickForceOption.push({label: "Update All & Recreate", detail: "Update all source files including main library/application sources, create the files if needed"});

				vscode.window.showQuickPick(pickForceOption,{title:"Select datatype which becomes the new component datamodel"}).then(selectedForceOption => {

					if(!selectedForceOption)
						return;

					let updateAll = false;
					if(selectedForceOption.label.includes("Update All")) {
						updateAll = true;
					}

					let recreate = false;
					if(selectedForceOption.label.includes("Recreate")) {
						recreate = true;
					}

					try {
				
						/**
						 * @type {UpdateComponentResults}
						 */
						let results = {};
						let componentName = "";
						switch(exospkg.componentClass) {
							case "ExosComponentC":
								{
									let component = new ExosComponentCUpdate(uri.fsPath, updateAll);
									componentName = component._name;
									results = component.updateComponent(recreate);
								}
								break;
							case "ExosComponentNAPI":
								{
									let component = new ExosComponentNAPIUpdate(uri.fsPath, updateAll);
									componentName = component._name;
									results = component.updateComponent(recreate);
								}
								break;
							case "ExosComponentSWIG":
								{
									let component = new ExosComponentSWIGUpdate(uri.fsPath, updateAll);
									componentName = component._name;
									results = component.updateComponent(recreate);
								}
								break;
							case "ExosComponentJS":
							case "ExosComponentPython":
								vscode.window.showInformationMessage(`Component is deploy-only and does not support updating`);
								break;
							case "ExosComponentJuliet":
								{
									let component = new ExosComponentJulietUpdate(uri.fsPath, updateAll);
									componentName = component._name;
									results = component.updateComponent(recreate);
								}
								break;
							default:
								vscode.window.showErrorMessage(`Component can not be updated: class ${exospkg.componentClass} can not be found`);
								return;
						}

						if(results.parseResults.componentErrors.length > 0) {
							vscode.window.showErrorMessage(`Component ${componentName} can not be updated: Errors encountered during update`);
							for(let error of results.parseResults.componentErrors) {
								vscode.window.showErrorMessage(error);
							}
						}
						else {
							vscode.window.showInformationMessage(`Component ${componentName} updated - ${results.updateResults.filesUpdated} file(s) updated`);

							if(results.updateResults.filesNotFound > 0) {
								vscode.window.showErrorMessage(`During the update, ${results.updateResults.filesNotFound} file(s) could not be found`);
							}

							if(results.updateResults.foldersNotFound > 0) {
								vscode.window.showErrorMessage(`During the update, ${results.updateResults.foldersNotFound} folder(s) could not be found`);
							}
						}

					}
					catch(error) {
						vscode.window.showErrorMessage(error);
					}
				});

			}
			else {
				vscode.window.showErrorMessage(`Component can not be updated: missing ComponentGenerator entry`);	
			}
		}
		else {
			vscode.window.showErrorMessage(`File ${path.basename(uri.fsPath)} can not be parsed! Parse error(s): ${result.parseErrors}`);
		}
	});
	context.subscriptions.push(updateComponent);

}
exports.activate = activate;

// this method is called when your extension is deactivated
function deactivate() {}

module.exports = {
	activate,
	deactivate
}
