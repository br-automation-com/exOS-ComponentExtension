const path = require('path');
const Mocha = require('mocha');
const glob = require('glob');

function run() {
	// Create the mocha test
	const mocha = new Mocha({
		ui: 'tdd',
		color: true
	});

	const testsRoot = path.resolve(__dirname, '..');

	return new Promise((c, e) => {
		glob('**/**.test.js', { cwd: testsRoot }, (err, files) => {
			if (err) {
				return e(err);
			}
			
			console.log("Control which tests to run in " + path.resolve(__dirname, "index.js"));
			// Uncomment below to filter away certain tests
			//files = files.filter(item => item !== 'suite/extension.test.js')
			//files = files.filter(item => item !== 'suite/datamodel_generation/datamodel.generation.test.js')
			//files = files.filter(item => item !== 'suite/template_generation/template.generation.test.js')

			// Add files to the test suite
			files.forEach(f => mocha.addFile(path.resolve(testsRoot, f)));

			try {
				// Run the mocha test
				mocha.run(failures => {
					if (failures > 0) {
						e(new Error(`${failures} tests failed.`));
					} else {
						c();
					}
				});
			} catch (err) {
				console.error(err);
				e(err);
			}
		});
	});
}

module.exports = {
	run
};
