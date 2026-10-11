@echo off
rem Abre o jogo recompilado com o desenho nativo ao vivo para jogar com o teclado (-Auto 0: sem o roteiro automatico de pad).
rem Teclado: setas = analogico, Espaco = X, Z = quadrado, X = triangulo, S = bola, Enter = Start, I/J/K/L = D-pad.
rem Outros argumentos passam direto para o run_live.bat (ex.: -Stats 1  ou  -Build build).
call "%~dp0run_live.bat" -Auto 0 %*
exit /b %errorlevel%
