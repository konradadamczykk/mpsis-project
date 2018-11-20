# MPSiS 2018/2019 - Data center optimization model
# Task MILP

# Parametry modelu
# l1 - Koszt m2 dzialki (int)
# l2 - Liczba producentow serwerow (int)
# L2[i] - Koszt serwera producenta i (z l2) (int)
# l3 - Liczba producentow rackow (int)
# L3[i] - Koszt racka producenta j (z l3) (int)
# l4 - Koszt kWh (int)

param l1, integer, >= 0;
param l2, integer, >= 0;
param l3, integer, >= 0;
param l4, integer, >= 0;

set SERV, default {1..l2};
set RACK, default {1..l3};

param L2{serv in SERV}, >= 0;
param L3{rack in RACK}, >= 0;

# Parametry ograniczen
# m1 - Maksymalna powierzchnia
# m2 - Minimalna liczba wszystkich maszyn
# M2[i] - Maksymalna liczba serwerow oferowanych przez producenta i
# m3 - Minimalna liczba wszystkich rackow (zalezna od x1 i x2)
# M3[i] - Maksymalna liczba rackow oferowanych przez producenta i
# m4 - Maksymalny miesieczny pobor pradu (zalezy od x2)

param m1, integer, >= 0;
param m2, integer, >= 0;
param M2{serv in SERV}, integer, >= 0;
param m3, integer, >= 0;
param M3{rack in RACK}, integer, >= 0;
param m4, integer, >= 0;

# Wspolczynniki
# w1 - powierzchnia (m2) racka producenta i
# w2 - liczba maszyn w racku (staly rozmiar)
# w3 - pobor pradu (miesieczny, kWh) przez jeden serwer

param w1{rack in RACK}, integer, >= 0;
param w2, integer, >= 0;
param w3{serv in SERV}, >= 0;

# Zmienne modelu
# x1 - ilosc m2 dzialki
# X2[i] - liczba serwerow producenta i
# X3[i] - liczba rackow producenta i
# x4 - liczba kWh

var x1, integer, >= 0;
var x2{serv in SERV}, integer, >= 0;
var x3{rack in RACK}, integer, >= 0;
var x4, >= 0;

# Funkcja celu
minimize z: x1*l1 + (sum{serv in SERV} L2[serv]*x2[serv]) + (sum{rack in RACK} L3[rack]*x3[rack]) + x4*l4;

# Ograniczenia
s.t. c1: x1 <= m1; 							# powierzchnia nie moze przekroczyc maksymalnej zadanej
s.t. c2: sum{serv in SERV} x2[serv] >= m2; 				# musimy miec minimum m2 serwerow
s.t. c3: sum{rack in RACK} (w2*x3[rack]) >= sum{serv in SERV} x2[serv]; # musimy miec miejsce w rackach na serwery (stalego rozmiaru, standard)
s.t. c4: sum{rack in RACK} (w1[rack]*x3[rack]) <= m1;			# musimy miec miejsce na racki
s.t. c5: sum{serv in SERV} (w3[serv]*x2[serv]) == x4;			# zaleznosc ilosci maszyn i pobranego calosciowego pradu
s.t. c6: x4 <= m4;							# nie mozemy przekroczyc poboru pradu przez wszystkie maszyny
s.t. c7{serv in SERV}: x2[serv] <= M2[serv];				# dla kazdego producenta serwerow: nie mozemy przekroczyc ilosci dostepnych
s.t. c8{rack in RACK}: x3[rack] <= M3[rack];				# dla kazdego producenta rackow: nie mozemy przekroczyc liczby dostepnych
s.t. c9: sum{serv in SERV} M2[serv] >= m2;				# suma dostepnych musi byc wieksza od minimalnej liczby
s.t. c10: sum{rack in RACK} M3[rack] >= m3;				# suma dostepnych musi byc wieksza od minimalnej liczby
