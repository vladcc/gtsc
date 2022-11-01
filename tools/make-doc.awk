#!/usr/bin/awk -f

function out() {
	if (!non_empty_seen && !match($0, "^[[:space:]]*$"))
		non_empty_seen = 1

	if (non_empty_seen)
		print
}

BEGIN                   {ifdefs = 0}
/#define.*_H$/          {next}
/#include[[:space:]]+</ {next}
/#include[[:space:]]+"/ {out(); next}
/#if/                   {if (++ifdefs != 1) out(); next}
/#endif/                {if (--ifdefs != 0) out(); next}
                        {out()}
