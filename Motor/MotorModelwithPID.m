clear all;
clc;

Gear = 65.5;
Ka = (((1.63/1000)*60)/(2*pi))*Gear;
La = 1.16*10^-3;
Ra = 1.93;
Jm = (2.6*10^-5);
Jpl = 13.95*10^-3;
Je = Jm*Gear + Jpl;
Bm = 1.2*10^-5;
Bpl = .01;
Be = Bm*Gear + Bpl;
Eff = .66;
Tfriction = 0.14136;

A = [-(Ra/La) -(Ka/La); (Ka*Eff/Je) -(Be/Je)];
B = [1/La 0;0 -1/Je];
C = [1 0;0 1];
D = [0 0;0 0];

MotorSS = ss(A,B,C,D);

figure(1);
[u,t] = gensig('square',1/500,.15,.00001);
U=[u*12 Tfriction*ones(length(u),1)];
lsim(MotorSS,U,t);

figure(2);
U=[ones(length(u),1)*6 Tfriction*ones(length(u),1)];
lsim(MotorSS,U,t);

figure(3);
PWM = 12;
B = [1/La;0];
C = [0 1];
D = [0];
MotorSS = ss(A,B,C,D);
C=pidtune(PWM*MotorSS,'pi');
System=feedback(C*(PWM*MotorSS),1);
input = [ones(1000,1)*1;ones(1000,1)*2;ones(1000,1)*3;ones(1000,1)*4];
Inputs = [input];
lsim(System,Inputs,(0:(length(input)-1))*0.001)