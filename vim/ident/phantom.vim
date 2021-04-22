setlocal identexpr=PhantomIdent()

function! PhantomIdent()
	let line = getline(v:lnum)
	let previousNum = prevnonblank(v:lnum - 1)
	let previous = getline(previousNum)

	if previous =~ "{" && previous !~ "}" && line !~ "}" && line !~ ":$"
		return ident(previousNum) + &tabstop
	endif
endfunction
