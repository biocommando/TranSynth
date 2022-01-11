
// set_linked arr idx value
'macro set_linked'
str => {
    const [arr, idx, value] = str.split(/\s\s*/).map(x => x.trim())
    return [
        `array_set ${arr} at ${idx} = ${value}`,
        `array_get link_mode at ${idx} to #link_tweaked`,
        `if #link_tweaked > 0`,
        `for #set_link_i in 1 to 4`,
        `array_get link_mode at #set_link_i to #link_other`,
        `if_then_ignore_warning #link_other == #link_tweaked`,
        `array_set ${arr} at #set_link_i = ${value}`,
        `loop`,
        `endif`
    ]
}
'macro end'