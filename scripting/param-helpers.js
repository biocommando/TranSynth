'macro js_ignore'
/*
include @/lib/arr.js
*/
'macro end'

'macro copy_param'
str => {
    const [arr, from, to] = str.split(',').map(x => x.trim())
    return [
        `[] #temp = ${arr}[${from}]`,
        `[] ${arr}[${to}] = #temp`
    ]
}
'macro end'

'macro for_each_param'
str => {
    const allParams = ['Tri', 'Saw', 'Sqr',
        'Wt', 'WtW', 'Dtn', 'Cut', 'Res',
        'Dist', 'LFO_f', 'ModPt', 'ModFl',
        'Vol', 'Noise']
    const commands = str.split(';').map(x => x.trim())
    let params = commands.splice(0, 1)[0].split(',').map(x => x.trim())
    if (params[0] === 'ALL')
        params = allParams
    const re = new RegExp('#PARAM', 'g')
    const ret = []
    params.forEach(x => {
        commands.forEach(y => {
            ret.push(y.replace(re, x))
        })
    })
    return ret
}
'macro end'

'macro for_each_param_start'
str => {
    globals.set('for_each_param__state', 'for_each_param ' + str)
    return []
}
'macro end'

'macro |'
str => {
    const s = globals.get('for_each_param__state')
    globals.set('for_each_param__state', s + ' ; ' + str)
    return []
}
'macro end'

'macro for_each_param_end'
str => {    
    return [
        globals.get('for_each_param__state')
    ]
}
'macro end'