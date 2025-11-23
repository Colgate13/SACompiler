section .data
    ; Formato para printf (precisamos de newline)
    fmt_int: db "%d", 10, 0    ; 10 = '\n', 0 = terminador

section .bss
    ; Variáveis
    a: resq 1          ; reserva 8 bytes (quadword) para 'a'
    b: resq 1          ; reserva 8 bytes para 'b'
    result: resq 1     ; reserva 8 bytes para 'result'

section .text
    global main
    extern printf      ; vamos usar printf da libc

main:
    ; Prólogo da função
    push rbp
    mov rbp, rsp
    
    ; a = 5
    mov qword [a], 5
    
    ; b = 3
    mov qword [b], 3
    
    ; ========================================
    ; result = (a + b)
    ; ========================================
    mov rax, [a]       ; carrega 'a' em rax
    add rax, [b]       ; soma 'b' com rax
    mov [result], rax  ; salva em 'result'
    
    ; print(result)
    mov rdi, fmt_int   ; primeiro parâmetro: formato
    mov rsi, [result]  ; segundo parâmetro: valor
    xor rax, rax       ; rax=0 (sem registradores float)
    call printf
    
    ; ========================================
    ; result = (a + b) * 2
    ; ========================================
    mov rax, [a]       ; carrega 'a'
    add rax, [b]       ; a + b
    imul rax, 2        ; multiplica por 2
    mov [result], rax
    
    ; print(result)
    mov rdi, fmt_int
    mov rsi, [result]
    xor rax, rax
    call printf
    
    ; ========================================
    ; result = ((a + b) * 2) + 1
    ; ========================================
    mov rax, [a]       ; carrega 'a'
    add rax, [b]       ; a + b
    imul rax, 2        ; (a + b) * 2
    add rax, 1         ; + 1
    mov [result], rax
    
    ; print(result)
    mov rdi, fmt_int
    mov rsi, [result]
    xor rax, rax
    call printf
    
    ; ========================================
    ; result = 10 - (a + b)
    ; ========================================
    mov rax, [a]       ; carrega 'a'
    add rax, [b]       ; a + b
    mov rbx, 10        ; coloca 10 em rbx
    sub rbx, rax       ; 10 - (a + b)
    mov [result], rbx
    
    ; print(result)
    mov rdi, fmt_int
    mov rsi, [result]
    xor rax, rax
    call printf
    
    ; ========================================
    ; result = (a * b) + (a - b)
    ; ========================================
    mov rax, [a]       ; carrega 'a'
    imul rax, [b]      ; a * b
    mov rbx, rax       ; salva (a * b) em rbx
    
    mov rax, [a]       ; carrega 'a' novamente
    sub rax, [b]       ; a - b
    
    add rbx, rax       ; (a * b) + (a - b)
    mov [result], rbx
    
    ; print(result)
    mov rdi, fmt_int
    mov rsi, [result]
    xor rax, rax
    call printf
    
    ; Epílogo e retorno
    mov rax, 0         ; código de retorno = 0
    pop rbp
    ret
