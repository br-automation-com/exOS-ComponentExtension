const assert = require('assert');
const path = require('path');
const fse = require('fs-extra');
const dircompare = require('dir-compare');
const { Test } = require('mocha');
const { ExosComponentC } = require('../../../src/components/exoscomponent_c');
const { ExosComponentNAPI } = require('../../../src/components/exoscomponent_napi');
const { ExosComponentSWIG } = require('../../../src/components/exoscomponent_swig');

// You can import and use all API from the 'vscode' module
// as well as import your extension to test it
const vscode = require('vscode');
const file = require('fs-extra/lib/ensure/file');
const { parseConfigFileTextToJson } = require('typescript');
const { stringify } = require('querystring');
// const myExtension = require('../extension');

suite('Template generation tests (<name> <AR side> <Linux side>)', () => {
    
    // Call the generator and compare it to expected output
    
    // if the 'expected' folder doesnt exist an error is reported and the output is copied to the 'expected' folder
    // this makes it easier to make new tests with new typ files.
    // run them once and "acknowledge"/ignore the error and it will pass on the next run
    
    // if the newly generate output does not match the expected an error is reported
    // and the output is copied to an 'unexpected' folder
    // again this makes it easier in case one wants to use the newly generated as the new expected (manual copy is needed)

    // A test should be made for each variant, not all combinations but just so we have all the different ones covered
    // c-api, c-static, cpp, py and napi

    // test title is used to set typ name and other options
    // it is also used for folder names (spaces replaced with underscores)
    
    // we could loop and find all .typ if we wanted
    // but lets keep it as an active choice to get them tested
    // The names must match both the filename and the STRUCT name
    typNames = [
        "StringAndArray",
        "ros_topics_typ"
    ];

    typNames.forEach(typName => {
        test(`${typName} c-api c-api`, function() {
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves. 
        });

        test(`${typName} c-api cpp`, function() {
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves. 
        });

        test(`${typName} c-static c-static`, function() {
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves. 
        });

        test(`${typName} c-static py`, function() { // py is actually only for the title as ExosComponentSWIG doesnt use the linux template in the options
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentSWIG(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves.
        });

        test(`${typName} cpp napi`, function() { // napi is actually only for the title as ExosComponentNAPI doesnt use the linux template in the options
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentNAPI(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves.
        });
    });

    // clear out entire unexpected path to avoid having old stuff 
    unexpectedBasePath = path.resolve(__dirname, '../template_generation/unexpected/');
    fse.emptyDirSync(unexpectedBasePath);

    // general function to test a specific typ file
    // generate and compare to expected output
    function genAndCompare(title, generator) {

        splitTitle = title.split(" ");

        genPath = path.resolve(__dirname, '../template_generation/generated/');

        // start with deleting everything previously generated
        fse.emptyDirSync(genPath);

        typFolder = path.resolve(__dirname, '../typFiles/');

        typFile = path.resolve(typFolder, `${splitTitle[0]}.typ`);
        typName = path.parse(typFile).name;

        // setup variables used by generator
        selectedStructure = {label: typName, detail: "template.generation.test.js"};
        selectedOptions = {
            packaging: "deb", templateLinux: splitTitle[2], templateAR: splitTitle[1], destinationDirectory: ""
        }

        // call the generator
        generator();

        // now compare the newly generated output with the expected output
        dirOptions = {
            compareContent: true,
            compareFileSync: dircompare.fileCompareHandlers.lineBasedFileCompare.compareSync,
            ignoreLineEnding: true
        };
        
        genPath = path.resolve(__dirname, '../template_generation/generated/', typName)
        assert.equal(fse.existsSync(genPath), true, `${genPath} doesnt exist`);

        expectedPath = path.resolve(__dirname, '../template_generation/expected/', `${typName}_${selectedOptions.templateAR}_${selectedOptions.templateLinux}`)

        if (!fse.existsSync(expectedPath)) {
            // copy the newly generated as the new expected and report an error to inform the user
            // next test will go through
            fse.copySync(genPath, expectedPath);
            assert.equal(false, true, `${expectedPath} didnt exist. Generated path has been copied so next test run will pass. Make sure that the generated is actually also what is expected`);
        }

        compRes = dircompare.compareSync(genPath, expectedPath, dirOptions);
        if (compRes.differences > 0) {
            unexpectedPath = path.resolve(unexpectedBasePath, `${typName}_${selectedOptions.templateAR}_${selectedOptions.templateLinux}`)
            fse.copySync(genPath, unexpectedPath);
            assert.equal(compRes.differences, 0, `Generated output for ${typName} differs from expected (if the output is the new expected, manually copy it from the unexpected folder)`);
        }
    }
});
