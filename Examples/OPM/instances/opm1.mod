#=============================#
#      Modelo Polad - AMPL    #
#=============================#

###############################
#          Producao
###############################

# Esteril/Minerio
set EM;

# Producao Minima (t/h)
param pl{m in EM} >=0;

# Producao Recomendada (t/h)
param pr{m in EM} >=0;

# Producao Maxima (t/h)
param pu{m in EM} >=0;

# Conversao de EM para {0,1}
param parEstMin{i in EM} binary;

# -----------------------------
#           Variaveis
# -----------------------------

# desvio negativo para a producao 
var dnp{m in EM} >=0;

# desvio positivo para a producao 
var dpp{m in EM} >=0;

# -----------------------------
#            Pesos
# -----------------------------

# Peso por desvio negativo para a producao 
param wnp{m in EM} >=0;

# Peso por desvio positivo para a producao 
param wpp{m in EM} >=0;


###############################
#          Parametros
###############################

# Parametros
set Parametros;

# Teor Minimo para o Parametro j
param tl{j in Parametros} >=0;

# Teor Recomendado para o Parametro j
param tr{j in Parametros} >=0;

# Teor Maximo para o Parametro j
param tu{j in Parametros} >=0;

# -----------------------------
#           Variaveis
# -----------------------------

# desvio negativo para o parametro j
var dnm{j in Parametros} >=0;

# desvio positivo para o parametro j
var dpm{j in Parametros} >=0;

# -----------------------------
#            Pesos
# -----------------------------

# Peso por desvio negativo para o parametro j
param wnm{j in Parametros} >=0;

# Peso por desvio positivo para o parametro j
param wpm{j in Parametros} >=0;

###############################
#           Frentes
###############################

# Frentes
set Frentes;

# Massa de cada frente
param qu{i in Frentes} >=0;

# Frente de esteril ou minerio
param estMin{i in Frentes} binary;

# Tempo de ciclo para a Frente i
param tempo{i in Frentes} >=0;

# -----------------------------
#           Variaveis
# -----------------------------

# Ritmo de lavra na Frente i
var x{i in Frentes} >= 0;

###############################
#           Carregadeiras
###############################

# Carregadeiras
set Carregadeiras;

# Producao Maxima da Carregadeira k (em t/h)
param cMax{k in Carregadeiras} >= 0;

# Producao Minima da Carregadeira k (em t/h)
param cMin{k in Carregadeiras} >= 0;

###############################
#           Caminhoes
###############################

# Caminhoes
set Caminhoes;

# Taxa maxima de utilizacao
param txUtilCam >= 0;

# Capacidade do Caminhao l
param capacidade{l in Caminhoes} >= 0;

# -----------------------------
#           Variaveis
# -----------------------------

# Desvio negativo de utilizacao do caminhao
var dnu{l in Caminhoes} >= 0;

# Desvio positivo de utilizacao do caminhao
var dpu{l in Caminhoes} >= 0;

# = 1 se Caminhao l foi utilizado, 0 caso contrario
var usou{l in Caminhoes} binary;


###############################
#           Matrizes
###############################

# Teor dos Parametros das Frentes
param teor{i in Frentes, j in Parametros} >= 0;

# = 1 se Carregadeira k opera na Frente i, 0 caso contrario
var opera{i in Frentes, k in Carregadeiras} binary;

# = 1 se Caminhao l eh compativel com Carregadeira k, 0 caso contrario
param comp{l in Caminhoes, k in Carregadeiras} binary;


# Numero de viagens que o Caminhao l realiza na Frente i
var viagens{i in Frentes, l in Caminhoes} >=0 integer;


###############################
#     Funcao de Avaliacao
###############################

minimize fo: sum{j in Parametros}(wnm[j]*dnm[j] + wpm[j]*dpm[j]) + sum{m in EM}(wnp[m]*dnp[m] + wpp[m]*dpp[m]) + sum{l in Caminhoes}(usou[l]);

###############################
#          Restricoes
###############################

#-----------------------------#
#          Producao           #
#-----------------------------#

# Producao maxima de minerio
s.t. PU{m in EM}: sum{i in Frentes: estMin[i]=parEstMin[m]} x[i] <= pu[m];

# Producao minima de minerio
s.t. PL{m in EM}: sum{i in Frentes: estMin[i]=parEstMin[m]} x[i] >= pl[m];

# Producao recomendada de minerio
s.t. PR{m in EM}: sum{i in Frentes: estMin[i]=parEstMin[m]} x[i] + dnp[m] - dpp[m] = pr[m];


#-----------------------------#
#         Parametros          #
#-----------------------------#

# Para cada parametro de controle j, deve ser satisfeito o limite inferior de especificacao
s.t. LimiteMin{j in Parametros}: sum{i in Frentes: estMin[i]!=0}((teor[i,j] - tl[j])*x[i]) >= 0;

# Para cada parametro de controle j, deve ser satisfeito o limite suferior de especificacao
s.t. LimiteMax{j in Parametros}: sum{i in Frentes: estMin[i]!=0}((teor[i,j] - tu[j])*x[i]) <= 0;

# Atendimento de meta para o parametro de controle j
s.t. Meta{j in Parametros}: sum{i in Frentes: estMin[i]!=0}((teor[i,j] - tr[j])*x[i]) + dnm[j] - dpm[j] = 0;


#-----------------------------#
#        Carregadeiras        #
#-----------------------------#

# Deve haver no maximo uma Carregadeira k operando em cada Frente i
s.t. FrenteCarregadeira{i in Frentes}: sum{k in Carregadeiras} opera[i,k] <= 1;

# Cada Carregadeira k deve operar no maximo em uma Frente i
s.t. CarregadeiraFrente{k in Carregadeiras}: sum{i in Frentes} opera[i,k] <= 1;

# O ritmo de lavra da Frente i deve ser maior do que a produtividade Minima (cMin) da Carregadeira k, alocada a frente
s.t. CMIN{i in Frentes}: x[i] >= sum{k in Carregadeiras} opera[i,k]*cMin[k];

# O ritmo de lavra da Frente i deve ser menor do que a produtividade Maxima (cMax) da Carregadeira k, alocada a frente
s.t. CMAX{i in Frentes}: x[i] <= sum{k in Carregadeiras} opera[i,k]*cMax[k];


#-----------------------------#
#          Caminhoes          #
#-----------------------------#

# Cada Caminhao l deve realizar viagens apenas a uma Frente i que esteja alocada uma carregadeira compativel (???)
s.t. Compatibilidade{i in Frentes, l in Caminhoes}: viagens[i,l] * tempo[i] <= 60 * (sum{k in Carregadeiras : comp[l,k] != 0} opera[i,k]);

# Cada Caminhao l deve obedecer a taxa maxima de utilizacao
s.t. TaxaUtilizacao{l in Caminhoes}: (sum{i in Frentes} viagens[i,l]*tempo[i])/60 <= txUtilCam;

# Variavel binaria de uso do Caminhao l
s.t. Usou{l in Caminhoes}: usou[l] >= (sum{i in Frentes} viagens[i,l]*tempo[i])/60;

#-----------------------------#
#           Frentes           #
#-----------------------------#

# O ritmo de lavra da Frente i deve ser igual a producao realizada pelos caminhoes alocados a frente
s.t. Lavra{i in Frentes}: x[i] = (sum{l in Caminhoes} viagens[i,l]*capacidade[l]);

