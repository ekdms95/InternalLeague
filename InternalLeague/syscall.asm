.MODEL flat, C
.code

ZwCopyVirtualProtect PROC public
	mov eax, 50h
	mov edx, 77E79F60h
	call edx
	ret

	;mov eax, 50h
	;mov edx, 77E79F60h
	;call edx
	;ret 14h
	;mov eax, 50h
	;mov edx, 77E79F60h
	;call edx
	;ret 14h
ZwCopyVirtualProtect ENDP

end