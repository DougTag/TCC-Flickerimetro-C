clc; close all;
s = tf('s');
format long;

signum = @(x) (x<0).*-1 + (x>=0); % Definição da função signum
         
%% Construção do sinal e configurações
fs = 20000;
Ts = 1/fs;

f_m_array = [0.325 0.917 8.8 13.5 33.3];
d_E_array = [2.5386 2.0473 0.6832 0.7780 2.0027]/100;

for i = 1:length(f_m_array)
    Tm = 1/f_m_array(i);
    f_m = f_m_array(i);
    d_E = d_E_array(i);

    [Ts_num, Ts_den] = rat(Ts);
    [Tm_num, Tm_den] = rat(Tm);

    % T = lcm(Tm, Ts), e como lcm(n1/d1, n2/d2) = lcm(n1, n2)/gcd(d1, d2):
    T = lcm(Ts_num, Tm_num)/gcd(Ts_den, Tm_den)

    t = (0:Ts:T-Ts)';

    u_E = (1 - 0.22/2 * cos(2*pi*100*t)).*(1+d_E/2*signum(sin(2*pi*f_m*t)));
    u_E_max = max(u_E);

    figure; plot(t, u_E); title('Sinal gerado u_E')
    
    filename = sprintf('waveform_%d.wav', i);
    audiowrite(filename, u_E/u_E_max, fs);
end


