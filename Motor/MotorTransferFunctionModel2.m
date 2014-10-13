% modeling Motor
% Pittman 14204S006 Lo-Cog DC servo Motor
% Encoder resulation 500 CPR
Kt=6.12E-02     % N-M
Ke=6.12E-02     % V/rad/s
Ra=1.01         % ohm
L=1.6e-3           % H
Jm= 2.6E-5      % kg-m^2
Tf=1.6          % N-m  friction torque
b=1.2e-5        % friction , Viscous damping N-M/s

% From text book p.55 eq(2.71)
% we get
K= Kt/(b*Ra+Kt*Ke)
Tau=Ra*Jm/(b*Ra+Kt*Ke)
    % K =
    %    16.2872
    % Tau =
    %     0.0070
    
%  Rad/s   ( Output)
% -------
% Voltage( Input )
num=[K/Tau]
den=[Tau/Tau 1/Tau]
    % num =
    %   2.3305e+003
    % den =
    %     1.0000  143.0906

sysG=tf(num,den)    
    % Transfer function:
    %   2331
    % ---------
    % s + 143.1
sysT=feedback(sysG,143.1/2331)    
zpk(sysT)    

numC1=[0.0578/0.00012^2]
denC1=[0.00012^2/0.00012^2 0.00021/0.00012^2 1/0.00012^2 ]
 sysC1=tf(numC1,denC1)
 step(sysC1)

 step(sysC1*sysG)
rlocus(sysC1*sysG)

sysT1=feedback(sysC*sysG,1)
zpk(sysT1) 
step(sysT1)

numC2=[0.0035/0.0035 1/0.0035]*492
denC2=[1 ]
sysC2=tf(numC2,denC2)
sysT2=feedback(sysC2*sysG,1)
zpk(sysT2)
step(sysT2)















