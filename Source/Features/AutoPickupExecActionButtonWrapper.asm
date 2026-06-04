EXTERN CheckExecActionButtonParamFilters :PROC
EXTERNDEF ExecuteActionButtonParamProxyReturn :QWORD
.code
ALIGN 16

ExecActionButtonParamProxyWrapper PROC
push rcx
push rdx
push r8
push r9
sub rsp, 58h
movaps xmmword ptr [rsp+10h], xmm0
movaps xmmword ptr [rsp+20h], xmm1
movaps xmmword ptr [rsp+30h], xmm2

call CheckExecActionButtonParamFilters

movaps xmm0, xmmword ptr [rsp+10h]
movaps xmm1, xmmword ptr [rsp+20h]
movaps xmm2, xmmword ptr [rsp+30h]
add rsp, 58h
pop r9
pop r8
pop rdx
pop rcx
cmp al, -1
jz normal_flow
ret

normal_flow:
jmp [ExecuteActionButtonParamProxyReturn]

ExecActionButtonParamProxyWrapper ENDP

END
