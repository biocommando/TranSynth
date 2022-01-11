const transpiler = require('D:/code/js/SimpleScriptCompiler/transpiler')
const fs = require('fs')

const chimpfiles = fs.readdirSync('.').filter(x => x.endsWith('.chimp'))

fs.writeFileSync('all-in-one.txt', chimpfiles.map(x => `include $/${x}`).join('\n'))

transpiler.transpile('all-in-one.txt', '../Data/scripts.txt')