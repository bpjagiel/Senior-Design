%Modeling of 12V Pittman 8224 motor with G35A wide-faced gear box
%% Set-up and motor variables
clc
clear all
Ke = 0.01;%Nm
Kt = Ke;%Due to SI units
Km = Kt;%Therefore we have only one constant, a motor constant Km
Bm = 1.34e-6;% Viscous damping factor,[Nm*s/rad]
Jm = 1.62e-6;%Inertia of rotor, [kg*m^2]
La = 0.58e-3;%Armature Inductance, [H]
Ra = 1.17;%Armature Resistance, [Ohm]
n = 60.5;% Ratio of G35A gear box => wm/wl
Eff = 0.66;% Efficeincy of gear-box, losses due to gear box gear friction
neff = n*Eff;%Effective gear ratio of gear-box due to friction losses
Inl = 0.37;%No load current draw

%% Mathematical modeling of motor transfer function
% Load is reffered to motor via effective gear ratio
% Model obtained from <A Journey from Robot to Digital Human>, pg. 298
% By: Edward Y.L. Gu, Springer, 2013
num = [