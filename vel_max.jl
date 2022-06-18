#  função que calcula a velocidade máxima do algoritmo
# Obs: é possivel executar o código no repl.it, caso nao tenha o julia instalado

# funções auxiliares
tcap(r, c, n, tg) = r * c * log(2^(n + 1)) + tg
tconv(n, t_clk) = n * t_clk

# função principal, que calcula dentre outras coisas, a velocidade máxima
# r = resistencia interna + externa
# c = capacitância
# n = número de bits
# tg = tempo de guarda / segurança
# t_clk = periodo de clock
function velmax(r, c, n, tg, t_clk)

  t_cap = tcap(r, c, n, tg)
  t_conv = tconv(n, t_clk)
  t_min = t_cap + t_conv
  s_time = t_cap / t_clk
  vel_max = 1 / t_min

  println("tcap  = ", t_cap)
  println("tconv = ", t_conv)
  println("t_clk = ", t_clk)
  println("t_min = ", t_min)
  println("velocidade máxima = ", vel_max)
  println("sampling time = ", s_time)
end

# adotamos 8 bits de amostragem, pois trará a maior velocidade permitida pela msp430
# velmax(11.8e3, 25e-12, 8, 800e-9, 1 / 5e6)
# velmax(11.8e3, 25e-12, 12, 800e-9, 1 / 32768)

print(3.3 / 2)

# RESULTADOS
# tcap = 2.640305764386655e-6
# tconv = 1.6e-6
# t_clk = 2.0e-7
# t_min = 4.2403057643866545e-6
# v_max = 235832.0497542343
# s_time = 13.201528821933275
