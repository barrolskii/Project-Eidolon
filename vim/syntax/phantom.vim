syntax keyword phantomTodos TODO XXX FIXME NOTE

" Language specific keywords
syntax keyword phantomKeywords
	\ var
	\ if
	\ loop
	\ func
	\ exit

" Match Phantom number types
syntax match phantomNumber "\v<\d+>"
syntax match phantomNumber "\v<\d+\.\d+>"

" Match strings
syntax region phantomString start=/"/ skip=/\\"/ end=/"/ oneline contains=phantomInterpolatedWrapper
syntax region phantomInterpolatedWrapper start="\v\\\(\s*" end="\v\s*)" contained containedin=phantomString contains=phantomInterpolatedString
syntax match phantomInterpolatedString "\v\w+(\(\))?" contained containedin=phantomInterpolatedWrapper

" Set highlights
highlight default link phantomTodos Todo
highlight default link phantomShebang Comment
highlight default link phantomComment Comment
highlight default link phantomMarker Comment

highlight default link phantomString String
highlight default link phantomInterpolatedWrapper Delimiter
highlight default link phantomNumber Number
highlight default link phantomBoolean Boolean

highlight default link phantomOperator Operator
highlight default link phantomKeywords Keyword
highlight default link phantomAttributes PreProc
highlight default link phantomStructure Structure
highlight default link phantomType Type
highlight default link phantomImports Include
highlight default link phantomPreprocessor PreProc
