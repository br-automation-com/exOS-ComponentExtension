const assert = require('assert');
const path = require('path');
const fse = require('fs-extra');
const dircompare = require('dir-compare');
const { Test } = require('mocha');
const { ExosComponentC } = require('../../../src/components/exoscomponent_c');

// You can import and use all API from the 'vscode' module
// as well as import your extension to test it
const vscode = require('vscode');
const file = require('fs-extra/lib/ensure/file');
const { parseConfigFileTextToJson } = require('typescript');
const { stringify } = require('querystring');
// const myExtension = require('../extension');

suite('Buffered FUB generation and run tests (<name> <AR side> <Linux side>)', () => {
    
    // Call the generator and (maybe manually) test it
    
    // A test should be made for each AR variant, Linux kept in c
    // c-api, c-static, cpp

    // test title is used to set typ name and other options
    // it is also used for folder names (spaces replaced with underscores)
    
    // we could loop and find all .typ if we wanted
    // but lets keep it as an active choice to get them tested
    // The names must match both the filename and the STRUCT name
    typNames = [
        "BufferFub",
        "BufferFub2"
    ];

    // start with deleting everything previously generated
    genPath = path.resolve(__dirname, '../buffered_fub/generated');
    fse.emptyDirSync(genPath);

    typNames.forEach(typName => {
        test(`${typName} c-api c-api`, function() {
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves. 
        });
/*
        test(`${typName} c-static c-api`, function() { // py is actually only for the title as ExosComponentSWIG doesnt use the linux template in the options
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves.
        });

        test(`${typName} cpp c-api`, function() { // napi is actually only for the title as ExosComponentNAPI doesnt use the linux template in the options
            genAndCompare(this.test.title, function() {
                let templateC = new ExosComponentC(typFile, selectedStructure.label, selectedOptions);
                templateC.makeComponent(genPath);
            });
            this.timeout(0); // avoid Error: Timeout of 2000ms exceeded. For async tests and hooks, ensure "done()" is called; if returning a Promise, ensure it resolves.
        });
        */
    });


    // general function to test a specific typ file
    // generate and compare to expected output
    function genAndCompare(title, generator) {

        splitTitle = title.split(" ");

        genPath = path.resolve(__dirname, '../buffered_fub/generated');

        // start with deleting everything previously generated
        fse.emptyDirSync(genPath);

        typFolder = path.resolve(__dirname, '../typFiles/');

        typFile = path.resolve(typFolder, `${splitTitle[0]}.typ`);
        typName = path.parse(typFile).name;

        // setup variables used by generator
        selectedStructure = {label: typName, detail: "buffered.fub.test.js"};
        selectedOptions = {
            packaging: "deb", templateLinux: splitTitle[2], templateAR: splitTitle[1], destinationDirectory: ""
        }

        // update genPath with more info before calling generator
        //genPath = path.resolve(genPath, `${typName}_${selectedOptions.templateAR}_${selectedOptions.templateLinux}`)
        //fse.emptyDirSync(genPath); // creates if not there yet

        // call the generator
        generator();

        // test if we generated something
        genPath = path.resolve(__dirname, '../buffered_fub/generated/', typName)
        assert.equal(fse.existsSync(genPath), true, `${genPath} doesnt exist`);

        // moved to a type named folder
        //genNamedPath = path.resolve(__dirname, '../buffered_fub/generated/', `${typName}_${selectedOptions.templateAR}_${selectedOptions.templateLinux}`);
        //fse.moveSync(genPath, genNamedPath);
        
        // Move to AS project for testing
        asPath = path.resolve(__dirname, '../AS/Project/Logical/BufferedFub/', `${typName}_${selectedOptions.templateAR}_${selectedOptions.templateLinux}`);
        fse.emptyDirSync(asPath);
        fse.moveSync(genPath, asPath, { overwrite: true });

        // TODO: inject some code to runtime test this

        // ### In UpdateBufferSampleSingle and UpdateBufferSampleAll actions
        // ### below '(* Your code here *)'
        /*
            // test code begin
            BufferedSampleRecv[BufferedSampleRecvIdx] := BufferFub_0.bufferedSample;
            BufferedSampleRecvIdx := BufferedSampleRecvIdx + 1;
            IF BufferedSampleRecvIdx > 19 THEN
                BufferedSampleRecvIdx := 0;
            END_IF
            // test code end
        */

        // ### BufferFub.var:
        /*
            BufferedSampleRecvIdx : USINT; (*test code*)
            testState : UINT; (*test code*)
            BufferedSampleRecv : ARRAY[0..19] OF UDINT; (*test code*)
        */
    
        // ### In BufferFub.st replace 'UpdateBufferSampleSingle;' with:
        /*
            // test code begin
            CASE testState OF
                0:
                    // nothing
                5:
                    // startup
                    BufferFubCyclic_0.Enable := TRUE;
                    IF BufferFubCyclic_0.Connected THEN
                        BufferFubCyclic_0.Start := TRUE;
                        testState := 10;
                    END_IF
                10:
                    IF BufferFubCyclic_0.Operational THEN
                        testState := 20;
                    END_IF
                20:
                    // tell linux to burst
                    BufferFub_0.setup.sampleCount := 12;
                    BufferFub_0.setup.sampleDelay := 0;
                    testState := 25;
                25:
                    // wait a scan to let setup arrive first
                    BufferFub_0.cmdSendBurst := TRUE;
                    testState := 30;
                50:
                    UpdateBufferSampleAll;
                    testState := 0;
                60:
                    UpdateBufferSampleSingle;
                    testState := 0;
            END_CASE;
            // test code end
        */

        // ### in Linux/bufferfub.c add:
        /*
            typedef struct {
                BufferFub *data;
                exos_datamodel_handle_t* bufferfub;
                exos_dataset_handle_t* bufferedsample;
                exos_dataset_handle_t* setup_dataset;
                exos_dataset_handle_t* cmdsendburst;
            } application_info_t;

        // in case EXOS_DATASET_EVENT_UPDATED:
            if (*cmdsendburst){
                
                application_info_t *application = (application_info_t *)dataset->user_context;
                if (NULL != application)
                {
                    VERBOSE("Send burst, sampleCount=%u, sampleDelay=%u  ", application->data->setup.sampleCount, application->data->setup.sampleDelay);
                    for (size_t i = 0; i < application->data->setup.sampleCount; i++)
                    {
                        application->data->bufferedSample = i + 1;
                        exos_dataset_publish(application->bufferedsample);
                        usleep(application->data->setup.sampleDelay);
                    }
                }
            }

        // in main():
            application_info_t myapplication = {&data, &bufferfub, &bufferedsample, &setup_dataset, &cmdsendburst};
        // and:
            cmdsendburst.user_context = &myapplication; //user defined
        */
    }
});
