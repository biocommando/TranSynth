include @/lib/sub_extensions.js

inline sub random
    #random = _random_
    #random = (#random % 10000) / 10000
return

sub generate_random_array(#max)
    for #i in 1 to #max
        call random
        array_set random at #i = #random
    loop
return