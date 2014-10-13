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
    % 
    %   2.3305e+003
    % 
    % 
    % den =
    % 
    %     1.0000  143.0906

sysG=tf(num,den)    
    % 
    %  
    % Transfer function:
    %   2331
    % ---------
    % s + 143.1
sysT=feedback(sysG,143.1/2331)    
zpk(sysT)    
    %
  Ts=0.05   
sysZOH = c2d(sysG,Ts,'zoh')
    % Transfer function:
    %     16.27
    % -------------
    % z - 0.0007813
    %  
    % Sampling time: 0.05
sysFOH = c2d(sysG,Ts,'foh')
    % Transfer function:
    % 14.01 z + 2.262
    % ---------------
    %  z - 0.0007813
    %  
    % Sampling time: 0.05
Ts = 0.05

z=tf('z',Ts)
s=2/Ts*(z-1)/(z+1) 
    % Transfer function:
    % 40 z - 40
    % ---------
    %   z + 1
    %  
    % Sampling time: 0.05
sysZ=16.29/(0.006989*s + 1)
    % Transfer function:
    % 16.29 z + 16.29
    % ---------------
    % 1.28 z + 0.7204
    %  
    % Sampling time: 0.05
% use 24V for input 
% From the datasheet we should get the max speed 388 rad/s when no-load
% we ca use
[y,t,x]=step(24*sysG)
max(y)
    % ans =
    % 
    %   389.5671
% or 
t=0:.01:10
u=24*ones(1,1001)
[y,t,x] =lsim(sysG,u,t)
% get the max speed
mean(y(901:1001k))
    % ans =
    % 
    %   390.8920
% the output speed from the motor model is 390.89 RPM
% In the Lab, when we apply 24V on the motor, the test speed is 3508RPM
