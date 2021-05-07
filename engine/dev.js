const execa = require('execa')
const esbuild = require('esbuild')
const chokidar = require('chokidar')
const debounce = require('lodash/debounce')
const dayjs = require('dayjs')
const chalk = require('chalk')

function log(...args) {
  args.push(chalk.cyan(dayjs().format('HH:mm:ss')))
  const message = args.join(' ')
  const separator = '-'.repeat(message.length)
  console.log(separator)
  console.log(message)
  console.log(separator)
}

function makeExe(cmd) {
  const { stderr, stdout } = execa.command(cmd)
  stderr && stderr.on('data', (chunk) => {
    console.log(chalk.redBright(chunk.toString()))
  })
  stdout.pipe(process.stdout)
}

function nativeBuild(event, path) {
  log(chalk.cyan.bold(event.toUpperCase()), path)
  makeExe('make')
}

function runTest() {
  log(chalk.yellow.bold('ESBUILD'))
  makeExe('./build/test')
}

function esBuildWatch(cb) {
  esbuild.build({
    entryPoints: ['./test/index.js'],
    outfile: './build/out.js',
    bundle: true,
    watch: {
      onRebuild(error, result) {
        if (error) {
          log('Error')
        } else {
          cb()
        }
      },
    },
  })
}

function main() {
  const debouncedNativeBuild = debounce(nativeBuild, 300)

  chokidar.watch(['./*.cpp', './*.h']).on('all', (event, path) => {
    debouncedNativeBuild(event, path)
  })

  esBuildWatch(runTest)
}

main()