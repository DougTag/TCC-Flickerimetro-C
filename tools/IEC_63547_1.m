clc; close all;
s = tf('s');
format long;

signum = @(x) (x<0).*-1 + (x>=0); % Definição da função signum
%c = 4;  % Constante para o tempo de acomodação dos filtros. É usado para
         % descartar os primeiros c * tau segundos do sinal a cada filtro
         % aplicado. Para c = 3 -> 95%, c=4 -> 98%, c=5 -> 99%
         % No final das contas, não foi necessário.
         
%% Construção do sinal e configurações
SYSTEM_TYPE = '230V/50Hz'; % '120V/60Hz' ou '230V/50Hz'
T_short = 2; % Tempo de amostragem do sinal, em minutos
fs = 2000;
Ts = 1/fs;
t = (0:Ts:T_short*60)';

% Sinal de calibração, definido no bloco C. Deve fazer mean(P_inst)=1
%u_E = ones(size(t,2),1) + 0.00630/2 * sin(2*pi*8.8*t); 

% Sinal de verificação. Comente/descomente para testar outros parâmetros.
%f_m = 0.325;    d_E = 2.5386/100;   % deu P_st = 0.999502008810051
f_m = 0.9167;   d_E = 2.0473/100;   % deu P_st = 0.999708407522475
%f_m = 8.8;      d_E = 0.6832/100;   % deu P_st = 0.999586092295688
%f_m = 13.5;     d_E = 0.7780/100;   % deu P_st = 1.000236984002230
%f_m = 33.3;     d_E = 2.0027/100;   % deu P_st = 1.005575356781761
u_E = (1 - 0.22/2 * cos(2*pi*100*t)).*(1+d_E/2*signum(sin(2*pi*f_m*t)));

%figure; plot(t, u_E); xlim([0 2]); ylim([0 1.2]); title('Sinal de entrada u_E')


%% BLOCO A - Normalização
% Nos primeiros ~40s o filtro ainda não carregou.

% Para obter a média do sinal de entrada. Constante de tempo de 10s.
F_Ad = c2d(1/(10*s+1), Ts, 'tustin')

u_E_media = filter(F_Ad.Numerator{1}, F_Ad.Denominator{1}, u_E);
%figure; plot(t, u_E_media); xlim([0 100]);ylim([0 1.2]); title('Média de u_E');

E = u_E./u_E_media;

% Tirar os primeiros 40s por causa do filtro ainda não ter carregado
%E = E(c*10*fs:end);
%t = t(c*10*fs:end);

%figure; plot(t, E); ylim([0 1.2]); xlim([40 41]); title('Sinal normalizado E')


%% BLOCO B - Frequências de flicker visível e resposta olho-cérebro

F_B_PA = s/(s+0.05*2*pi);  % Constante de tempo: ~3.18s

switch (SYSTEM_TYPE)
    case '120V/60Hz'
        [b, a] = butter(6, 2*pi*42, 'low', 's');
        F_B_PB = tf(b, a);

        k = 1.6357;
        lambda = 2*pi*4.167375;
        w1 = 2*pi*9.07169;
        w2 = 2*pi*2.939902;
        w3 = 2*pi*1.394468;
        w4 = 2*pi*17.31512;
    case '230V/50Hz'
        [b, a] = butter(6, 2*pi*35, 'low', 's');
        F_B_PB = tf(b, a);

        k = 1.74802;
        lambda = 2*pi*4.05981;
        w1 = 2*pi*9.15494;
        w2 = 2*pi*2.27979;
        w3 = 2*pi*1.22535;
        w4 = 2*pi*21.9;
    otherwise
        fprintf('Unknown system type')
        return;
end

F_RLOC = k*w1*s/(s^2+2*lambda*s+w1^2) * (1+s/w2)/( (1+s/w3)*(1+s/w4) );
F_RL = 3.57/(0.02*10^-3*s^2 + 21.2*10^-3*s + 1);

F_B_ROC = F_RLOC/F_RL;

% Precisei discretizar cada filtro e aplicar no sinal.
% Se juntar os três (multiplicando as tfs) e aplicar de uma vez, vai ao oo.
F_B_PAd = c2d(F_B_PA, Ts, 'tustin')
Y_B1 = filter(F_B_PAd.Numerator{1}, F_B_PAd.Denominator{1}, E);
%figure; plot(t, Y_B1); ylim([-0.2 0.2]); xlim([40 41]); title('Sinal filtrado Y_B1');

% Esse filtro, de 6a ordem, precisa ser decomposto em sessões de segunda
% ordem
F_B_PBd = c2d(F_B_PB, Ts, 'tustin')
sos = tf2sos(F_B_PBd.Numerator{1}, F_B_PBd.Denominator{1});
F_B_PB1d = tf(sos(1, 1:3), sos(1,4:6), Ts)
F_B_PB2d = tf(sos(2, 1:3), sos(2,4:6), Ts)
F_B_PB3d = tf(sos(3, 1:3), sos(3,4:6), Ts)
Y_B2 = filter(F_B_PBd.Numerator{1}, F_B_PBd.Denominator{1}, Y_B1);
%figure; plot(t, Y_B2); ylim([-0.01 0.01]); xlim([60 63]); title('Sinal filtrado Y_B2');

F_B_ROCd = c2d(F_B_ROC, Ts, 'tustin')
sos = tf2sos(F_B_ROCd.Numerator{1}, F_B_ROCd.Denominator{1});
F_B_ROC1d = tf(sos(1, 1:3), sos(1,4:6), Ts)
F_B_ROC2d = tf(sos(2, 1:3), sos(2,4:6), Ts)
Y_B3 = filter(F_B_ROCd.Numerator{1}, F_B_ROCd.Denominator{1}, Y_B2);
%figure; plot(t, Y_B3); ylim([-0.003 0.003]); xlim([60 63]); title('Sinal filtrado Y_B3 = Y_B');

Y_B = Y_B3;

%{
% Essa parte não foi usada, pois vai ao oo quando se junta os três filtros
F_B = F_B_PA * F_B_PB * F_B_ROC;

F_Bd = c2d(F_B, Ts, 'tustin'); % Constante de tempo final: ~3.5s

Y_B = filter(F_Bd.Numerator{1}, F_Bd.Denominator{1}, E);

% Tirar os primeiros 14s por causa de um dos filtros ainda não ter carregado
Y_B = Y_B(c*3.5*fs:end);
t = t(c*3.5*fs:end);

figure; plot(t, Y_B); ylim([-0.003 0.003]); xlim([60 63]); title('Sinal filtrado Y_B');
%}


%% BLOCO C - Quadratura, média móvel e normalização

% Quadratura
Y_Bquad = Y_B.*Y_B;

% Média móvel
F_C_PBd = c2d(1/(0.3*s + 1), Ts, 'tustin') % Constante de tempo: 0.3s

Y_C = filter(F_C_PBd.Numerator{1}, F_C_PBd.Denominator{1}, Y_Bquad);
figure; plot(t, Y_C); ylim([0 0.000003]); xlim([60 63]); title('Y_C');

% Tirar os primeiros 1.2s por causa do filtro ainda não ter carregado
%Y_C = Y_C(c*0.3*fs:end);
%t = t(c*0.3*fs:end);

S = 1.101603155420234e+06; % Fator de escala. É determinado ao fazer S=1 e 
                           % calcular o P_inst do sinal de calibração, de 
                           % modo a se tornar mean(P_inst)=1.

P_inst = S * Y_C;

% Tiro os primeiros 60s ao final, ao invés de tirar um pouco após cada
% filtro. Notei que não faz diferença, e é mais fácil de implementar
P_inst = P_inst(60*fs:end);
t = t(60*fs:end);

figure; plot(t, P_inst); ylim([0 3]); xlim([60 63]); title('Perceptividade de cintilação instantânea P_{inst}');

%% BLOCO D - Análise estatística
P = prctile(P_inst, 100-[0.1 0.7 1 1.5 2.2 3 4 6 8 10 13 17 30 50 80]);

P_1_s = mean([P(2) P(3) P(4)]);
P_3_s = mean([P(5) P(6) P(7)]);
P_10_s = mean([P(8) P(9) P(10) P(11) P(12)]);
P_50_s = mean([P(13) P(14) P(15)]);

P_st = sqrt(0.0314*P(1) + 0.052*P_1_s + 0.0657*P_3_s + 0.28*P_10_s + 0.08*P_50_s)

%% OUTROS
% Exportar P_inst para testar a minha implementação do T-Digest
fileID = fopen('sinal.txt', 'w');  % Abre arquivo para escrita
fprintf(fileID, '%.6f\n', P_inst);   % Escreve cada valor em uma linha
fclose(fileID);

% Imprimir os numeradores e denominadores dos filtros
fprintf("static float F_Ad_num[] = {%.20e, %.20e};\n", F_Ad.Numerator{1}(1), F_Ad.Numerator{1}(2))
fprintf("static float F_Ad_den[] = {%.20e, %.20e};\n", F_Ad.Denominator{1}(1), F_Ad.Denominator{1}(2))

fprintf("static float F_B_PAd_num[] = {%.20e, %.20e};\n", F_B_PAd.Numerator{1}(1), F_B_PAd.Numerator{1}(2))
fprintf("static float F_B_PAd_den[] = {%.20e, %.20e};\n", F_B_PAd.Denominator{1}(1), F_B_PAd.Denominator{1}(2))

fprintf("static float F_B_PB1d_num[] = {%.20e, %.20e, %.20e};\n", F_B_PB1d.Numerator{1}(1), F_B_PB1d.Numerator{1}(2), F_B_PB1d.Numerator{1}(3))
fprintf("static float F_B_PB1d_den[] = {%.20e, %.20e, %.20e};\n", F_B_PB1d.Denominator{1}(1), F_B_PB1d.Denominator{1}(2), F_B_PB1d.Denominator{1}(3))

fprintf("static float F_B_PB2d_num[] = {%.20e, %.20e, %.20e};\n", F_B_PB2d.Numerator{1}(1), F_B_PB2d.Numerator{1}(2), F_B_PB2d.Numerator{1}(3))
fprintf("static float F_B_PB2d_den[] = {%.20e, %.20e, %.20e};\n", F_B_PB2d.Denominator{1}(1), F_B_PB2d.Denominator{1}(2), F_B_PB2d.Denominator{1}(3))

fprintf("static float F_B_PB3d_num[] = {%.20e, %.20e, %.20e};\n", F_B_PB3d.Numerator{1}(1), F_B_PB3d.Numerator{1}(2), F_B_PB3d.Numerator{1}(3))
fprintf("static float F_B_PB3d_den[] = {%.20e, %.20e, %.20e};\n", F_B_PB3d.Denominator{1}(1), F_B_PB3d.Denominator{1}(2), F_B_PB3d.Denominator{1}(3))

fprintf("static float F_B_ROC1d_num[] = {%.20e, %.20e, %.20e};\n", F_B_ROC1d.Numerator{1}(1), F_B_ROC1d.Numerator{1}(2), F_B_ROC1d.Numerator{1}(3))
fprintf("static float F_B_ROC1d_den[] = {%.20e, %.20e, %.20e};\n", F_B_ROC1d.Denominator{1}(1), F_B_ROC1d.Denominator{1}(2), F_B_ROC1d.Denominator{1}(3))

fprintf("static float F_B_ROC2d_num[] = {%.20e, %.20e, %.20e};\n", F_B_ROC2d.Numerator{1}(1), F_B_ROC2d.Numerator{1}(2), F_B_ROC2d.Numerator{1}(3))
fprintf("static float F_B_ROC2d_den[] = {%.20e, %.20e, %.20e};\n", F_B_ROC2d.Denominator{1}(1), F_B_ROC2d.Denominator{1}(2), F_B_ROC2d.Denominator{1}(3))

fprintf("static float F_C_PBd_num[] = {%.20e, %.20e};\n", F_C_PBd.Numerator{1}(1), F_C_PBd.Numerator{1}(2))
fprintf("static float F_C_PBd_den[] = {%.20e, %.20e};\n", F_C_PBd.Denominator{1}(1), F_C_PBd.Denominator{1}(2))