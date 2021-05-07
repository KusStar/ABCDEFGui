const fs = require('fs')
const execa = require('execa')
const esbuild = require('esbuild')
const chokidar = require('chokidar');

chokidar.watch(['./*.cpp', './*.h']).on('all', (event, path) => {
  console.log('--------------')
  console.log('Make')
  console.log('--------------')
  const exe = execa('make');
  exe.stderr.pipe(process.stdout)
  exe.stdout.pipe(process.stdout)
});

build()

let times = 0
function make() {
  let cmd
  if (!fs.existsSync('./build/test')) {
    cmd = 'make'
  } else {
    cmd = './build/test'
  }
  console.log('--------------')
  console.log('--------------', times++)
  execa(cmd).stdout.pipe(process.stdout);
}

function build() {
  esbuild.build({
    entryPoints: ['./test/index.js'],
    outfile: './build/out.js',
    bundle: true,
    watch: {
      onRebuild(error, result) {
        if (error) {
          console.log('--------------', 'stopped')
          result.stop()
        }
        else {
          make()
        }
      },
    },
  }).then(result => {
    // Call "stop" on the result when you're done
    // result.stop()
    make()
  })
}
