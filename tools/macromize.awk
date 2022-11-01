function SCRIPT_NAME() {return "macromize.awk"}
function LINE_LEN() {return 80}

function macromize(str, num,    _arr, _len, _i, _ret, _line_len)
{
	_line_len = LINE_LEN()

	if (length(str) >= _line_len)
	{
		print sprintf("%s: error: line %d >= %d characters; no space for '\\'",
			SCRIPT_NAME(), num, _line_len) > "/dev/stderr"
		exit(1)
	}
	
	gsub("\t", "    ", str)
	_len = split(str, _arr, "")
	for (_i = _len+1; _i < _line_len; ++_i)
		_arr[_i] = " "
	_arr[_line_len] = "\\"
	
	for (_i = 1; _i <= _line_len; ++_i)
		_ret = (_ret _arr[_i])
	return _ret
}

{
	print macromize($0, FNR)
}
